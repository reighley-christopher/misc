package reighley.christopher

import java.lang.reflect.Method
import java.io.OutputStream
import java.io.InputStream
import java.lang.Thread
import scala.sys.process._
import scala.collection.Map
import scala.collection.immutable.{Map => ImMap}
import scala.collection.mutable.{Map => MuMap}
import java.net.HttpURLConnection
import java.net.URL
import scala.io.Source
import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.databind.JsonNode
import scala.jdk.CollectionConverters._

object Util {
  def mapToJson( map : Map[String,String] ) : String = "{" + map.map( x => "\"" + x._1 + "\":\"" + x._2 + "\"").mkString(",") + "}" 
  def dir( example:Object ):Array[String] = {
    example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) 
    }

  def jsonToMap( json:String ):Map[String,String] = {
    val mapper = new ObjectMapper()
    mapper.readTree(json).fields.asScala.map( e => e.getKey() -> e.getValue().asText ).toMap
    }

  def jsonToShallowMap( json:String ):ImMap[String, JsonNode] = {
    val mapper = new ObjectMapper()
    mapper.readTree(json).fields.asScala.map( e => e.getKey() -> e.getValue() ).toMap
    }

  def shallowMapToJson( map:Map[String, JsonNode] ):JsonNode = 
    {
    val mapper = new ObjectMapper()
    val obj = mapper.createObjectNode()
    for( (key, value) <- map ) obj.set( key, value )
    return obj
    }

  //TODO connect the error stream to something throws exceptions
  def simpleProcess(path:String):(OutputStream, InputStream) = 
    {
    var inp:Option[InputStream] = None
    var outp:Option[OutputStream] = None
    Process(path).run( new ProcessIO( 
      {(o:OutputStream) => outp = Some(o) },
      {(i:InputStream) => inp = Some(i) },
      {(e:InputStream) => }
      ) )
    while( inp.isEmpty || outp.isEmpty ){}; 
    (outp.get, inp.get) 
    }

  class IteratorIterator[X]( factory : () => Iterator[X] ) extends Iterator[X]
    {
    var iter = factory()
    def hasNext = { 
      if( !iter.hasNext ) iter = factory()
      iter.hasNext 
      } 
    def next() = iter.next()
    } 

  class IterableIterable[X]( factory : () => Iterator[X] ) extends Iterable[X]
    {
    def iterator = new IteratorIterator( factory )
    }

  def template( tmpl:String ) = (map:Map[String, String]) => 
    {
    var ret = tmpl;
    map.keys.foreach( (x:String) => {
      ret = ret.replace(s"<%= $x %>", map(x))
      }) 
    ret
    } 

  def simple_http(_url:String, body:String, header:Map[String,String]):String = {
    val url:URL = new URL(_url)
    val connection:HttpURLConnection = url.openConnection().asInstanceOf[HttpURLConnection]
    header.foreach( (entry)=>connection.setRequestProperty(entry._1, entry._2)) 
    if(body.length > 0)
      {
      connection.setDoOutput(true) 
      val writestream = connection.getOutputStream()
      writestream.write(body.getBytes())
      writestream.flush()
      writestream.close()
      }
    val response = connection.getInputStream()
    val ret = Source.fromInputStream(response).mkString 
    response.close()
    ret
    }

  }
