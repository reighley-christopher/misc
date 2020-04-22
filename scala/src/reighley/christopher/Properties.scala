package reighley.christopher

import java.util.Properties
import java.io.FileInputStream

object Properties  {
  val properties = {
    val file = System.getenv().get("CRPROPS")
    val ret = new java.util.Properties()
    ret.load(new FileInputStream(file))
    ret
    }
}
