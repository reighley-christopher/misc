import chaintools._
import chaintools.ChainTools._
import chaintools.HTTPService._
import org.apache.kafka.streams.processor._ 
import org.apache.kafka.streams.scala._ 

//basic arrangement of the Streams
//for the moment, permit no user updates or edit the list
//accept an object from the HTTP server
//feed object to kafka stream
//kafka stream includes a table which is updated 
//the updated data is periodically synced to the riak store
//prompted by a heartbeat

//some helpful notes about how this thing works
//data coming in is of form of a periodic heartbeat from the user
//it needs to pass through a function with a local store that checks that the heartbeat is valid
//the local store is periodically synced to riak

object BusinessLogic extends Processor[String, String] {
  def init( context : ProcessorContext) = {}
  def close() = {}
  def process( key : String, value : String ) = {}
  }

def createStream = {
  A.subscribe("http-in")
  A.subscribe("heartbeat")
  A.subscribe("riak-out")
  detach( http("localhost", 8888, "/") ) > kafka("http-in")
  detach( kafka("riak-out") ) > sinkprint 
  val builder = new StreamsBuilder()
  builder.stream[String,String]("http-in").map({( x:String, y:String) => ( x + "key" , y + "value"  )}).to("table")
  val topology = builder.build() 
  val stream = new KafkaStreams(topology, properties)
  stream.setUncaughtExceptionHandler( ExceptionHandler ) 
  stream.start() 
  }

