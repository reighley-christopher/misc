package controlserver 

import scala.sys.process._
import java.io.FileOutputStream
import java.io.File
import org.apache.commons.io.IOUtils     
import java.nio.charset.StandardCharsets 
import java.lang.Thread
import reighley.christopher.Util

class FanInOut(directory:String)
  {
  val (outp, inp) = Util.simpleProcess(directory +  "/bin/faninout")
  def input(name:String) =
    {
    if( ! ( new File( directory + "/" + name) ).exists )
      {
      ( "mkfifo " + directory + "/" + name ) !!
      }
    outp.write(( "i " + directory + "/" + name + "\n").getBytes)
    outp.flush()
    }
  def output(name:String) =
    {
    if( ! ( new File( directory + "/" + name) ).exists )
      {
      ( "mkfifo " + directory + "/" + name ) !!
      }
    outp.write(("o " + directory + "/" + name + "\n").getBytes)
    outp.flush()
    }
  def execute(path:String) =
    {
    outp.write(("e " + path + "\n").getBytes)
    outp.flush()
    }
  def attribute(name:String, value:String) =
    {
    outp.write(("a " + name + " " + value + "\n").getBytes )
    outp.flush()
    }
  def exit() = 
    {
    outp.write("x\n\n".getBytes )
    outp.flush()
    }
  def flush():String =
    {
    val bytes = new Array[Byte](inp.available())
    print("available %d\n", inp.available())
    inp.read(bytes)
    IOUtils.toString(bytes) 
    }
  def query() = 
    {
    outp.write("?\n\n".getBytes)
    outp.flush()
    }
  def test(s:String) = 
    {
    outp.write(s.getBytes)
    outp.flush()
    }
  def noisy() = 
    ( new Thread { 
      override def run() = { 
    //TODO will we just kill the thread when the input closes? by throwing an exception?
        while(true)
          {
          if( inp.available() > 0 )
            {
            val bytes = new Array[Byte](inp.available())
            inp.read(bytes)
            print( IOUtils.toString(bytes) ) 
            }
          }
        }
    } ).start() 
  }

object FanInOut 
  {
  def setup(directory:String) =
    {
    try
      {
      ("mkdir " + directory ) !! ; 
      ("mkdir " + directory + "/bin") !! ;
      } catch {
      case _: Throwable =>
      }
    if( ! ( new File(directory + "/bin/faninout") ).exists  )
      {
      val binaryStream = getClass.getClassLoader.getResourceAsStream("controlserver/bin/faninout")
      val executable = new FileOutputStream( directory + "/bin/faninout"  )
      IOUtils.copy(binaryStream, executable)
      executable.close() 
      ("chmod 700 " + directory + "/bin/faninout") ! ;
      }
    }
 
  def apply(directory:String):FanInOut =
    {
    setup(directory);
    new FanInOut(directory)
    }
  }
