package chaintools

class ChainSink[X](f : X => Unit) {
  def func = f
  def absorb(iter:Iterable[X]):Unit = iter.foreach( f )
  }

class ChainLink[X,Y](f : X => Y) {
  def iterator(iter:ChainHead[X]):Iterator[Y] = new Iterator[Y] { 
    val ator = iter.iterator
    def hasNext = ator.hasNext
    def next() = f(ator.next)
    } 
  // def >( k : ChainSink[Y] ) : ChainSink[X] 
  // def >[Y]( k : ChainLink[X,Y] ) : ChainLink[X, Y]
}

trait ChainHead[X] extends Iterable[X] {

  def >( k : ChainSink[X] ) = {
    k.absorb(this)
    }

  def build_chain[Y]( self:ChainHead[X], link:ChainLink[X,Y] ) = new ChainHead[Y] {
    override def toString = "[Composit Chain]" 
    def iterator() = link.iterator( self ) 
    } 

  def >[Y]( k : ChainLink[X, Y] ):ChainHead[Y] = build_chain(this, k) 
}

object ChainTools {

def literal[X]( iterable : Iterable[X] ) : ChainHead[X] = new ChainHead[X] {
  def iterator = iterable.iterator
  } 

def delimit(delimiter:String) = new ChainLink[Map[String,String], String]({(map:Map[String,String]) => map.values.reduce( {(a,b) => a ++ delimiter ++ b} )  })

def throughprint[X] = new ChainLink[X, X]({ (input:X) => input match { 
    case a : String => println(a); a.asInstanceOf[X]
    case b : AnnotatedString => println(b.body); b.asInstanceOf[X]
    } })

def sinkprint = new ChainSink[String]( println )

def strip_annotations = new ChainLink[AnnotatedString, String]({ x => x.body })
def flatten_annotations = new ChainLink[AnnotatedString, String]({ x => x.annotations.map({x => x._1 + "=" + x._2}).reduceOption({(y, x) => y  + "\n" +  x }).getOrElse("") + "\n" + x.body + "\n" } )
}
