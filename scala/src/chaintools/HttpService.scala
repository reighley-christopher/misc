package chaintools

import com.sun.net.httpserver.HttpServer
import com.sun.net.httpserver.HttpHandler
import com.sun.net.httpserver.HttpExchange
import java.net.InetSocketAddress
import chaintools._
import scala.collection.mutable.Queue
import scala.io.Source

/* TODO or BURN this has prints and delays in it but I don't know if I even need this */
 
class ThreadedIterator[X]( iterator : Iterator[X] ) extends Iterator[X] {
  var _hasNext = iterator.hasNext 
  var buffer:Option[X] = if( iterator.hasNext ) Some(iterator.next()) else None 
  val r:Runnable = new Runnable {
  def run(): Unit = {
     while(true) {
        if( iterator.hasNext && buffer.isEmpty ) {
          this.synchronized {
            buffer = Option( iterator.next )
            _hasNext = iterator.hasNext
            println("populated buffer")
            } 
          }
        if( !_hasNext && buffer.isEmpty ) {
          Thread.currentThread().join()
          }
        Thread.sleep(500)
        print(".")
        }
      }
    }
  val t = new Thread(r) 
  t.start
  def hasNext = this.synchronized { _hasNext || buffer.isDefined }
  def next:X = {
    println("next")
    /*if there is anything in the buffer return it otherwise block
    TODO raise exception if not hasNext instead of entering an infinte loop*/
    var v:Option[X] = None 
    while( v.isEmpty ) {
      this.synchronized {
        v = buffer
        buffer = None 
        }
      Thread.`yield`
      }
    if (v.isDefined) println("contents in buffer")
    return v.get
    }
  }

class TaskLink[X] extends ChainLink[X,X]({(x)=>x}) {
  override def iterator(iter:ChainHead[X]):Iterator[X] = new ThreadedIterator( iter.iterator ) 
  }

class CallbackHandler( post:String => Unit, get:() => String, filePath : String ) extends HttpHandler {

  def handle( exchange:HttpExchange ):Unit = {
    //TODO OMG what spaghetti.
    //the problem here is tha  there is no file associated with the post endpoint
    //and that sendResponseHeaders must be called before opening the body output

    val headers = exchange.getResponseHeaders()
    headers.add("test", "test")
    val input = exchange.getRequestBody()
    val buffer = new Array[Byte](input.available())
    input.read(buffer)
    val verb = exchange.getRequestMethod()
    println(verb)
    if(verb == "POST" ) {
      val str = buffer.map( _.toChar )
      post( str.mkString )
      }
    if(verb == "POST") {
      exchange.sendResponseHeaders( 200, -1 )
      }
    val filename = filePath + exchange.getRequestURI().getPath()
    val data : Array[Byte] = verb match {
      case "POST" => Array()
      case "GET" =>  Source.fromFile(filename).mkString.getBytes() 
      }
    if(verb == "GET") {
      exchange.sendResponseHeaders( 200, data.length )
      }
    val output = exchange.getResponseBody()
    if(verb == "GET" ) {
      println(filename)
    //val data = get().getBytes()
      output.write( data )
      }
    output.flush()
    output.close()
    input.close()
    }
  }

/*http will start an http server which serves a single page, at a particular url
  for GET requests and feeds the body of POST requests down the pipe
*/

class HTTPInterface( host:String, port:Int, path:String ) extends ChainSink[String]( { x => Unit  } ) with  ChainHead[String] {

  val requestQueue = new Queue[String]
  val http_server = HttpServer.create()

  def do_the_thing( s : String ) : Unit = {} 
  override def absorb( iter:Iterable[String]):Unit = iter.foreach(do_the_thing )
 
  def post_callback(data:String) = requestQueue.synchronized {
    requestQueue.enqueue(data)
    }  

  def get_callback() = "hello world"

  http_server.createContext(path, new CallbackHandler( post_callback, get_callback, "/home/reighley/Code/misc/focus_game" ) )
  http_server.bind( new InetSocketAddress( host, port ), 0 )
  http_server.start()

  def iterator = new Iterator[String] { 
    def hasNext = true
    def next:String = {
      while( requestQueue.isEmpty ) { Thread.`yield` }
      return requestQueue.synchronized { requestQueue.dequeue() }
      }  
    } 
  }

class TaskHead[X]( base : ChainHead[X] ) extends ChainHead[X] {
  override def >( k : ChainSink[X] ):Unit = {
    val self = this
    val t = new Thread { override def run()={ k.absorb(self) } } 
    t.start() 
    }
  def iterator = new Iterator[X] {
    val iterator = base.iterator 
    def hasNext = true
    def next():X = {
      while( ! iterator.hasNext ) { Thread.`yield` }
      iterator.next
      } 
    } 
  }

object HTTPService {
  def http(host:String, port:Int, path:String) = new HTTPInterface( host, port, path ) 
  def detach() = new TaskLink[String]
  def detach[X]( head:ChainHead[X] ) : TaskHead[X] = new TaskHead[X]( head )
  }
