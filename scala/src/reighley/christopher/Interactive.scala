package reighley.christopher 

import scala.io.StdIn
import scala.collection.mutable.ArrayBuffer

/*Interactive will use STDIN and STDOUT to use with chaintools
you will need to be able to annotate the string, and send messages that are many lines long
end an interactive session with esc ?like VI? expecting VT100 terminal commnads 
*/

/*
BUGS 
backspace doesn't work in command mode
unprintable characters still move the cursor position right
*/

/*
TODO
if a line under the delete is blank remove it
if the cursor is at the far end of the line on delete merge the line under it 
vi commands 'y' 'd' 'p'
command line mode - delegate command actions to the command handler object
*/

/*
notes for the inevitable rewrite
GraphicsManager should always keep track of the cursor
BufferManager should be accessed only through GraphicsManager
should just have a function "update" which handles a box of the buffer, would be cleaner
possibly an "invalidate", which would take some work but be cleaner still
ideally all operations on BufferManager would be tolerant of out of bounds indicies and retroactively fill the buffer with spaces
*/

trait CommandDelegate {
  def process(line:String, contents:String):String
  def close():String
  }

class SimpleCommandDelegate() extends CommandDelegate {
  var data:String = ""
  def process(line:String, contents:String):String = {
    line match {
      case "save" => data = contents
      case "echo" => print("hello world\n")
      case _ => {}
      }
    return "[Intentionally Blank]" 
    }
  def close():String = data
  }

class BufferManager {
  var buffer:ArrayBuffer[String] = ArrayBuffer[String]("")
  def insert_line(row:Int, col:Int) = 
    {
    if(buffer(row).length < col+1) buffer(row) = buffer(row).padTo(col, ' ') 
    buffer.insert(row+1, buffer(row).substring(col) ) 
    buffer(row) = buffer(row).substring(0,col) 
    }
  def get_line(row:Int, col:Int):String = 
    {
    if(buffer(row).length < col+1) buffer(row) = buffer(row).padTo(col, ' ') 
    buffer(row).substring(col)
    }
  def insert_char(c:Int, row:Int, col:Int) = 
    { 
    if(buffer(row).length < col+1) buffer(row) = buffer(row).padTo(col, ' ') 
    buffer(row)=buffer(row).substring(0,col) + c.asInstanceOf[Char] + buffer(row).substring(col) 
    }
  def delete(row:Int, col:Int) = 
    {
    if(buffer(row).length > col) buffer(row) = buffer(row).substring(0, col) + buffer(row).substring(col+1) 
    } 
  def contents:String = buffer.reduce( (l,r) => l+"\n"+r ) 
  def reinitialize(data:String) = { buffer = ArrayBuffer[String](); buffer.prependAll(data.split("\n")) }
  def lines = buffer.length
  }

class GraphicsManager {
  var row = 0
  var col = 0
  var max_row = 0
  def diagnostic(n:Int) = { print("%d".format(n)); System.out.write(0x1b); System.out.write("[1D".getBytes) }
  def cursor_up(n:Int) = { System.out.write(0x1b); System.out.write("[%dA".format(n).getBytes) ; row = row-n;  }
  def cursor_down(n:Int) = { System.out.write(0x1b); System.out.write("[%dB".format(n).getBytes) ; this.row = this.row+n }
  def cursor_right(n:Int) = { if(n > 0) { System.out.write(0x1b); System.out.write("[%dC".format(n).getBytes) ; this.col = this.col+n } } /*vt100 behavior not what I expect if I send  [0C probably doesn't work elsewhere either*/
  def cursor_left(n:Int) = { System.out.write(0x1b); System.out.write("[%dD".format(n).getBytes) ; this.col = this.col-n } 
  def upper_corner = { System.out.write("╔═".getBytes) }
  def left_margin =  { System.out.write("║".getBytes) }
  def lower_corner = { System.out.write("╚═".getBytes) }
  def command_prompt = { System.out.write("╡:".getBytes) }
  val command_buffer = new StringBuilder
  def write_stationary(str:String) = { print(str) ; if(str.length > 0) { System.out.write(0x1b); System.out.write("[%dD".format(str.length).getBytes)} }
  def add_line(buffer:BufferManager) = {
    val move = max_row - row
    var i:Int = 0 
    var erase = buffer.get_line(row, col-1).length 
    System.out.write( ( (" "*erase) + "\r").getBytes )
    System.out.write('\n')
    buffer.insert_line(row, col-1)  /*-1 because we have already incremented col to account for the \r TODO bad juju*/
    this.row = this.row+1
    this.max_row = this.max_row+1
    for(i <- row to max_row )
      {
      if( i < max_row ) {
        erase = buffer.get_line(i+1, 0).length+1 /*+1 because I haven't drawn the left margin yet*/ 
        print( (" " * erase ) + "\r")
        }
      left_margin
      print(buffer.get_line(i,0))
      print("\n\r")
      this.row = this.row+1
      }  
    lower_corner
    col = 1 /*when we complete the lower corner this is where we are*/ 
    cursor_up(1)
    cursor_left(1)
    if( buffer.get_line(max_row,0) == "") { 
      System.out.write(" \b".getBytes)
      }  
    cursor_down(1) 
    cursor_up( move + 1 ) 
    }

  def delete(buffer:BufferManager) = {
    /*delete the character under the cursor*/
    buffer.delete(row, col)
    write_stationary(buffer.get_line(row, col) + " ")
    }

  def change_prompt(prompt:String) = {
    var movev = max_row - row
    var moveh = col
    cursor_down( movev + 1 )
    System.out.write('\r')
    lower_corner
    col = 1 
    System.out.write(prompt.getBytes)
    col = col + prompt.length
    cursor_left( prompt.length + 1 )
    cursor_right( moveh )
    cursor_up( movev + 1)
    }

  def clear(buffer:BufferManager):Unit = {
    /*clear will start with the cursor on the command line, and end with the cursor at the top*/
    var i = 0
    clear_command_line()
    System.out.write(0x1b); System.out.write("[%dA".format(max_row+2).getBytes)
    print("  \n\r")
    for(i <- 0 to max_row) {
      print( ( " " * ( buffer.get_line(i, 0).length+1 ) ) + "\n\r" )
      }
    print("    ")
    System.out.write(0x1b); System.out.write("[%dA\r".format(max_row+2).getBytes)
    }

  def clear_command_line() = System.out.write(("\r╚═╛" + " "*(command_buffer.length + 2) + "\r" ).getBytes) 

  def repaint(buffer:BufferManager):Unit = {
    /*repaint will start with cursor above the first line and end with the cursor on the command line*/
    var i:Int = 0
    upper_corner
    System.out.write('\n')
    max_row = buffer.lines-1
    if(row > max_row) row = max_row
    for( i <- 0 to max_row ) {
      left_margin
      print(buffer.get_line(i, 0) + "\r\n")
      } 
    System.out.write("\r>>>>".getBytes)
    //lower_corner 
    //System.out.write(("\r╚═╛" + " "*(command_buffer.length + 2) + "\r" ).getBytes) 
    System.out.write('\b')
    }

  def cursor_prompt = { change_prompt("╛") }

  def command = command_buffer.toString

  def command_mode() = {
    /*changes the prompt, moves the actual cursor, but keeps the cursor ref where it is*/
    System.out.write(0x1b); System.out.write("[%dB".format(max_row - row + 1).getBytes)   
    System.out.write("\r╚═╡:".getBytes)
    }

  def command_input(c : Int) =
    {
    command_buffer += c.asInstanceOf[Char]
    System.out.write(c)
    System.out.flush()
    }

  def return_from_bottom() = {
    clear_command_line 
    System.out.write(0x1b) 
    System.out.write("[%dA".format(max_row-row + 1).getBytes) 
    System.out.write(0x1b)
    System.out.write("[%dC".format(col+1).getBytes) 
    command_buffer.clear()
    } 

  def initialize(buffer:BufferManager) = {
    /*draws the bracket with one line in it*/
    var i:Int = 0
    row = 0
    col = 0
    max_row = buffer.lines-1
    repaint(buffer)
    return_from_bottom()
    change_prompt("  ") 
   }
  }

object Interactive {

  def start(delegate:CommandDelegate):String = {
    val graphics = new GraphicsManager
    val buffer = new BufferManager()
    buffer.reinitialize("")
    graphics.initialize(buffer)
    System.out.flush()
    var mode:Int = 0 
    while(true) {
      val x  = System.in.read()
      mode match {
        case 0 => {
          //System.out.write(x)
          graphics.col = graphics.col+1
          x match {
          case '\r' => { graphics.add_line(buffer) }
          case 127 => { graphics.col = graphics.col-1; if(graphics.col > 0) graphics.cursor_left(1) ; graphics.delete(buffer) } 
          case 21 => { mode = 1; graphics.col = graphics.col-1; graphics.cursor_prompt  } /* ^U, char 21 is Negative Acknowledge, I guess nobody uses it much so why not TODO make sure it still works via SSH*/
          case _ => { System.out.write(x) ; buffer.insert_char(x, graphics.row, graphics.col-1) ; graphics.write_stationary(buffer.get_line(graphics.row, graphics.col)) }
          }
          System.out.flush()
        }
        case 1 => {
          x match {
            case 'h' => {graphics.cursor_left(1)}
            case 'j' => {graphics.cursor_down(1)}
            case 'k' => {graphics.cursor_up(1)}
            case 'l' => {graphics.cursor_right(1)}
            case 'd' => {print("%d".format(graphics.col))}
            case 'x' => {graphics.delete(buffer)}
            case 'i' => {graphics.change_prompt("  "); mode = 0}
            case ':' => {graphics.command_mode() ; mode = 2}
            case _ => {}
            } 
        }
        case 2 => {
          if(x == '\r') { 
            if(graphics.command == "quit") 
              {
              return delegate.close()
              } else {
              graphics.clear(buffer); 
              buffer.reinitialize( 
                delegate.process(graphics.command, buffer.contents)
                )
              } ; 
            graphics.repaint(buffer) ; 
            graphics.return_from_bottom() ; mode = 1 
            } else {
            graphics.command_input(x.asInstanceOf[Char])
            }
        }
        case _ => {}
        }
      }
    return "THIS SHOULD BE UNREACHABLE"
    }  
  }
