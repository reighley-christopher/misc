package kafkainterface

import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._
import org.apache.kafka.streams.scala.ImplicitConversions._

import scala.collection.mutable.Set
import scala.collection.mutable.Map
import scala.collection.JavaConverters._
import scala.collection.JavaConversions._

import java.io.FileInputStream
import java.io.FileOutputStream

import scala.util.parsing.json.JSON

import reighley.christopher.Tableizer

class ConsumerProducerAdmin(val consumer:Consumer[String,String], val producer:Producer[String, String], val admin:AdminClient, val local_print:String => Unit = print ) {
  val kafka_to_kafka:Map[String,Set[String]] = Map() //kafka ins are Maps because we get the input in random order and need to index in
  val kafka_to_file:Map[String,Set[String]] = Map()
  val file_to_kafka:Set[(String,String)] = Set() //TODO implement file to kafka requires figuring out how to identify a key
  val file_to_file:Set[(FileInputStream,FileOutputStream)] = Set() //we will have to open the files ourselves  
  val listens:Set[String] = Set()
  val tables:Map[String, Tableizer] = Map() 
  val shuntfiles:Set[String]  = Set()
  var okay_to_die:Boolean = false
  var be_quiet = true //TODO start as false if subscriptions already exist

  //putting this the top so it annoys me into changing it 
  //TODO fix bad part
  /****************************************** bad part ************************************/
  def info() = {
    /*
    what I want is : 
    for each topic, the subscribed consumer groups, their offsets
    some topic specific detail would be nice too but that seems hard to come by (need to break topic out by logDir, id, rack, broker or whatever
    in particular being able to detect when a topic is growing very fast without being read enough
    TODO I hate everything about this
    I am assuming one partition per consumer but conceivably a single consumer could be working on multiple partitions and have multiple offsets
    */
    val consumerGroups:Iterable[String] = collectionAsScalaIterable(admin.listConsumerGroups.all.get).map( (x) => x.groupId )
    val topics = Map[String, Map[String, Long] ]()
    for(name <- consumerGroups) {
      val offs = admin.listConsumerGroupOffsets(name).partitionsToOffsetAndMetadata.get.asScala.toArray
      for( pair <- offs ) {
        val topi = pair._1.topic
        if( !topics.keys.contains( topi ) ) { topics(topi) = Map[String, Long]() }
        val offs = topics(topi)
        offs(name) = pair._2.offset 
        }
      }
    topics.mkString //TODO format this better
    } 

  def pretty_print( records:ConsumerRecords[String, String] ):String = {
    var list:List[String] = List[String]()
    records.forEach( new java.util.function.Consumer[ConsumerRecord[String,String]]() { 
      override def accept(rec:ConsumerRecord[String,String]) = { 
        list = list :+ ( rec.key().asInstanceOf[String] + " " + rec.value().asInstanceOf[String] ) //TODO this should prepend, and the reverse the list
        }
      } )
    list.mkString("\n") 
    }

  def dump(topic:String):String = {
    var list:List[String] = List[String]() 
    consumer.synchronized { 
      //hopefully the thread synchronization will prevent dump from resetting the shunts, 
      //but if new records come in while dumping shunt will probably miss them 
      subscribe(topic)
      consumer.seekToBeginning(consumer.assignment())  //TODO oh wow we seek everybody to the beginning hmmm
      var records = consumer.poll(10000) //if the offset is large seek may take a long time and paradoxically the first poll will return no records at all
      while(records.count() > 0) {
        print(records.count)
        list = list :+ pretty_print(records)
        records = consumer.poll(1000) 
        }
      }
    list.mkString("\n") 
    }

  /******************************************** end bad part ***********************************/
 
  val thr = new Thread { override def run() = inner_loop }
  //val stdout_fifo = new FileInputStream("shunts/display") //this line will cause the process to block as soon as the object is created
  def subscribe( name:String ) = consumer.synchronized {
    val subs:Set[String] = consumer.subscription.asScala.clone
    subs.add(name)
    if ( !consumer.subscription.contains(name) ) consumer.subscribe(subs.asJava)
    be_quiet = false
    }

  def start() = { thr.start() }

  def topics() = admin.listTopics().names().get()

  def listen(topic:String) = 
    {
    listens.add(topic) 
    subscribe(topic)
    }

  def send( topic:String, key:String, value:String ) = 
    {
    val rec = new ProducerRecord( topic, key, value )
    producer.send(rec)
    }

  def create_kafka_to_kafka_shunt(inp:String, outp:String) = {
    if(!kafka_to_kafka.contains(inp)) 
      {
      kafka_to_kafka(inp) = Set()
      subscribe(inp)
      } 
      kafka_to_kafka(inp).add(outp) 
    }

  def create_kafka_to_file_shunt(inp:String, outp:String) = {
    if(!kafka_to_file.contains(inp))
      {
      kafka_to_file(inp) = Set()
      subscribe(inp)
      }
      kafka_to_file(inp).add(outp)
    }

  def tableize(inp:String, outp:String) = {
    if(!tables.contains(inp))
      {
      tables(inp) = new Tableizer(outp, "key")
      subscribe(inp)
      }
    }

  def write_to_file(filename:String, data:String)
    {
    //TODO save old file handles, find a way not to hang on fifos
    val str = new FileOutputStream(filename, true)
    str.write( data.getBytes )
    str.close() 
    } 

  def addTopic( name:String ) = {
    import java.util.Vector
    val topic = new Vector[NewTopic](1)
    topic.add(0, new NewTopic( name, 1, 1 ) ) 
    admin.createTopics(topic)
    }

  def removeTopic( name:String ) = {
    import java.util.Vector
    val topic = new Vector[java.lang.String](1)
    topic.add(0, name ) 
    admin.deleteTopics(topic)
    }

  def inner_loop() = {
    while(! okay_to_die ) {
      if( be_quiet ) Thread.`yield` 
      else
        
        consumer.synchronized {
        val records = consumer.poll(1000)
        for(r <- records)
          {
          val topic = r.topic()
          if(kafka_to_kafka.contains(topic)) for( s <- kafka_to_kafka(topic) )
            {
            producer.send(new ProducerRecord(s, r.key(), r.value()))
            } 
          if(kafka_to_file.contains(topic)) for( s <- kafka_to_file(topic) )
            {
            write_to_file( s, r.value() ) 
            }
          if(tables.contains(topic))
            {
            val dict = JSON.parseFull( r.value() ).get.asInstanceOf[ scala.collection.immutable.Map[String, String] ] 
            tables(topic).update_row(Map("key" -> r.key) ++ dict )
            }
          if(listens.contains(topic))
            {
            local_print(r.value() + "\n")
            } 
          }
        } 
      }
    }
  }

