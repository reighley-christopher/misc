package chaintools

import scala.util.{Try, Success, Failure}

class AnnotatedString( _body : String, header : Tuple2[String, String]* ) 
  {
  def gloss( keys : String* ):Seq[String] = keys.map( get )  

//  def get(key : String ) : String = "key"
  
  def get( key : String ) : String = Try(header.filter( pair => pair._1 == key ).head) match
    {
    case Success(vl) => vl._2 
    case Failure(_ : java.util.NoSuchElementException) => ""
    case Failure(exp) => println(exp) ; throw(exp)
    } 

  def body : String = _body 
  }
