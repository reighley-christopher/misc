package controlserver

import scala.sys.process._
import java.net.ServerSocket
import java.net.Socket
import java.io.OutputStream
import java.io.InputStream
import org.apache.commons.io.IOUtils
import org.apache.commons.io.output.TeeOutputStream
import org.apache.commons.io.input.TeeInputStream
import java.util.Properties 
import java.io.FileInputStream

object Socket2
  {
 
 

  class ProcessManager(jar : String, port : Int, bootscript : String ) extends Runnable 
    {
 
    var server:ServerSocket = new ServerSocket(port) 
    var upstream_in:InputStream = null
    var upstream_out:OutputStream = null
    var downstream_in:Option[InputStream] = None 
    var downstream_out:Option[OutputStream] = None 
    var buffer:Array[Byte] = new Array[Byte](255)
    val proc = Process("scala -classpath " + jar).run( new ProcessIO( 
      {(o:OutputStream) => upstream_out = o },
      {(i:InputStream) => upstream_in = i },
      {(e:InputStream) => }
      ) )

   def loop():Unit =  
     {
     downstream_in match {
     case Some( in ) => { 
       val bytes_up = in.available().min(255)
       in.read(buffer, 0, bytes_up)
       upstream_out.write(buffer, 0, bytes_up)
       upstream_out.flush()
     
     } 
     case None => 
     }

     downstream_out match {
     case Some( out ) => {
     val bytes_down = upstream_in.available().min(255)
     upstream_in.read(buffer, 0, bytes_down)
     //System.out.write(buffer, 0, bytes_down)
     out.write(buffer, 0, bytes_down)
     } 
     case None => 
     }
     Thread.`yield`() 
     }

   def redirect( in:InputStream, out:OutputStream ) = 
      {

      downstream_in = Some(in)
      downstream_out = Some(out)
 
      }

    def run():Unit =
      {
      while(true)
        {
        loop()
        }
      }

    }

  def acceptLoop(proc : ProcessManager ):Unit = {
    val socket:Socket = proc.server.accept()
    println("connection")
    proc.redirect( socket.getInputStream(), socket.getOutputStream() )
    acceptLoop(proc)
    }

  def main(params:Array[String] ):Unit = {
    val properties = {
      val file = params(0) 
      val ret = new java.util.Properties()
      ret.load(new FileInputStream(file))
      ret
      }
    val jar : String = properties.get("controlserver.classpath").asInstanceOf[String]
    val port : Int = properties.get("controlserver.port").asInstanceOf[String].toInt
    val bootscript : String = properties.get("controlserver.bootscript").asInstanceOf[String]
    val proc = new ProcessManager(jar, port, bootscript)
    println("classpath : " + jar )
    println("port : " + port )
    println("bootscript" + bootscript )
    println("properties : " + params(0) )
    //Java will refuse to halt while it's error and output streams are attached (bastard)
    //TODO handle logging now, and write a signal handler for this.
    System.out.close()
    System.err.close()
    new Thread( proc ).start()
    acceptLoop( proc )
    }
  }
