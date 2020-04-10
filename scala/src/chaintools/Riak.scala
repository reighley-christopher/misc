package chaintools

import javax.json._
import chaintools.ChainTools._
import org.apache.http.client.methods.HttpPut
import org.apache.http.entity.StringEntity
//import org.apache.http.impl.client.HttpClientBuilder
import java.net.HttpURLConnection
import java.net.http.HttpClient
import java.net.http.HttpRequest
import java.net.http.HttpResponse
import java.net.URI

// simplified Riak interface
// for the purposes of the Focus Game we need to update a  
// > riak(bucket) 
// riak(bucket) >  

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

def to_riak(payload : String ) : Unit = {
  val client = HttpClient.newHttpClient
  val request = HttpRequest.newBuilder()
    .uri(URI.create("http://new.reighley-christopher.net/riak/buckets/focus/keys/focus") )
    .PUT(HttpRequest.BodyPublishers.ofString( payload ) )
    .header("Content-Type", "application/json" )
    .build
  try { 
    client.send(request, HttpResponse.BodyHandlers.ofString() )
    } catch {  
      case ex : java.io.IOException => None  //riak returns a Content-Length header when sometimes it shouldn't, but that's okay
      case x : Throwable => throw x
    } 
  }

class RiakInterface() extends ChainSink[String]( { to_riak } ) 

def riak() = new RiakInterface 

} 


