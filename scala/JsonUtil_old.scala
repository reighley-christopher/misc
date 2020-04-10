import javax.json._
import scala.collection.JavaConverters._
import scala.reflect.ClassTag
import scala.reflect.runtime.universe._
import scala.reflect.api._
import scala.collection.mutable.ArrayStack

def riak() : String = {
  scala.io.Source.fromURL( "http://new.reighley-christopher.net/riak/buckets/projer/keys/projer" ).mkString
  }

def fromJson( json:String ) = Json.createReader( new java.io.StringReader(json) ).read

def structureJson( strct : JsonStructure ): Option[JsonObject] = strct match {
  case obj: JsonObject => Some(obj) 
  case arr: JsonArray => None 
  }

def arrayJson( arry : JsonStructure ): Option[JsonArray] = arry match {
  case obj: JsonObject => None 
  case arr: JsonArray => Some(arr) 
  }

def filterCast[X](a : Any)(implicit tt: ClassTag[X] ) : Option[X] = a match {
  case aa : X => Some(aa)    
  case _ => None 
  }

/*
def reduceJsonArray[X]( a : JsonArray, nestedProcessor : JsonValue => X ) : Array[X] = {
  val iter = a.iterator()
  val ret = new ArrayStack[X]()
  while(iter.hasNext()) {
    val e = iter.next()
    ret.push(nestedProcessor( e ) )
    }
  val ret2 = new Array[X](ret.length) 
  ret.copyToArray( ret2  )
  return ret2
  } 
*/

object TypeNote extends Enumeration {
  type TypeNote = Value
  val  _Array, _Int = Value 
  }
import TypeNote._

case class AnnotatedThing[X]( value : Int , tag : ClassTag[X], note : String, tpe: TypeNote, next : Option[AnnotatedThing[_]] )

object Unpacker {

  def unpack[Y]( a : Array[Y] )( implicit atag : ClassTag[Array[Y]], ytag : ClassTag[Y] ) : AnnotatedThing[Array[Y]] = a match {
    case ay : Array[Y] => AnnotatedThing( 0, atag,  "array",_Array, None ) 
    case _ => AnnotatedThing( 0, atag, "huh", _Int,  None )   
    }

  def unpack[Y]( a : Any ): Option[Any] = None 

  def emit( tag : ClassTag[scala.Int] ) : scala.Int = 0
  def emit( tag : ClassTag[java.lang.String] ) : java.lang.String = "."

/*  def emit[Y]( tag : ClassTag[Array[Y]] )( implicit itag : ClassTag[Y] ) : Array[Y] = Array( emit( itag ) ) */ 
  /*def emit[Y]( value : Y, tag :  ClassTag[Array[Y]] ) : Array[Y] = Array[Y](value)*/
  def emit[Y]( value : Y , tag : ClassTag[Y] ) : Y = value 

  def unapply[X](a:X) : Option[Array[X]] = None 

 }

/*
object Describer {
  val I = typeOf[Int] //WHY!!!??
  val S = typeOf[String] //WHY!!!??

  def build_array_from_array[X]( arr : JsonArray, etype : Type )(implicit typetag : TypeTag[X] ) : Array[Any] = { 
    val iter = arr.iterator()
    val ret = new ArrayStack[Any]()
    while(iter.hasNext()) {
      val e = iter.next()
      ret.push( inner( e,  etype, typetag )  )
      }
    val ret2 = new Array[Any](ret.length) 
    ret.copyToArray( ret2  )
    return ret2
    } 

  def build_array[X]( crush : JsonValue, etype : Type, typetag : TypeTag[Array[X]] ) : Array[Any] = crush match {
    case arr : JsonArray => build_array_from_array( arr, etype )
    case _  => Array[Any]() 
    }

  def outer[X](crush : JsonValue )(implicit tag : TypeTag[X]) = inner[X]( crush, tag.tpe, tag ) 
  def inner[X]( crush : JsonValue, tpe : Type , tag : TypeTag[X] ) : Any = tpe match {
    case TypeRef( _, _, List( x ) ) => build_array( crush, x, tag )
    case TypeRef( _, _, List( _, x ) ) => "map" + inner( crush, x, tag )
    case I => "int"
    case S => "string"
    case _ => "nope" 
    } 
  }
*/

object ArrayExtractor {
  def unapply[X]( tag : ClassTag[Array[X]] )( implicit itag : ClassTag[X] ) = itag 
  } 

object Selecter {
  def first[X]()(implicit typetag : TypeTag[X], classtag : ClassTag[X] ) : Option[X] = { 
    val m = Marker[X]
    typetag match {
      case t : TypeTag[Array[y]] => Some(t)
      case _ => None
      }
    }  
  } 

object Chainer { 

/*we can surface a classtag like this*/
def get_classtag[X]()( implicit tag : ClassTag[X] ) : ClassTag[X] = tag

/*we can extract a classtag of something we know is an array like this*/
def array_inner_class[X]( tag : ClassTag[Array[X]] )( implicit itag : ClassTag[X] ) = itag 

def chain_of_annotations[X]( ct : ClassTag[Array[X]] ) : AnnotatedThing[Array[X]] = 
  AnnotatedThing( 0, ct, "array", _Array, chain_of_annotations( array_inner_class( ct ) ) ) 
def chain_of_annotations( ct : ClassTag[Int] ) : AnnotatedThing[Int] = AnnotatedThing( 0, ct, "int", _Int, None )

}

/* how complicated can we make this totally generic factory ( parameters later ) ? */  
def reduce[X]():Some(X) = None  


/*big important question is can I force a ClassTag on a function if I know what it is, working around the implicit modifier*/

def boil[X]( a : SpecialSauce )( implicit ytag : ClassTag[X] ): Option[X] = a match {
  case Unpacker(x)  => filterCast[X](x) //x is another SpecialSauce 
  } 
 
/*def tag_from_type[X]( t : Type, parent : TypeTag[Array[X]] ) = TypeTag(parent.mirror, new TypeCreator {def apply( m : Mirror[_] ) = t} ) */

def reduceJson[X]( a : JsonArray )( implicit tag : TypeTag[X], classTag : ClassTag[X] ) : Option[X] = tag.tpe  match {
  /*case TypeRef( _, _, List(x) ) => println(x) ; reduceJsonArray( a, boil( TypeTag(tag.mirror, new TypeCreator { def apply( m : Mirror[_] ) = x }  )) ) */ 
  case _ => None 
  } 
   

import java.lang.reflect.Method
def dir( example:Object ):Array[String] = { example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) } 

def get_json_array() = arrayJson( fromJson( riak() ) ).get

def test_reducer:Array[String] = {
  val v1 = arrayJson(fromJson(riak()))
  val v0 = arrayJson(get_json_array())
  val v2 = v0.getOrElse(Json.createArrayBuilder().build())
  reduceJson[Array[String]](v2) 
  }


