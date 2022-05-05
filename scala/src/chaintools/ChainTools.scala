package chaintools

import reighley.christopher.Util._
import scala.util.parsing.json.JSON

class ChainSink[X](f : X => Unit) {
  def func = f
  def absorb(iter:Iterable[X]):Unit = iter.foreach( f )
  }

class ChainLink[X,Y](f : X => Y) {
  def iterator(iter:ChainHead[X]):Iterator[Y] = new Iterator[Y] { 
    val ator = iter.iterator
    def hasNext = ator.hasNext
    def next() = f(ator.next)
    } 
  // def >( k : ChainSink[Y] ) : ChainSink[X] 
  // def >[Y]( k : ChainLink[X,Y] ) : ChainLink[X, Y]
}

trait ChainHead[X] extends Iterable[X] {

  //TODO this indirection is a big part of the problem with the confusing semantics of detach()
  def >( k : ChainSink[X] ) = {
    k.absorb(this)
    }

  def build_chain[Y]( self:ChainHead[X], link:ChainLink[X,Y] ) = new ChainHead[Y] {
    override def toString = "[Composit Chain]" 
    def iterator() = link.iterator( self ) 
    } 

  def >[Y]( k : ChainLink[X, Y] ):ChainHead[Y] = build_chain(this, k) 
}

object ChainTools {

def literal[X]( iterable : Iterable[X] ) : ChainHead[X] = new ChainHead[X] {
  def iterator = iterable.iterator
  } 

def transform[X]( f:X => X ) = new ChainLink[X, X](f) 

def delimit(delimiter:String) = new ChainLink[Map[String,String], String]({(map:Map[String,String]) => map.values.reduce( {(a,b) => a ++ delimiter ++ b} )  })

def split(delimiter:Char) = new ChainLink[String, Array[String]]( _.split(delimiter) )

def label(headers:Array[String]) = new ChainLink[Array[String], Map[String, String]]( (x:Array[String]) => Map(headers.zip(x):_*) )

def map_to_json() = new ChainLink[Map[String,String], String]( mapToJson(_) )
def json_to_map() = new ChainLink[String, Map[String,String]](JSON.parseFull(_).get.asInstanceOf[Map[String,Any]].mapValues(_.toString) )

def tweek(key:String, value_transform:String => String ) = new ChainLink[Map[String,String], Map[String,String]](map => 
  map + ( key -> value_transform( map(key) ) ) 
  ) 

def templated(str:String) = new ChainLink[String, String]( (x:String) => template(str)(JSON.parseFull(x).get.asInstanceOf[Map[String,String]]) )

def throughprint[X] = new ChainLink[X, X]({ (input:X) => input match { 
    case a : String => println(a); a.asInstanceOf[X]
    case b : AnnotatedString => println(b.body); b.asInstanceOf[X]
    } })

def sinkprint = new ChainSink[String]( println )

def strip_annotations = new ChainLink[AnnotatedString, String]({ x => x.body })
def flatten_annotations = new ChainLink[AnnotatedString, String]({ x => x.annotations.map({x => x._1 + "=" + x._2}).reduceOption({(y, x) => y  + "\n" +  x }).getOrElse("") + "\n" + x.body + "\n" } )
def json_annotations = new ChainLink[AnnotatedString, String]({ inp => 
  val x:Map[String,String] = JSON.parseFull(inp.body) match {
    case Some(x) => x.asInstanceOf[Map[String,String]]
    case _ => Map[String,String]()
    }
  mapToJson(x ++ inp.annotations) 
  })

def extract_annotation(key:String) = new ChainLink[String, AnnotatedString]( x => {
 val js = JSON.parseFull(x).get.asInstanceOf[Map[String,String]]
 val annotate = ( key -> js(key) )
 val body =  mapToJson( js - key )
 new AnnotatedString(body, annotate)
 } )

def annotate(header : Tuple2[String, String]*) = new ChainLink[String, AnnotatedString]( x => new AnnotatedString( x, header:_* ) )
//TODO my method of defining nodes has prevented me from taking advantage of type inference for dispatch : I should have liked to call this
//simply annotate 
def reannotate(header : Tuple2[String, String]*) = new ChainLink[AnnotatedString, AnnotatedString]( x => 
  new AnnotatedString( x.body, (x.annotations ++ header):_* ) ) 

}
