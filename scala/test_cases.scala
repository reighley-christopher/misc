def testcase1:Unit = {
  sql("select now()") > delimit("") > kafka("stupid")
  val s1 = kafka("stupid")
  val s2 = kafka("stupid2")
  s1 > s2
  }

def testcase2:Unit = {
  sql("select now()") > delimit("") > kafka("stupid")
  val s2 = kafka("stupid2")
  val s1 = kafka("stupid")
  s1 > s2
  }
 
