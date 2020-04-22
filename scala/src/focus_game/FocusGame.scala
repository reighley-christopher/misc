package focus_game

import org.apache.kafka.clients.admin._
import org.apache.kafka.clients.producer._
import org.apache.kafka.clients.consumer._
import org.apache.kafka.streams.scala._
import org.apache.kafka.streams.processor._ 
import org.apache.kafka.streams.scala.kstream._
import org.apache.kafka.streams.KafkaStreams
import java.io.FileInputStream
import java.util.Properties
import java.lang.reflect.Method
import chaintools._
import chaintools.ChainTools._
import chaintools.HTTPService._
import chaintools.Riak._

import scala.collection.mutable.Set
import scala.collection.mutable.Queue
import scala.collection.JavaConverters._
import scala.util.control.NonFatal

import scala.util.parsing.json.JSON

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
import java.util.Date
import java.text.SimpleDateFormat

import scala.util.{ Try, Success, Failure }

import chaintools.Kafka.A
import chaintools.Kafka.kafka 

import reighley.christopher.Properties.properties

object ExceptionHandler extends java.lang.Thread.UncaughtExceptionHandler {
  def uncaughtException( t:Thread, e:Throwable ) {
    println(e)
    } 
  }

//basic arrangement of the Streams
//for the moment, permit no user updates or edit the list
//accept an object from the HTTP server
//  - the form will be a single JSON object {"id" : [a user], "event" : [start, cancel, complete], "timestamp" : datetime}
//feed object to kafka stream
//kafka stream includes a table which is updated
//the table will keep track of how many times the running process has recieved a complete event today, and the current state for each user
//the processor should take this information and transform it into an update to the table, but in order to effect that update
//it will need the record from the same table. I will get the record by materializing the table  
//the updated data is periodically synced to the riak store
//prompted by a heartbeat

//some helpful notes about how this thing works
//data coming in is of form of a periodic heartbeat from the user
//it needs to pass through a function with a local store that checks that the heartbeat is valid
//the local store is periodically synced to riak based on a periodic heartbeat from the system

object FocusLogic {

def mapToJson( map : Map[String,String] ) : String = "{" + map.map( x => "\"" + x._1 + "\":\"" + x._2 + "\"").mkString(",") + "}" 

object Event extends Enumeration
  {
  type Event = Value
  val start = Value 
  val cancel = Value 
  val complete = Value 
  implicit def EventFromString(s : String) = withName(s)
  }

import Event._

case class Input(id:String, event:Event, timestamp: Date  )

object State extends scala.Enumeration
  {
  type State = Value
  val slacking = Value
  val working = Value
  val resting = Value
  implicit def StateFromString(s : String) = withName(s)
  }

val ISO8601 = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'")
implicit def ISO8601Date(str : String) = ISO8601.parse(str) 

import State._

case class Record(id:String, state:State, score:Int , period:Date, lastUpdate:Date, expectedUpdate:Date)

def RecordFromJson( j : String ) : Record = {
  val x = JSON.parseFull(j).get.asInstanceOf[Map[String,String]]
  return Record( x("id"), x("state"), x("score").toInt, x("period"), x("lastUpdate"), x("expectedUpdate") ) 
  }

def JsonFromRecord( record : Record ) : String = {
  val m : Map[String,String] = Map( "id" -> record.id, "state" -> record.state.toString , "score" -> record.score.toString, 
                                    "period" -> ISO8601.format(record.period), 
                                    "lastUpdate" -> ISO8601.format(record.lastUpdate) , 
                                    "expectedUpdate" -> ISO8601.format(record.expectedUpdate) )
  return mapToJson( m ) 
  }

def InputFromJson( j : String ) : Input = {
  val x = JSON.parseFull(j).get.asInstanceOf[Map[String,String]]
  return Input( x("id"), x("event"), x("timestamp") ) 
  }

def JsonFromInput( input : Input ) : String = {
  val m : Map[String,String] = Map( "id" -> input.id , "event" -> input.event.toString, "timestamp" -> ISO8601.format( input.timestamp ) ) 
  return mapToJson(m) 
  }
}

object BusinessLogic extends Transformer[String, String, KeyValue[String, String]] {

  //the actual state machine
  def logic( inp : FocusLogic.Input, rec : FocusLogic.Record ) : FocusLogic.Record = inp.event match { 
    case FocusLogic.Event.complete => rec.copy( state = FocusLogic.State.resting, score = rec.score+1, lastUpdate=inp.timestamp )
    case FocusLogic.Event.cancel => rec.copy( state = FocusLogic.State.slacking, lastUpdate=inp.timestamp ) 
    case FocusLogic.Event.start => rec.copy( state = FocusLogic.State.working, lastUpdate=inp.timestamp )
    } 

  //define the initial state
  def logic( inp : FocusLogic.Input ) : FocusLogic.Record = 
    FocusLogic.Record( inp.id, FocusLogic.State.slacking, 0, inp.timestamp, inp.timestamp, inp.timestamp ) 

  var ctx : Option[ProcessorContext] = None  
  def init( context : ProcessorContext) = { ctx = Some(context) }
  def close() = {}
  def transform( key : String, value : String ):KeyValue[String, String] = {
    val store : KeyValueStore[String,ValueAndTimestamp[String]] = 
      ctx.get.getStateStore("table-store").asInstanceOf[KeyValueStore[String, ValueAndTimestamp[String]]] 
    //TODO since I'm not checking the integrity of the data store, if it gets corrupted by a schema change or something
    //this transformer will crash, better make a smarter mechanism for fixing up the data store. 
    val value2:Option[ValueAndTimestamp[String]] = Option(store.get(key))
    val inp = FocusLogic.InputFromJson(value)
    val rec = Try[FocusLogic.Record] { FocusLogic.RecordFromJson(value2.get.value) } match {
      case Success(rec) => rec 
      case Failure(err) => logic(inp) 
      } 
    val out_rec = logic(inp, rec)
    return new KeyValue( inp.id, FocusLogic.JsonFromRecord(out_rec) )
    }
  }

object DataEdit extends Transformer[String, String, KeyValue[String, String]] {
  var ctx : Option[ProcessorContext] = None
  def init( context : ProcessorContext ) = { ctx = Some(context) }
  def close() = {}
  def clear( store : KeyValueStore[String, ValueAndTimestamp[String]] ) = {
    val iter = store.all() 
    while(iter.hasNext())
      {
      val n = iter.next()
      ctx.get.forward( n.key, null )
      }
    iter.close()
    println("EDIT : CLEAR")
    } 
  def transform( key : String, value : String ):KeyValue[String,String] = {
    val store : KeyValueStore[String, ValueAndTimestamp[String]] =
      ctx.get.getStateStore("table-store").asInstanceOf[KeyValueStore[String, ValueAndTimestamp[String]]]
      
    value match {
      case "clear" => clear(store)
      }
    return null
    }
  }

object RiakDumper extends Transformer[String, String, KeyValue[String, String]] {
  var ctx : Option[ProcessorContext] = None
  def init( context : ProcessorContext) = { ctx = Some(context) }
  def close() = {}
  def transform( key : String, value : String ):KeyValue[String, String] = {
    //this dumps the whole of the table into the riak out stream
    val store : KeyValueStore[String,ValueAndTimestamp[String]] = 
      ctx.get.getStateStore("table-store").asInstanceOf[KeyValueStore[String, ValueAndTimestamp[String]]]
    val iter = store.all()
    val siter = iter.asScala
    val out =  "[" + siter.map( {x => x.value.value} ).mkString(",") + "]" 
    iter.close()
    return new KeyValue("riak" , out )
    }
  }

object BLSupplier extends TransformerSupplier[String, String, KeyValue[String, String]] { def get = BusinessLogic }
object DumpSupplier extends TransformerSupplier[String, String, KeyValue[String, String]] { def get = RiakDumper }
object EditSupplier extends TransformerSupplier[String, String, KeyValue[String, String]] { def get = DataEdit }

object EntryPoint { 

//val properties = new Properties()
//properties.load(new FileInputStream("hello.properties"))

//make sure something coming from http-in is a legitimate event, and gives it the right key.
def sanitize( key : String, value : String):Iterable[(String,String)] = Try[FocusLogic.Input] {
  FocusLogic.InputFromJson( value ) 
  } match {
  case Success(r) => List( ( r.id, value ) )
  case Failure(e) => println(e) ; List[(String,String)]() 
  }

//TODO sometimes the update doesn't push through the RiakDumper even when it needs to update 

def createStream = {
  A.subscribe("http-in")
  A.subscribe("heartbeat")
  A.subscribe("sanitize")
  A.subscribe("riak-out")
  A.addTopic("table")
  println("set up outside consumers")
  detach( http("localhost", properties.get("focus_game.http-in").asInstanceOf[String].toInt, "/") ) > kafka("http-in")
  detach( http("localhost", properties.get("focus_game.heartbeat").asInstanceOf[String].toInt, "/") ) > kafka("heartbeat")
  detach( http("localhost", properties.get("focus_game.edits").asInstanceOf[String].toInt, "/") ) > kafka("edits")
  detach( kafka("riak-out") ) > riak
  println("started http server threads")
  val builder = new StreamsBuilder()
  val tablevar : KTable[String,String] = builder.
    table[String, String]("table", Materialized.as[String, String, ByteArrayKeyValueStore]("table-store") )
  val heartstream : KStream[String,String] = tablevar.toStream
  heartstream.to("heartbeat") 
  builder.stream[String,String]("http-in").flatMap(sanitize).transform(BLSupplier, "table-store").to("table")
  builder.stream[String,String]("heartbeat").transform(DumpSupplier, "table-store").to("riak-out")
  builder.stream[String,String]("edits").transform( EditSupplier, "table-store").to("table")
  val topology = builder.build()
  print(topology.describe())
  val stream = new KafkaStreams(topology, properties)
  stream.setUncaughtExceptionHandler( ExceptionHandler ) 
  stream.start() 
  }

}
