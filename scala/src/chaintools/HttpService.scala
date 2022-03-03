package chaintools

import com.sun.net.httpserver.HttpServer
import com.sun.net.httpserver.HttpHandler
import com.sun.net.httpserver.HttpExchange
import java.net.InetSocketAddress
import chaintools._
import scala.collection.mutable.Queue
import scala.collection.mutable.Map
import scala.io.Source
import java.time.Instant
import java.time.Duration
import scala.collection.mutable.HashMap
import scala.util.Random
import scala.collection.JavaConverters._
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.ArrayBlockingQueue
import java.util.concurrent.TimeUnit

import reighley.christopher.Util._

/* TODO or BURN this has prints and delays in it but I don't know if I even need this */

class ThreadedIterable[X]( iter : Iterable[X] ) extends Iterable[X] {
  def iterator = new ThreadedIterator( iter.iterator )
  }
 
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
  def hasNext = {
    println("hasNext") 
    this.synchronized { _hasNext || buffer.isDefined }
    }
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

object ActiveServices {
  val dict:Map[String, Map[Integer, HttpServer]] = Map[String, Map[Integer,HttpServer]]() 
  def register(host:String , port:Integer, path:String, handler:HttpHandler) =  {
    val func = (p:String)=>{ Map[Integer, HttpServer]() }
    val http_server:HttpServer = ( dict.get(host) match {
      case None => {val m = Map[Integer, HttpServer](); dict.put(host, m); m } 
      case Some(x) => x 
      } ).get(port) match {
      case None => {
        val h = HttpServer.create();
        //TODO I would expect the size of the thread pool to grow to 10 but it doesn't seem to work
        //the construction of LoiteringHttp may prevent the pool from growing 
        h.setExecutor(new ThreadPoolExecutor( 2, 10, 2, TimeUnit.SECONDS, new ArrayBlockingQueue(16)   ) ) 
        dict.get(host).get.put(port, h);
        h.bind(new InetSocketAddress( host, port ), 0);
        h.start();
        h 
        } 
      case Some(x:HttpServer) => x 
      } 
    http_server.createContext( path, handler )
    }
  } 

class CallbackHandler( post:Map[String,String] => String, get:Map[String,String] => String, uriPath:String, filePath : String ) extends HttpHandler {

  def handle( exchange:HttpExchange ):Unit = {
    //TODO OMG what spaghetti.
    //the problem here is tha  there is no file associated with the post endpoint
    //and that sendResponseHeaders must be called before opening the body output
    //I think I can clean this up using Option and matching on the verb 

    val headers = exchange.getResponseHeaders()
    //TODO these headers should not be so rigid, in particular I should be able to set Content-Type depending on input
    headers.add("test", "test")
    headers.add("Access-Control-Allow-Origin", "*")
    val input = exchange.getRequestBody()
    val buffer = new Array[Byte](input.available())
    var post_data:Option[String] = None 
    input.read(buffer)
    val verb = exchange.getRequestMethod()
    val ip = exchange.getRemoteAddress()
    var request_headers:Map[String, String] = Map()
    //TODO remember to search for all these print statements and delete them
    print("trying to handle some kind of request\n");
    //TODO this try should return a 500 with stack trace on exception rather than just swallowing everything
    try
      {
      val java_request_headers = exchange.getRequestHeaders()
      val keys = java_request_headers.keySet().asScala
      keys.foreach( key => request_headers(key) = java_request_headers.get(key).asScala.fold("")(_+_) )
      } catch {
      case ex : Exception => print("okay there was an exception %s\n".format(ex)) //TODO I should not be eating exceptions (maybe send 500?)
      }
    //java_request_headers.keySet().asScala.foreach( key => request_headers(key) = java_request_headers.get(key).asScala.fold("")( _+_ ) ) 
    if(verb == "POST" ) {
      val str = buffer.map( _.toChar )
      val post_map = request_headers + ( "ip"->ip.toString ) + ( "body"->str.mkString )
      post_data = Some(post(post_map))
      }
    if(verb == "POST") {
      if(post_data == Some("")) exchange.sendResponseHeaders( 200, -1 ) else
      exchange.sendResponseHeaders( 200, post_data.getOrElse("").length )
      }
    val path = exchange.getRequestURI().getPath()
    val subpath = path.substring( path.indexOf(uriPath) + uriPath.length ) /*I am depending on the fact that HTTP server will 404 if path does not start with uriPath TODO revisit this assumption */
    print("we expect here to be handling a request with verb %s\n".format(verb) )
    val data : Array[Byte] = verb match {
      case "POST" => post_data.getOrElse("").getBytes() 
      case "GET" =>  { 
        print("here we are even calling it and everything %s".format(get))
        val xx = get(request_headers + ( "ip"->ip.toString ) + ( "path"->subpath ) ).getBytes()
        print("got %d bytes back\n", xx.length) 
        xx
        }
      }
    if( verb == "GET" ) exchange.sendResponseHeaders( 200, data.length )
    val output = exchange.getResponseBody()
      //println(filename)
      //val data = get().getBytes()
    output.write( data ) //todo what happens if I try to write zero bytes after I already sent a -1 for length in response header
    output.flush()
    output.close()
    input.close()
    }
  }

/*
datastore will store a collection of key value pairs, it will keep track of the age of the pairs, and dispose of any that have not been read recently
*/
trait Datastore[A] {
  def get(key:String):A
  def set(key:String, value:A):Unit
  def purge(timestamp:Instant):Unit 
  } 

class MapDatastore[A] extends Datastore[A] {
  /*TODO should use apply/unapply it seems better */
  val map = new HashMap[String, Tuple2[A, Instant]]
  def get(key:String) = map(key)._1
  def set(key:String, value:A) = { map(key) = Tuple2( value, Instant.now() ) }
  def purge(timestamp:Instant) = {} /*TODO implement purging of datastore when it reaches a certain size and time since last purge is long*/ 
  }


//TODO this is alot like MapDataStore need to handle the timeout failure modes
class Waiter[A] {
  val index = Map[String, A]()
  val timeouts = Map[String, Instant]()
  //start should block until finish is called from another thread
  def start(token:String):A = 
    {
    timeouts(token) = Instant.now()
    //TODO don't hardcode the timeout, create some kind of strategy
    while( ! ( index contains token ) && Instant.now().isBefore( timeouts(token).plus(Duration.ofSeconds(60) ) ) ) { Thread.`yield` }
    print("we are pleased to accept your request for token %s\n".format(token) )
    if(index contains token) print("we have found a value for it!\n");
    else  print("we have no value for it\n")
    print( Duration.between( Instant.now(), timeouts(token) ) )
    val ret = index.remove(token)
    timeouts.remove(token)
    return ret.get //TODO if I timed out 404, if I excepted 500
    } 
  
  def finish( token:String, ret:A ) = 
    {
    index(token) = ret 
    } 
  }

class LoiteringHTTPInterface( host:String, port:Int, path:String, wait:Waiter[AnnotatedString] ) extends ChainSink[AnnotatedString]( { x => wait.finish( x.get("key"), x ) } ) with ChainHead[AnnotatedString] {

  //def do_the_thing( s : AnnotatedString ) : Unit = {print("absorb\n")} /*TODO wut??*/ 
  //override def absorb( iter:Iterable[AnnotatedString]):Unit = iter.foreach(do_the_thing )

  val rand = new Random
  val requestQueue = new Queue[AnnotatedString]
  //the get_callback needs to block until the data it is waiting for comes back
  def callback(data:Map[String,String]) = {
    print("attempting to handle request :\n");
    print(data); 
    //make a random key 
    val hashkey="%s-%s-%d".format( data("ip"), Instant.now().toString, rand.nextInt(1000)).hashCode.toHexString
    requestQueue.enqueue(new AnnotatedString(data.getOrElse("body", ""), (data + ("key" -> hashkey ) - "body" ).toSeq:_* ) )
    val bo = wait.start(hashkey).body
    print("handled request %s\n".format(mapToJson(data)) )
    bo
    }

  //this is what makes it a chainhead, producing requests one at a time foreve
  def iterator = new Iterator[AnnotatedString] { 
    def hasNext = true
    def next:AnnotatedString = {
      while( requestQueue.isEmpty ) { Thread.`yield` }
      return requestQueue.synchronized { requestQueue.dequeue() }
      }  
    }

  val handler = new CallbackHandler( callback, callback, path,  "/home/reighley/Code/misc/focus_game" )
  ActiveServices.register(host, port, path, handler)
  } 

class TokenizedHTTPInterface( host:String, port:Int, path:String, datastore:Datastore[String] ) extends ChainSink[AnnotatedString]( { x => datastore.set(x.get("key"),x.body) } ) with ChainHead[AnnotatedString] {

/*TODO copy pasted from StringHTTPInterface, merge these*/

  val requestQueue = new Queue[AnnotatedString]
  //val http_server = HttpServer.create()

/*
  def do_the_thing( s : String ) : Unit = {} 
  override def absorb( iter:Iterable[AnnotatedString]):Unit = iter.foreach(do_the_thing )
*/

  val rand = new Random

  /*TODO make it possible to supply my own key, so I can use this as a pub/sub*/ 
  def post_callback(data:Map[String,String]) = requestQueue.synchronized {
    /*I need IP address in the annotation, and I need to assign a token, and I need to set that token to key so I can find it later*/
    val hashkey="%s-%s-%d".format( data("ip"), Instant.now().toString, rand.nextInt(1000)).hashCode.toHexString
    requestQueue.enqueue(new AnnotatedString(data("body"), "ip"->data("ip"), "key"->hashkey) ) //TODO add the rest of the headers
    hashkey 
    }  

  def get_callback(query:Map[String,String]) = {
    datastore.get(query("path"))
    } 

  val handler = new CallbackHandler( post_callback, get_callback, path,  "/home/reighley/Code/misc/focus_game" )
  ActiveServices.register(host, port, path, handler)

  def iterator = new Iterator[AnnotatedString] { 
    def hasNext = true
    def next:AnnotatedString = {
      while( requestQueue.isEmpty ) { Thread.`yield` }
      return requestQueue.synchronized { requestQueue.dequeue() }
      }  
    } 
  }

class StringHTTPInterface( host:String, port:Int, path:String ) extends ChainSink[String]( { x => Unit  } ) with  ChainHead[String] {

  val requestQueue = new Queue[String]
  //val http_server = HttpServer.create()

  def do_the_thing( s : String ) : Unit = {} /*TODO wut??*/ 
  override def absorb( iter:Iterable[String]):Unit = iter.foreach(do_the_thing )
 
  def post_callback(data:Map[String,String]) = requestQueue.synchronized {
    requestQueue.enqueue(data("body"))
    ""
    }  

  def get_callback(dummy:Map[String,String]) = "hello world"

  val handler = new CallbackHandler( post_callback, get_callback, path, "/home/reighley/Code/misc/focus_game" )
  //http_server.createContext(path, new CallbackHandler( post_callback, get_callback, path, "/home/reighley/Code/misc/focus_game" ) )
  //http_server.bind( new InetSocketAddress( host, port ), 0 )
  //http_server.start()
  ActiveServices.register(host, port, path, handler)

  def iterator = new Iterator[String] { 
    def hasNext = true
    def next:String = {
      while( requestQueue.isEmpty ) { Thread.`yield` }
      return requestQueue.synchronized { requestQueue.dequeue() }
      }  
    } 
  }

class AnnotatedStringHTTPInterface( host:String, port:Int, path:String ) extends ChainSink[AnnotatedString]( { x => Unit } ) with ChainHead[AnnotatedString] {
  val requestQueue = new Queue[AnnotatedString]
  //val http_server = HttpServer.create()

  def do_the_thing( s : AnnotatedString ) : Unit = {} /*TODO wut??*/ 
  override def absorb( iter:Iterable[AnnotatedString]):Unit = iter.foreach(do_the_thing )
 
  def post_callback(data:Map[String,String]) = requestQueue.synchronized {
    val out = new AnnotatedString( data("body"), (data-"body").toSeq:_* ) 
    requestQueue.enqueue(out)
    ""
    }  

  def get_callback(dummy:Map[String,String]) = "hello world"

  val handler = new CallbackHandler( post_callback, get_callback, path, "/home/reighley/Code/misc/focus_game" )
  //http_server.createContext(path, new CallbackHandler( post_callback, get_callback, path, "/home/reighley/Code/misc/focus_game" ) )
  //http_server.bind( new InetSocketAddress( host, port ), 0 )
  //http_server.start()
  ActiveServices.register(host, port, path, handler)

  def iterator = new Iterator[AnnotatedString] { 
    def hasNext = true
    def next:AnnotatedString = {
      while( requestQueue.isEmpty ) { Thread.`yield` }
      return requestQueue.synchronized { requestQueue.dequeue() }
      }  
    } 
  }


/*TaskIntersection gives boths Head and Sink behavior to an object running in a separate thread*/
/*TODO this copies a lot of code from TaskHead maybe I can abstract this out as a mixin?*/
class TaskIntersection[X,Y]( base : ChainSink[X] with ChainHead[Y] ) extends ChainSink[X]({x => Unit}) with ChainHead[Y] {
  var _stop = true
  def stop:Unit = { this.synchronized{ _stop=false } }

  override def toString = "TaskIntersection"

  /*if absorb is called I am being used as a Sink, and I need to accept all the members of the iterable (one at a time and then yield)*/
  /*and provide them to base Sink ?somehow? do I trust base sink to start a thread or am I starting two threads? */
  override def absorb(iterable:Iterable[X]) = {
    println("absorb")
    //base.absorb(new ThreadedIterable(iterable) )
    val t = new Thread { override def run()= {
      iterable.foreach( { x =>
        base.func(x) //this breaks threading? 
        Thread.`yield`() 
        } ) 
      } }
    t.start()
    }
 
  /*if > is called I am being used as a source and the chain is complete and I should start the thread to absorb it all*/ 
  override def >( k : ChainSink[Y] ):Unit = {
    val self = this
    val t = new Thread { override def run()={ k.absorb(self) } } 
    t.start() 
    }

  def iterator = new Iterator[Y] {
    val iterator = base.iterator
    /*as soon as > is called the iterator will call hasNext at once, but it may not know if it is time to stop, that will happen when base.iterator 
      runs out, or at least that is what should happen but it never seems to call next*/ 
    def hasNext = {
      true // this.synchronized{ iterator.hasNext }
      } /*if this causes problems with focus_game it is because hasNext needs to be fixed to true for http to work and this is a bug*/
    def next():Y = {
      while( ! iterator.hasNext && ! _stop ) { Thread.`yield` }
      iterator.next
      } 
    } 
  }

/*TaskHead runs the downstream chains in a separate thread
the cases which need to be dealt with are:
the base iterator returns hasNext = true 
the base iterator runs out, and returns hasNext = false
the base iterator returns hasNext = false but actually we are just waiting for more data and should call again later
TODO this loitering behavior is not supported yet, but it seems like it might make for a more sensible Kafka iterface ??  
*/

class TaskHead[X]( base : ChainHead[X], loiter:Boolean = false ) extends ChainHead[X] {
  var _stop = true
  def stop:Unit = { this.synchronized{ _stop=false } }
  override def >( k : ChainSink[X] ):Unit = {
    val self = this
    val t = new Thread { override def run()={ k.absorb(self) } } 
    t.start() 
    }
  def iterator = new Iterator[X] {
    val iterator = base.iterator
    /*as soon as > is called the iterator will call hasNext at once, but it may not know if it is time to stop, that will happen when base.iterator 
      runs out, or at least that is what should happen but it never seems to call next*/ 
    def hasNext = {
      true // this.synchronized{ iterator.hasNext }
      } /*if this causes problems with focus_game it is because hasNext needs to be fixed to true for http to work and this is a bug*/
    def next():X = {
      while( ! iterator.hasNext && ! _stop ) { Thread.`yield` }
      iterator.next
      } 
    } 
  }

object HTTPService {
  def http(host:String, port:Int, path:String) = new AnnotatedStringHTTPInterface( host, port, path )
  def http(host:String, port:Int, path:String, store:Datastore[String]) = new TokenizedHTTPInterface( host, port, path, store )
  def loiteringHttp(host:String, port:Int, path:String) = new LoiteringHTTPInterface( host, port, path, new Waiter())
  def default_http(port:Int) = new TokenizedHTTPInterface( "localhost", port, "/", new MapDatastore[String] ) 
  def detach() = new TaskLink[String]
  def detach[X]( head:ChainHead[X] ) : TaskHead[X] = new TaskHead[X]( head )
  def detach[X,Y]( base:ChainSink[X] with ChainHead[Y] ) = new TaskIntersection[X,Y](base)
  }
