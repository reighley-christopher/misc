import javax.script.Compilable
import scala.tools.nsc.interpreter._
import scala.tools.nsc.Settings
import scala.compat.Platform
import jdk.internal.loader.ClassLoaders
import jdk.internal.loader.ClassLoaders._

object test
  {
  def main( args : Array[String] ) : Unit = {
    val lo = ClassLoaders.appClassLoader()
    print(System.getProperty("java.class.path") )
    //val mm = Platform.getClassForName("IMain")
    val m = new IMain( new Settings() )
    println( m.getClass().getClassLoader()) 
    // java.lang.Class.forName("javax.script.ScriptEngineManager") 
    } 
  }

