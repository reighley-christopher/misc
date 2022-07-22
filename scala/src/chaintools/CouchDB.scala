package chaintools

import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.databind.JsonNode
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
//import org.apache.http.impl.client.CloseableHttpClient
import org.apache.http.impl.client.HttpClientBuilder
import org.apache.http.HttpResponse
import org.apache.http.client.methods.RequestBuilder
import org.apache.http.entity.StringEntity
import reighley.christopher.Util._
import scala.collection.mutable.{Map => MuMap}
import org.apache.commons.io.IOUtils
import java.nio.charset.Charset
//import java.net.URI

// simplified CouchDB interface
// for the purposes of the Focus Game we need to update a  
// > couchdb(database) 
// couchdb(database) > 
// the input and output need to be annotated with header information for the API, at very least we need to know a key (maybe more)
// somebody has to keep track of the _rev 
// couchdb updates keys with PUT
//TODO develop more interesting merge rules
//TODO accept queries
object CouchDB { 

def couchdb_get() : String = {
  scala.io.Source.fromURL( s"http://new.reighley-christopher.net/couchdb/jeddb/test" ).mkString
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

//def to_riak(payload : JsonValue) : Unit = {
//  }

def couchdb_put(database : String, key : String, jsonnode : JsonNode):(Int, String) = {
  val client : HttpClient = HttpClientBuilder.create().build()
  val request = RequestBuilder 
    .put("http://new.reighley-christopher.net/couchdb/%s/%s".format(database, key) )
    .addHeader("Content-Type", "application/json" )
    .setEntity( new StringEntity( jsonnode.toString() ) )
    .build
  try {
    val response = client.execute(request) //TODO if I try a URL with a pipe, this fails silently. real pain in the ass java.io.IOException?
    val entity = response.getEntity().getContent() 
    return (response.getStatusLine().getStatusCode(), entity.toString)
    } catch {
      case _:Exception => return ( 0, "")
    }
  }

def to_couchdb_pessimistic(payload : Map[String, JsonNode], database : String, key : String, revisions : MuMap[String, String] ) : Unit = {
  val client : HttpClient = HttpClientBuilder.create().build()
  val mapper = new ObjectMapper()
  val request = RequestBuilder
    .get("http://new.reighley-christopher.net/couchdb/%s/%s".format(database, key) )
    .build
  val response = client.execute(request)
  val dict = jsonToMap( IOUtils.toString(response.getEntity().getContent(), Charset.forName("UTF-8")) )
  revisions(dict("_id")) = dict("_rev")
  val stree = shallowMapToJson( payload + ("_rev" -> mapper.valueToTree(revisions(key) ) ) )
  to_couchdb_optimisitic(payload, database, key, revisions ) //the recursion will hopefully not go too deep, but there is always a possibility of multiple contention 
  }

//this assumes that no collision occurs, updates if it gets a 409 Conflict error
def to_couchdb_optimistic(payload : payload : Map[String, JsonNode], database : String, key : String, revisions : MuMap[String, String] ) : Unit = {
  //val client = HttpClient.newHttpClient
  val client : HttpClient = HttpClientBuilder.create().build()
  val mapper = new ObjectMapper()
   
  //if there is no entry in the revisions database this might be a new record just go for it, a 200 request will contain a rev field remember to save
  if( revisions.contains(key)) {
    val strtree = shallowMapToJson(tree + ("_rev" -> mapper.valueToTree(revisions(key) ) ) )
    print(strtree.toString())
    couchdb_put( database, key, strtree ) match {
      case (409, _ ) => {
        //oh no conflict
        to_couchdb_pessimistic( payload, database, key, revisions )
        //we will do a get to update the revision number
        //var new_rev = mapper.read_tree( couchdb_get(database, key) )
        //then record new revision number
        //revisions[key] = tree.get("_rev") 
        //then write out
        //tree.set("_rev", revisions[key]) 
        }
      case (0, _ ) => { 
        ()
        //this was an error
        }
      case (_, _) => {
        ()
        //anything else was maybe okay (should probably handle other http errors but no)
        //TODO don't swallow errors
        } 
      }
    } else to_couchdb_pessimistic( payload, database, key, revisions )

  }

def astring_to_couchdb(database:String, payload : AnnotatedString ) : Unit = {
  var dummykey = payload.get("key")
  println("to_optimistic_test : %s".format(dummykey) )
   
  to_couchdb_optimistic(payload.body, database, dummykey, MuMap() ) 

  }

//def to_riak_test_wrapper() : AnnotatedString => Unit = 
//  {
//  var ret : AnnotatedString => Unit = to_riak_test
//  return ret
//  }

//def to_riak_from_annotations( bucket : String ) : AnnotatedString => Unit = 
//  { 
//  var ret : AnnotatedString => Unit = { ( payload : AnnotatedString ) 
//    => println("to riak from annotations"); to_riak( payload.body, bucket, payload.get("key") ) }
//  return ret 
//  } 

class CouchDBInterface(database : String, funct : AnnotatedString => Unit ) extends ChainSink[AnnotatedString]( x => () ) {
  val m = MuMap[String, String]()
  override def absorb( iter:Iterable[AnnotatedString]):Unit = iter.foreach(func)
  }

def couchdb(bucket : String) = new CouchDBInterface(bucket, astring_to_couchdb ) 

} 


