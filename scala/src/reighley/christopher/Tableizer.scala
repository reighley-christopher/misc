package reighley.christopher
//this should be written in something more useful than scala

/*
a tabelizer opens a file to write  
the file is a tab delimited text file (maybe other formats later)
the rows end in \n and are of fixed length, the last row is padded by spaces(or .?) after a final tab
every meaningful row terminates with a \t, but if all trailing columns are empty there may not be \t for every row
if a row grows too long the entire file is rewritten (maybe to a tmp file which is then copied over) 
the first row is the column headings
one of the headings is designated as the key, at first the entire file is the name of the key, if the file exists
it is read and used, unless the first characters differ from the name of the key, or if there is a lock file present (same name as table .lock), 
line which case an error is raised.
Tabelizer accepts a dictionary as update, if the key is already present it is amended, preserving any value not mentioned in the dict
if the key is not present a line is added, with values not mentioned in the dict represented as empty strings 
*/

/*
- constructor takes filename and primary key, 

- insert method takes a dict of String to String
  - one of the keys must by the primary key
  - I will establish the line, checking to make sure it is not too long
  - the rewrite procedure if the line is too long is different than if it is short enough
    - if it is to long the file will have to be rewritten, according to the length of the new update
    - if it is short enough the line between the entry in index and the index + length must be rewritten
 
*/

import scala.collection.mutable.Map
import scala.collection.mutable.ArrayBuffer
import java.io.RandomAccessFile
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.File

class Tableizer(var filename:String, var primaryKey:String) {
  var row_length = 32
  val columns:ArrayBuffer[String] = ArrayBuffer[String]()
  val index = Map[String, Long]()
  var (fd:RandomAccessFile, lockfile:File) = ensure_file() //this is annoying, ensure is global and depends on the members above TODO fix
 
  def ensure_file() = {
    var lk = new File(filename + ".lock")
    if( lk.exists() ) throw new Exception("lock file exists")
    lk.createNewFile() 
    //make sure file exists create it if not
    val fd = new RandomAccessFile(filename, "rw")
    if( fd.length > 0 ) 
      { 
      initialize(fd)
      if( columns(0) != primaryKey ) {
        lk.delete()
        fd.close()
        throw new Exception("primaryKey should be %s found %s".format(primaryKey, columns(0)) ) 
        } 
      build_index(fd)
      } else {
      columns += primaryKey
      fd.write((primaryKey + "\t" + ("."*(row_length - 2  - primaryKey.length ) ) + "\n").getBytes)
      }
    (fd, lk) 
    } 

  def rewrite_profile(x:Integer) = x - x%10 + 10 //round up to the nearest multiple of 10 easy to visually check for consistency 
 
  def write_row( key:String, list:String ) =  
    {
    //if key is not in the index, offset will be -1 add it and seek to the end of the file
    if(list.length+2 > row_length) { rewrite( rewrite_profile( list.length+2 ) )  } 
    if(!index.contains(key) ) { index(key) =  fd.length() } 
    val text =   list + "\t" + ("."*(row_length - ( list.length + 2) ) ) + "\n"
    fd.seek(index(key)) //docs say write starts at the file pointer but it seems to start on the next character
    fd.write(text.getBytes) //possibly fd.getFD().sync() after 
    } 

  def add_column(colname:String):Unit = 
    {
    if(columns.contains(colname)) return 
    columns.append(colname)
    val newrow = columns.mkString("\t")
    if(newrow.length+2 > row_length) { rewrite( rewrite_profile( newrow.length+2 ) ) }  
    val pad = newrow + "\t" + ("."*(row_length - ( newrow.length + 2) ) ) + "\n"
    fd.seek(0)
    fd.write(pad.getBytes)
    }

  def initialize(fd:RandomAccessFile) = 
    {
    fd.seek(0)
    val header = fd.readLine()
    row_length = header.length + 1 //readline strips off the ending newline but it counts toward row_length
    columns ++= header.split("\t")
    columns.trimEnd(1) 
    }

  def build_index(fd:RandomAccessFile) =
    {
    var i = row_length 
    val buffer = new Array[Byte](row_length)
    while(i < fd.length)
      {
      fd.seek(i)
      fd.read(buffer)
      index(buffer.map(_.toChar).mkString.split("\t")(0)) = i
      i = i + row_length
      } 
    }

  def read_row(key:String):Map[String,String] = 
    {
    val row = new Array[Byte](row_length)
    if( !index.contains(key) ) 
      {
      val ret = Map[String,String]()
      for(x <- columns) { ret(x) = "" }
      ret(primaryKey) = key
      return ret
      } 
    fd.seek(index(key))
    read_at_current_position(row)
   }

  def read_at_current_position(buffer:Array[Byte]):Map[String,String] =
    {
    val ret = Map[String,String]()
    fd.read(buffer) 
    val spt = buffer.map(_.toChar).mkString.split("\t")
    for(( x, i )  <- columns.zipWithIndex ) ret(x) = if(i < spt.length-1) spt(i) else ""
    ret 
    }

  def update_row(data:Map[String,String]) =
    {
    val oldrow = read_row(data(primaryKey))
    val missing_columns:Set[String] = data.keys.toSet &~ columns.toSet
    for( x <- missing_columns) add_column( x )
    for( x <- columns) { if(!data.contains(x)) data(x) = oldrow(x) } 
    val list =  columns.map( (x) => data(x)  ).mkString("\t")
    write_row( data(primaryKey), list )
    }
  
  def rewrite(new_row_length:Int) =
    {
    val tmp:FileOutputStream = new FileOutputStream(filename + ".tmp")
    val buffer_in:Array[Byte] = new Array(row_length)
    val buffer_rewrite:Array[Byte] = new Array(new_row_length)
    val newrow = columns.mkString("\t")
    var i:Integer = 1
    //write header
    val pad = newrow + "\t" + ("."*(new_row_length - newrow.length - 2)) + "\n"
    tmp.write(pad.getBytes)
    while(i*row_length < fd.length)
      {
      //TODO this is wasteful, don't need to remap the columns just rewrite the end 
      fd.seek(i*row_length)
      val data = read_at_current_position(buffer_in)
      val list =  columns.map( (x) => data(x)  ).mkString("\t")
      val line = list + "\t" + ("."*(new_row_length - list.length - 2)) + "\n"
      tmp.write(line.getBytes)
      index(data(primaryKey)) = i*new_row_length
      i = i + 1 
      }
    tmp.close()
    val tmp_in:FileInputStream = new FileInputStream(filename + ".tmp")
    fd.seek(0)
    i = 0
    val numrows = index.size
    while( i <= numrows ) //be careful of the off by one error : index.size does not include the header line
      {
      tmp_in.read(buffer_rewrite)
      fd.write(buffer_rewrite)
      i = i+1 
      }
    row_length = new_row_length     
    tmp_in.close() 
    }
  
  def close() {
    lockfile.delete()
    fd.close()
    }
 
  override def finalize() = close()

  }

object Tests {
def test1 = {
  val t = new Tableizer("test", "key")
  t.add_column("col1")
  t.add_column("col2")
  t.update_row(Map("key" -> "a", "col1" -> "girl", "col2" -> "funny" ))
  t.update_row(Map("key" -> "b", "col2" -> "dog" ))
  t.update_row(Map("key" -> "a", "col1" -> "boy" ))
  t.add_column("col1") //should be a noop
  try {
    val tt = new Tableizer("test", "key")
    tt.update_row(Map("key" -> "a", "col1" -> "esplode!"))
    } catch { case e:Exception => print("I esplodes : %s\n".format(e.getMessage)) }
  print(t.row_length)
  print(t.columns)
  print(t.index)
  t.close()
  try {
    val tt = new Tableizer("test", "foo")
    tt.update_row(Map("key" -> "a", "col1" -> "esplode!"))
    } catch { case e:Exception => print("I esplodes : %s\n".format(e.getMessage)) }
}

def test2 = {
  val t = new Tableizer("test", "key")
  print(t.row_length)
  print(t.columns)
  print(t.index)
  t.update_row(Map("key" -> "c", "col1" -> "girl", "col2" -> "hot")) 
  t.update_row(Map("key" -> "b", "col1" -> "catcher"))
  t.add_column("col3")
  t.update_row(Map("key" -> "a", "col3" -> "fail"))
  //t.add_column("col4")
  t.update_row(Map("key" -> "c", "col4" -> "success"))
}

def test3 = {
  val t = new Tableizer("test", "key")
  t.update_row(Map("key" -> "a", "col4" -> "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")) 
  print(t.row_length)
  print(t.columns)
  print(t.index)
  }

def test4 = {
  val t = new Tableizer("test", "key")
  print(t.index)
  t.rewrite(60)
  print(t.index)
  t.update_row(Map("key" -> "a", "col4" -> "offset"))
  print(t.index)
  }

def test5 = {
  val t = new Tableizer("test", "key")
  t.update_row(Map("key" -> "a", "coooooooooooooooooooooool3" -> "column") )
  t.update_row(Map("key" -> "c", "coooooooooooooooooooooool3" -> "row", "col1" -> "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "col2" -> "aaaaaaaaaaaaaaaaaaaaaaaaaaa" ) ) 
  }
}
