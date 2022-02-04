package reighley.christopher

import java.lang.reflect.Method

object Util {
  def mapToJson( map : Map[String,String] ) : String = "{" + map.map( x => "\"" + x._1 + "\":\"" + x._2 + "\"").mkString(",") + "}" 
  def dir( example:Object ):Array[String] = {
    example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) 
    }
  }
