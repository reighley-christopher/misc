import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._
import org.apache.kafka.streams.scala._
import org.apache.kafka.streams.processor._ 
import org.apache.kafka.streams.scala.kstream._
import org.apache.kafka.streams.KafkaStreams
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.File
import java.util.Properties
import java.lang.reflect.Method
import chaintools._
import chaintools.ChainTools._
import chaintools.HTTPService._
import chaintools.Riak._

import scala.collection.mutable.Set
import scala.collection.mutable.Queue
import scala.collection.mutable.Map
import scala.collection.JavaConverters._
import scala.collection.JavaConversions._
import scala.util.control.NonFatal

import scala.util.parsing.json.JSON

import org.apache.kafka.streams.kstream.Materialized
import org.apache.kafka.streams.scala.ImplicitConversions._
import org.apache.kafka.streams.scala._
import Serdes._
import org.apache.kafka.streams.kstream.Transformer
import org.apache.kafka.streams.kstream.TransformerSupplier
import org.apache.kafka.streams.KeyValue
import org.apache.kafka.streams.state.KeyValueBytesStoreSupplier
import org.apache.kafka.streams.state.KeyValueStore
import org.apache.kafka.streams.scala.ByteArrayKeyValueStore
import org.apache.kafka.streams.state.ValueAndTimestamp
import java.util.Date
import java.text.SimpleDateFormat
import java

import scala.util.{ Try, Success, Failure }

val properties = new Properties()
properties.load(new FileInputStream("kafkainterface.properties"))

def getAdmin():AdminClient = {
  AdminClient.create( properties ) 
  }

def getProducer():Producer[String, String] = {
  new KafkaProducer[String,String](properties)
  }

//getConsumer is a mess because I'm running this exposed in the console
//so I can't overload and also subscribe is burried inside A which is means I
//copied and pasted the code
//TODO clean this up

def getDefaultConsumer():Consumer[String, String] = {
  new KafkaConsumer[String,String](properties)
  }

def dir( example:Object ):Array[String] = {
  example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) 
  }

class ThreadMaster(val consumer:Consumer[String,String], val producer:Producer[String, String] ) {
  val kafka_to_kafka:Map[String,Set[String]] = Map() //kafka ins are Maps because we get the input in random order and need to index in
  val kafka_to_file:Map[String,Set[String]] = Map()
  val file_to_kafka:Set[(String,String)] = Set() //TODO implement file to kafka requires figuring out how to identify a key
  val file_to_file:Set[(FileInputStream,FileOutputStream)] = Set() //we will have to open the files ourselves  
  val tables:Set[(String, Unit)] = Set() //TODO put in the tabelizer
  val shuntfiles:Set[String]  = Set()
  var okay_to_die:Boolean = false
  var be_quiet = true //TODO start as false if subscriptions already exist 
  val thr = new Thread { override def run() = inner_loop }
  //val stdout_fifo = new FileInputStream("shunts/display") //this line will cause the process to block as soon as the object is created
  //TODO obviously unnecessary copy/paste refactor this whole thing so A and ThreadMaster are the same object
  def subscribe( name:String ) = consumer.synchronized {
    val subs:Set[String] = consumer.subscription.asScala.clone
    subs.add(name)
    if ( !consumer.subscription.contains(name) ) consumer.subscribe(subs.asJava)
    be_quiet = false
    }

  def start() = { thr.start() }

  def create_kafka_to_kafka_shunt(inp:String, outp:String) = {
    if(!kafka_to_kafka.contains(inp)) 
      {
      kafka_to_kafka(inp) = Set()
      subscribe(inp)
      } 
      kafka_to_kafka(inp).add(outp) 
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
            } else print(r) 
          }
        } 
      }
    }
  }

object A {
  val admin = getAdmin()
  val producer = getProducer()
  val consumer = getDefaultConsumer()
  val threadm = new ThreadMaster(consumer, producer)
  threadm.start()
  def topics() = admin.listTopics().names().get()
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

  def subscribe( name:String ) = consumer.synchronized {
    val subs:Set[String] = consumer.subscription.asScala.clone
    subs.add(name)
    if ( !A.consumer.subscription.contains(name) ) consumer.subscribe(subs.asJava)
    }

  def p(name:String, partition:Int) = new org.apache.kafka.common.TopicPartition(name, partition)
  def P(name:String, partition:Int) = Set(p(name, partition)).asJava

  def pretty_print( records:ConsumerRecords[String, String] ):String = {
    var list:List[String] = List[String]()
    records.forEach( new java.util.function.Consumer[ConsumerRecord[String,String]]() { 
      override def accept(rec:ConsumerRecord[String,String]) = { 
        list = list :+ ( rec.key().asInstanceOf[String] + " " + rec.value().asInstanceOf[String] ) //TODO this should prepend, and the reverse the list
        }
      } )
    list.mkString("\n") 
    }

  //TODO if I have a lot of data (for instance from a feedback loop), a topic may be too large to dump
  //maybe add the ability to rewind some limited amount 
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
  
  def files() = (new File("shunts")).list()  

  def list() = topics().toArray().mkString("\n") + "\n----\n" + files().mkString("\n")
  def tableize(topic:String, filename:String) = "UNIMPLEMENTED\n"
  def untableize(topic:String) = "UNIMPLEMENTED\n"
  def listen(topic:String) = "UNIMPLEMENTED\n"
  def unlisten(topic:String) = "UNIMPLEMENTED\n"
  def shunt(inpu:String, outpu:String) = {threadm.create_kafka_to_kafka_shunt(inpu, outpu) } //TODO other kinds of shunts 
  def unshunt(endpoint:String) = "UNIMPLEMENTED\n"
  def info() = {
    /*
    what I want is : 
    for each topic, the subscribed consumer groups, their offsets
    some topic specific detail would be nice too but that seems hard to come by (need to break topic out by logDir, id, rack, broker or whatever
    in particular being able to detect when a topic is growing very fast without being read enough
    TODO I hate everything about this
    I am assuming one partition per consumer but conceivably a single consumer could be working on multiple partitions and have multiple offsets
    */
    val consumerGroups:Iterable[String] = collectionAsScalaIterable(A.admin.listConsumerGroups.all.get).map( (x) => x.groupId )
    val topics = Map[String, Map[String, Long] ]()
    for(name <- consumerGroups) {
      val offs = A.admin.listConsumerGroupOffsets(name).partitionsToOffsetAndMetadata.get.asScala.toArray
      for( pair <- offs ) {
        val topi = pair._1.topic
        if( !topics.keys.contains( topi ) ) { topics(topi) = Map[String, Long]() }
        val offs = topics(topi)
        offs(name) = pair._2.offset 
        }
      }
    topics.mkString //TODO format this better
    } 

  def run( command:String ):String = {
    val cmd = command.split(" ")
    cmd(0) match {
      case "list" => list() 
      case "tableize" => tableize(cmd(1), cmd(2))
      case "untableize" => untableize(cmd(1))
      case "listen" => listen(cmd(1))
      case "unlisten" => unlisten(cmd(1))
      case "dump" => dump(cmd(1))
      case "shunt" => shunt(cmd(1), cmd(2)) ; ""
      case "unshunt" => unshunt(cmd(1))
      case "info" => info()
      case _ => """commands :
                  |list
                  |tableize
                  |untableize
                  |listen
                  |unlisten
                  |dump
                  |shunt
                  |unshunt
                  |info""".stripMargin
 
      } 
    } 

}

def test() =
  {
  A.run("shunt funnybench_in stupid")
  }

//TODO this belongs to chaintools, which has moved
//make sure these are still acurate and build tests if possible

// I will want to start a process that pulls from a kafka stream or hdfs file and writes to a kafka stream or hdfs file,
// and monitors records processed, keeps track of ongoing efforts in this databasse, logs errors, accepts a Kafka stream graph

 
/*what I want to happen is that when I use a task as a ChainSink, everything before and everything after will be
 separate tasks, with the objects before existing is a separate Thread, 

consider:
sql > detach > sinkprint
it parses as :

(sql > detach ) > sinkprint
this means that sql > detach must have a value
so detach has to be a ChainLink
in which case we would return a new head with a well defined iterator, the links iterator. 

*/ 

//write a select statement and drop the output into a kafka topic


/* A DOCUMENT OF PUZZLING BEHAVIOR

kafka("stupid") > kafka("stupid2") 
transmits no records

val k = kafka("stupid2")
data > kafka("stupid")
kafka("stupid") > k 
k > sinkprint
transmits records
but prints nothing

kafka("stupid") > k
kafka("stupod2") > sinkprint
does print

kafka("stupid2") > k
creates a weird loop

data > kafka("stupid") 
val s1 = kafka("stupid")
val s2 = kafka("stupid2")
s1 > s2
transmits no records
but 
kafka("stupid") > s2 will transmit, with the same s2

so the problem is probably that the left hand side, which is the object for which > and iterator are
called is instantiated after the right hand side for which absorb is called 

why would this make a difference? It doesn't seem to matter how long the two objects have been instantiated,
only that the right hand side needs to be intantiated first 

in the first case data is written before anything else,
then the consumer is created
then the producer is created, but the consumer reads nothing

in the second case the producer, and the second consumer is created first
then the data is written
then the first consumer, and the read is successful
but the read from the second consumer doesn't read (it is created before the data) 
*/


