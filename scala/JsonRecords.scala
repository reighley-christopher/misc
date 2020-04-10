import java.lang.reflect.Method
import scala.annotation.StaticAnnotation
import scala.reflect.ClassTag
import scala.reflect.runtime.universe._
import scala.reflect.api._
import scala.util.parsing.json.JSON 

def fromJson( json:String ) = Json.createReader( new java.io.StringReader(json) ).read

def dir( example:Object ):Array[String] = {
  example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) 
  }

case class Record( a : String, b : Int  )

//X will be a CaseClass, values passed to the apply method with the variables from the  
def parseJson[X]( schema:Array[String], json:JsonStructure )( implicit tag : ClassTag[X] ) : Class[_] = {
  return tag.runtimeClass
  }

def test() = {
  parseJson[Record]( Array("a", "b"), fromJson("{\"a\":\"1\",\"b\":1}" ) )  
  } 
