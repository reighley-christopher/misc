package chaintools

import reighley.christopher.Interactive
import reighley.christopher.CommandDelegate
import HTTPService.detach
import scala.collection.mutable.Queue

/*BUG
does not clean up the thread if an exception is thrown in the UI loop
*/

/*
these are the commands for the interactive block
switch from data entry mode to command mode via ^U (negative acknowledge)
: - to enter a command
h, j, k, l - vi cursor commands
i - switch to data entry mode
x - delete character under cursor
d - print column number into buffer
commands :
attr - set the attributes of the block, keys = values
body - set the body of the block
flush - send the block out the output channel
info - display the current contents, both body and annotations
echo - print "hello world" to test 
quit - exits interactive block
*/

object AnnotatedInteractiveDelegate extends ChainSink[String]( { x => () } ) with CommandDelegate with ChainHead[AnnotatedString] {
  /*acts as delegate for UI commands, manages the UI thread, acts as chainhead and chainsink for */
  var body:String = ""
  var attr:Map[String,String] = Map() 
  var attr_mode:Boolean = false
  var out_buffer:Option[AnnotatedString] = None 
  var print_buffer:Option[String] = None
  var open = true
  var current_mode = "body" 
  val self = this

  override def absorb( iter: Iterable[String] ):Unit = {
    val iterator = iter.iterator
    val t:Thread = new Thread { override def run = { open=true; Interactive.start(self) ; print("thread stopping\n") } }
    t.start()
    while( iterator.hasNext && open ) {
      /*we will yield until the print buffer has been emptied*/
      while(print_buffer != None && open) { Thread.`yield`() }
      if( open ) print_buffer = Some(iterator.next()) 
      }
    print("should exit\n")
    }

  def iterator = new Iterator[AnnotatedString] {
    def hasNext = {
      /*hasNext should just yield until it knows what its answer will be*/
      while( out_buffer == None && open ) { Thread.`yield`() }
      open
      } 
    def next():AnnotatedString = {
      /*we are being called nested within absorb in the main thread, waiting for the ui thread to populate
        the out_buffer
        */
      while( out_buffer == None && open) { Thread.`yield`() }
      /*maybe we got here because I have no intention of populating the out_buffer because I closed, hasNext was lying TODO make hasNext not lie*/ 
      val ret = out_buffer.get
      out_buffer = None
      return ret }
    }

  def parsemap(text:String):Map[String,String] = {
    /*TODO
    trim off spaces
    handle blank lines
    */
    val lines:Array[String] = text.split("\n")
    val arrays:Array[Array[String]] = lines.map { x => x.split("=").padTo(2, "") }
    val pairs:Array[(String,String)] = arrays.map { x => (x(0), x(1)) }
    pairs.toMap   
    }  

  def displaymap(map:Map[String, String]):String = {
    ( map.toArray.map({ x=> x._1 + "=" + x._2}).reduceOption({(x,y) => x+"\n"+y}) ) match { case None => "" ; case Some(x) => x } 
    }

  def modeSave(text:String):Unit = current_mode match {
    case "body" => body = text 
    case "attr" => attr = parsemap(text)
    case _ => {}
    }

  def displayMode():String = current_mode match {
    case "attr" => displaymap(attr)
    case "body" => body
    case "info" => attr.toString + body + out_buffer.toString
    }

  def process(line:String, contents:String):String = {
    line match {
      case "attr" => modeSave(contents) ; current_mode="attr"  
      case "body" => modeSave(contents) ; current_mode="body"
      /*flush will have to synchronize with the rendering thread in order to work
        the flow of control will generally be :
        absorb is called on the ChainSink end, it will be handed an iterator (absorb should be launching the UI thread)
        the iterator will never expect hasNext to be false, but will yield if there is nothing to do (how does it know?)
        process is called from the UI thread, it will populate the out_buffer with a single element and then give control
        back to the main thread, wait until the main thread finishes its work (how will it know?)
        then print the contents of the print_queue
        TODO improve interactive so that there is a way to repaint asynchronously     
       */
      case "flush" => { 
        modeSave(contents)
        out_buffer = Some(new AnnotatedString(body, attr.toSeq:_*)); 
        Thread.`yield`(); 
        while(print_buffer == None) { Thread.`yield`()  }
        this.synchronized {
          print(print_buffer.get)
          print_buffer = None
          }
        }
      case "info" => modeSave(contents); current_mode="info" 
      case "echo" => print("hello world\n")
      case _ => {}
      }
    return displayMode() 
    }

  def get():AnnotatedString = { val ret:AnnotatedString = out_buffer.get ; out_buffer = None ; return ret }
  def close():String = {
    /*this needs to tell the main loop in absorb to stop, but it is being called within the UI thread so
      we are probably already inside the body of the loop in the main thread before this happens 
     */
    print("goodbye\n")
    open = false
    body
    }
  }

object AnnotatedInteractive {
  def interactive() = AnnotatedInteractiveDelegate
  }
