package funnybench.worker

import reighley.christopher.Properties.properties

import chaintools.ChainTools._
import chaintools.HTTPService._
import chaintools.MapDatastore
import chaintools.AnnotatedString
import chaintools.Kafka.A
import chaintools.Kafka.kafka 


/*TODO did I really have to import all this stuff?*/
/*somwhere in here is the implicit declaration of which Serdes to use for the consumed parameter when declaring a stream
  and we error out otherwise I should be able to strip out most of this stuff*/
import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._
import org.apache.kafka.streams.scala._
import org.apache.kafka.streams.processor._ 
import org.apache.kafka.streams.scala.kstream._
import org.apache.kafka.streams.KafkaStreams

import Serdes._

import org.apache.kafka.streams.kstream.Materialized
import org.apache.kafka.streams.scala.ImplicitConversions._
import org.apache.kafka.streams.scala._
import org.apache.kafka.streams.kstream.Transformer
import org.apache.kafka.streams.kstream.TransformerSupplier
import org.apache.kafka.streams.KeyValue
import org.apache.kafka.streams.state.KeyValueBytesStoreSupplier
import org.apache.kafka.streams.state.KeyValueStore
import org.apache.kafka.streams.scala.ByteArrayKeyValueStore
import org.apache.kafka.streams.state.ValueAndTimestamp

//TODO create a TransformSupplier that take wraps part of a chain so I can use them interchangeably 

class Worker {
  val datastore = new MapDatastore[String]
  def teststub(key:String, value:String):(String,String) = (key, value.toUpperCase)
   
  def start() = {
    A.subscribe("funnybench_in")
    A.subscribe("funnybench_out")
    val service = detach(http("localhost", 1100, "/", datastore))
    /*service has two functions*/
    /*data comes in from some source to be sent out in response to GET requests, and data that comes in from put requests goes out to somewhere else
      in general these will be different sources so I can't write it as a single line. */
    kafka("funnybench_out") > service 
    service > kafka("funnybench_in")

    /*don't really need a kafka Stream in here but lets build one for testing and logging*/ 
    val builder = new StreamsBuilder()

    /*the test stub will process the input and capitalize it*/
    builder.stream[String,String]("funnybench_in").map(teststub).to("funnybench_out") 
    val stream = new KafkaStreams(builder.build, properties)
    stream.start()
    }
  }
