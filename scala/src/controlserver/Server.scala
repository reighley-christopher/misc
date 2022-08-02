package controlserver

import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.ServerSocket
import java.net.Socket
import java.io.PrintWriter
import java.io.InputStream
import java.io.OutputStream
import scala.tools.nsc.Settings
import scala.tools.nsc.interpreter._

//I think I don't use this at all because in 2.11 the interpreter was hopelessly broken
//TODO maybe in 2.13 it works and we can replace Socket2 with something that makes more sense

object Server {

  val server = new ServerSocket(10707)

  object C {
    val x = 2
    }

  class MyLoop {
    //ILoop doesn't work, keeps nulling out the IMain interpretter, so I'm going to write my own
    private var settings = new Settings()
    private var intp : IMain = new IMain(settings, new shell.ReplReporterImpl(settings))
    private var reader : BufferedReader = null
    private var out : OutputStream = null
    private var writer : PrintWriter = null 
    def this(reader : BufferedReader, out : OutputStream ) = {
       this()
       this.writer = new PrintWriter(out)
       this.out = out
       this.reader = reader 
       }

    def printWelcome() : Unit = {
      this.writer.println("connected")
      this.writer.flush() 
      }

    def interpret(s:String) : Unit = { this.intp.interpret(s) }

    def process(settings:Any) : Unit = {
      this.writer.print("> ")
      this.writer.flush()
      val line = this.reader.readLine()
      line match {
        case ":quit" => { print("command") ; return }
        case expr => Console.withOut( this.out ){ Console.withErr( out ) {  this.intp.interpret(expr) } }
        } 
      this.writer.flush() 
      process(settings)
      }

    def bind( n:String, t:String, v:Any):Results.Result = {
      intp.bind(n, t, v)
      }

    }

  def getILoop(in0: InputStream, out: OutputStream):MyLoop = {
    //ILoop takes a BufferdReader in0
    //and a JPrintWriter out
    val loop = new MyLoop( new BufferedReader( new InputStreamReader( in0 ) ), out ) 
    loop
    }

  def acceptLoop():Unit = {
    val socket:Socket = server.accept()
    println("connection")
    val thread = new Thread( new Runnable { def run() { startServer(socket) } } )
    thread.start()
    acceptLoop() 
    }

  def startServer(socket: Socket):Unit = {
    val in = socket.getInputStream() 
    val out = socket.getOutputStream()
    val loop = getILoop( in , out )
    loop.printWelcome()
    loop.process(new Settings())
    socket.close()
    }

  def main(args: Array[String]):Unit = { startServer( server.accept() ) } 

}

