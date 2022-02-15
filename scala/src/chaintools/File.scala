package chaintools

import ChainTools._
import scala.io.Source 
import reighley.christopher.Util.IterableIterable
import java.io.FileWriter
import java.io.{File => jFile}

object File
  {
  def read_fifo(path:String) = literal(new IterableIterable( () => Source.fromFile(path).getLines() ) ) 
  def write_fifo(path:String) = {
    val fileWriter = new FileWriter(new jFile(path) )
    new ChainSink[String]( str => { fileWriter.write(str); fileWriter.flush() } )
    }
  } 
