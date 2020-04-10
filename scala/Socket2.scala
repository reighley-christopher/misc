import scala.sys.process._
import java.net.ServerSocket
import java.net.Socket
import java.io.OutputStream
import java.io.InputStream
import org.apache.commons.io.IOUtils
import org.apache.commons.io.output.TeeOutputStream
import org.apache.commons.io.input.TeeInputStream

object Socket2
  {
  
  var server:ServerSocket = new ServerSocket(10707) 

  class ProcessManager() extends Runnable 
    {
 
    var upstream_in:InputStream = null
    var upstream_out:OutputStream = null
    var downstream_in:Option[InputStream] = None 
    var downstream_out:Option[OutputStream] = None 
    var buffer:Array[Byte] = new Array[Byte](255)
    val proc = Process("scala").run( new ProcessIO( 
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
     System.out.write(buffer, 0, bytes_down)
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

  val proc = new ProcessManager()

  def acceptLoop():Unit = {
    val socket:Socket = server.accept()
    println("connection")
    proc.redirect( socket.getInputStream(), socket.getOutputStream() )
    acceptLoop()
    }

  def main(params:Array[String]):Unit = {
    new Thread( proc ).start()
    acceptLoop()
    }
  }
