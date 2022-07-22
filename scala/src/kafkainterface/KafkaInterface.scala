package kafkainterface

import sys.process._
import java.util.Properties

import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._

import reighley.christopher.Interactive
import scala.language.postfixOps
//temp for testing
//import java.io.FileInputStream 
//import org.apache.kafka.clients.admin._
//import org.apache.kafka.clients.producer._
//import org.apache.kafka.clients.consumer._
//import org.apache.kafka.streams.scala._
//import org.apache.kafka.streams.processor._ 
//import org.apache.kafka.streams.scala.kstream._
//import org.apache.kafka.streams.KafkaStreams
//import org.apache.kafka.streams.kstream.Materialized
//import org.apache.kafka.streams.scala.ImplicitConversions._
//import org.apache.kafka.streams.scala._
//import Serdes._
//import org.apache.kafka.streams.kstream.Transformer
//import org.apache.kafka.streams.kstream.TransformerSupplier
//import org.apache.kafka.streams.KeyValue
//import org.apache.kafka.streams.state.KeyValueBytesStoreSupplier
//import org.apache.kafka.streams.state.KeyValueStore
//import org.apache.kafka.streams.scala.ByteArrayKeyValueStore
//import org.apache.kafka.streams.state.ValueAndTimestamp
//end big blob for testing

object KafkaInterface {

  var linebuffer:String = ""

  def print_above(line:String):Unit =
    {
    if(Interactive.alive)
      {
      Interactive.async_print(line)
      } else {
      print("\r" + line + "\r\nkafka> " + linebuffer)
      }
    }
  
  def print_below(line:String):Unit = 
    {
    print( line.replace("\n", "\r\n") )
    }

  def process_command( parser:Parser, command:String , threadm:ConsumerProducerAdmin):Unit =
    {
    if(command == "exit") { 
       //TODO clean up threadmaster
       System.exit(0) 
       }
    if(command == "test") { 
       threadm.send("stupid2", "key", "value")
       }
    if(command == "yield") {
      Thread.`yield`()
      }
    print_below( parser.run(command) )
    }

  def inner_loop(parser:Parser, threadm:ConsumerProducerAdmin) = 
    {
    if(System.in.available > 0)
      {
      val ch = System.in.read()
      System.out.write(ch)
      System.out.flush()
      if(ch == 3) { System.exit(0) }
      if(ch == '\r') 
        { 
        print("\n") ; process_command(parser, linebuffer, threadm); linebuffer = ""; print("kafka> ") 
        } else if(ch == 127) {
        linebuffer = linebuffer.substring(0, linebuffer.length - 1)
        System.out.write(0x1b)
        System.out.write("[1D".getBytes)
        } else { 
        linebuffer += ch.toChar 
        }
      }
    } 

  def main(params:Array[String]):Unit = {
    "stty -F /dev/tty raw -echo" ! ; //tell linux I want raw mode, no echo (the way God intended)
    val properties = new Properties()
//    properties.load(new FileInputStream("kafkainterface.properties"))
//    print(params(0))

    properties.setProperty("bootstrap.servers", params(0))
    properties.setProperty("client.id", "kafkainterface")
    properties.setProperty("application.id", "kafkainterface")
    properties.setProperty("group.id", "kafkainterface")
    properties.setProperty("key.serializer", "org.apache.kafka.common.serialization.StringSerializer")
    properties.setProperty("value.serializer", "org.apache.kafka.common.serialization.StringSerializer")
    properties.setProperty("key.deserializer", "org.apache.kafka.common.serialization.StringDeserializer")
    properties.setProperty("value.deserializer", "org.apache.kafka.common.serialization.StringDeserializer")

    val admin = AdminClient.create( properties ) 
    val producer = new KafkaProducer[String,String](properties)
    val consumer = new KafkaConsumer[String,String](properties)

    val threadm = new ConsumerProducerAdmin(consumer, producer, admin, print_above)
    val parser = new Parser(threadm)
    threadm.start()
    print("kafka> ")
    while(true) {
      inner_loop(parser, threadm)
      }
    }
  }
