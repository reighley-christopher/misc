package chaintools

import javax.json._
import chaintools.ChainTools._
import org.apache.http.client.methods.HttpPut
import org.apache.http.entity.StringEntity
//import org.apache.http.impl.client.HttpClientBuilder
//import java.net.HttpURLConnection
//import java.net.http.HttpClient
//import java.net.http.HttpRequest
//import java.net.http.HttpResponse
import org.apache.http.HttpRequest
import org.apache.http.client.HttpClient
import org.apache.http.impl.client.DefaultHttpClient
import org.apache.http.HttpResponse
import org.apache.http.client.methods.RequestBuilder
import org.apache.http.entity.StringEntity
//import java.net.URI

// simplified Riak interface
// for the purposes of the Focus Game we need to update a  
// > riak(bucket) 
// riak(bucket) > 
// the input and output need to be annotated with header information for the API, at very least we need to know a key (maybe more)
// for this purpose I will need to create an AnnotatedString class 

object Riak { 

def from_riak() : String = {
  scala.io.Source.fromURL( "http://new.reighley-christopher.net/riak/buckets/focus/keys/focus" ).mkString
  }

//def to_riak( payload : String ) = {
//  val put = new HttpPut( "http://new.reighley-christopher.net/riak/buckets/focus/keys/focus" )
//  put.setEntity( new StringEntity( payload ) )
//  val client = HttpClientBuilder.create.build
//  client.execute( put )  
//  }

//def to_riak(payload : String ) = {
//  val client = HttpClient.newHttpClient
//  val connection = new HttpURLConnection( "http://new.reighley-christopher.net/riak/buckets/focus/keys/focus" )
//  connection.setRequestMethod("PUT")
//  val output = connection.getOutputStream()
//  output.write(payload)
//  output.close
//  }

def to_riak(payload : JsonValue) : Unit = {
  }

def to_riak(payload : String, bucket : String, key : String ) : Unit = {
  //val client = HttpClient.newHttpClient
  val client : HttpClient = new DefaultHttpClient()
  print("RIAK: " + payload) 
  val request = RequestBuilder 
    .put("http://new.reighley-christopher.net/riak/buckets/%s/keys/%s".format(bucket, key) )
    .addHeader("Content-Type", "application/json" )
    .setEntity( new StringEntity( payload ) )
    .build
//  val request = HttpRequest.newBuilder()
//    .uri(URI.create("http://new.reighley-christopher.net/riak/buckets/focus/keys/focus") )
//    .PUT(HttpRequest.BodyPublishers.ofString( payload ) )
//    .header("Content-Type", "application/json" )
//    .build
  try { 
    print(client.execute(request)) //TODO if I try a URL with a pipe, this fails silently. real pain in the ass java.io.IOException?
    } catch {  
      case ex : java.io.IOException => None  //riak returns a Content-Length header when sometimes it shouldn't, but that's okay
      case x : Throwable => print(x); throw x
    } 
  }

def to_riak_test(payload : AnnotatedString ) : Unit = {
  var dummykey = payload.get("key")
  println("to_riak_test with real key part 3 : %s".format(dummykey) )
  
  to_riak(payload.body, "focus", dummykey ) 
  }

def to_riak_test_wrapper() : AnnotatedString => Unit = 
  {
  var ret : AnnotatedString => Unit = to_riak_test
  return ret
  }

def to_riak_from_annotations( bucket : String ) : AnnotatedString => Unit = 
  { 
  var ret : AnnotatedString => Unit = { ( payload : AnnotatedString ) 
    => println("to riak from annotations"); to_riak( payload.body, bucket, payload.get("key") ) }
  return ret 
  } 

class RiakInterface(bucket : String, funct : AnnotatedString => Unit ) extends ChainSink[AnnotatedString]( to_riak_test ) 

def riak(bucket : String) = new RiakInterface(bucket, to_riak_test ) 

} 


