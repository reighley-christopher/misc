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

import reighley.christopher.Tableizer
import reighley.christopher.Interactive
import reighley.christopher.CommandDelegate
import reighley.christopher.Util.dir

import scala.util.{ Try, Success, Failure }

import kafkainterface.ConsumerProducerAdmin
import kafkainterface.Parser

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

//TODO clean up tableizer lock files and thread  

object A {
  val admin = getAdmin()
  val producer = getProducer()
  val consumer = getDefaultConsumer()
  val threadm = new ConsumerProducerAdmin(consumer, producer, admin)
  threadm.start()

  def p(name:String, partition:Int) = new org.apache.kafka.common.TopicPartition(name, partition)
  def P(name:String, partition:Int) = Set(p(name, partition)).asJava

  //TODO if I have a lot of data (for instance from a feedback loop), a topic may be too large to dump
  //maybe add the ability to rewind some limited amount 
  def parser = new Parser(threadm)
  def run = parser.run(_)
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


