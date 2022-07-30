package chaintools

import java.sql.Connection
import java.sql.ResultSet
import java.sql.Statement
import java.sql.DriverManager
import chaintools.ChainTools._
import scala.collection.mutable.Queue

object Sql { 

val mysql = DriverManager.getConnection("jdbc:mysql://192.168.1.2/Catalog", "jupyter", "")

class SQLInterface( query:String ) extends ChainHead[Map[String,String]] { 

  def iterator = new Iterator[Map[String, String]] { 
    val resultset=mysql.createStatement().executeQuery(query)
    val meta = resultset.getMetaData() 
    var _hasNext = resultset.next()
    def hasNext = _hasNext
    def next() = 
      {
      var q = Queue[(String,String)]()
      for(i <- 1 to meta.getColumnCount()) 
         {
         q += ( meta.getColumnLabel(i) -> resultset.getString(i) )
         }
      _hasNext = resultset.next()
      q.toMap
      }
   
    }
  override def toString() = { "sql(" + query + ")" }
  }

def sql(query: String) = new SQLInterface(query)

}
