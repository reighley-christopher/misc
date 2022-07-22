package chaintools

import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._
import org.apache.kafka.streams.scala._
import org.apache.kafka.streams.processor._ 
import org.apache.kafka.streams.scala.kstream._
import org.apache.kafka.streams.KafkaStreams
//import scala.collection.JavaConverters._
import scala.jdk.CollectionConverters._
import scala.collection.mutable.Set
import reighley.christopher.Properties.properties
import java.time.Duration

object Kafka {

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

//TODO this was copied from KafkaInterface because I want my own consumer for this module  
object A {
  val admin = getAdmin()
  val producer = getProducer()
  val consumer = getDefaultConsumer()
  def topics() = admin.listTopics().names().get()
  def addTopic( name:String ) = {
    import java.util.Vector
    val topic = new Vector[NewTopic](1)
    topic.add(0, new NewTopic( name, 1, 1:Short ) ) 
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

}


class KafkaIteratorRecord( _data : ConsumerRecords[String,String] ) {
  var next : KafkaIteratorRecord = null
  var data : ConsumerRecords[String,String] = _data
  }

class KafkaIterator( name : String ) extends Iterator[AnnotatedString] {

  var rec = KafkaPollData.latest 
  var iter = rec.data.records(name).iterator()

  def getNext : Boolean = {
    if ( rec.next != null ) { 
      rec = rec.next 
      iter = rec.data.records(name).iterator()
      return iter.hasNext 
      } else {
      KafkaPollData.getMore
      if ( rec.next != null ) {
        rec = rec.next
        iter = rec.data.records(name).iterator()
        return iter.hasNext 
        } else {
        return false
        }
      }
    }

  def hasNext : Boolean = { 
   if ( iter.hasNext ) { return true } else { while( !getNext ) { Thread.`yield` }  }
   return true
   }
 
  def next() = {
    val item = iter.next 
    val ret = new AnnotatedString( item.value, "key" -> item.key ) 
    ret
    }

  }

object KafkaPollData {
  /*this will keep track of the most current fetch and supply iterators*/
  var latest : KafkaIteratorRecord = A.consumer.synchronized { new KafkaIteratorRecord( A.consumer.poll(Duration.ofMillis(100)) ) } 
  def getIterator( name : String ) : KafkaIterator = new KafkaIterator( name )
  def getMore = A.consumer.synchronized {
    if( latest.next == null ) {
      latest.next = new KafkaIteratorRecord( A.consumer.poll(Duration.ofMillis(100)) )
      if( latest.next != null ) { latest = latest.next }
      }
    }
  }

/*TODO to make focus_game work I will need a chaintools node index which takes a string and gives it attributes*/
class KafkaInterface(name: String) extends ChainSink[AnnotatedString](  { (data) => A.producer.send( new ProducerRecord( name, data.get("key"), data.body ) ) } ) with ChainHead[AnnotatedString] {
  if( !A.topics().contains(name) ) { A.addTopic(name) }
  A.subscribe(name)
  def iterator = KafkaPollData.getIterator(name) 
  
  override def toString = "kafka("+name+")" 
  }
// ... first create a sequence in a managable form

def kafka(name: String):KafkaInterface = new KafkaInterface(name)

} 
