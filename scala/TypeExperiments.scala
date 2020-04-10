import scala.reflect.runtime.universe._
import scala.reflect.ClassTag

/*the problem here is that I don't get to specify the Array on the outside*/ 
def filter1[X](a : Any)(implicit tt: TypeTag[X]):Option[Array[_]] = a match {
  case aa : Array[_]  => Some(aa)
  case _ => None 

  }

/*
/*to write a filter2[X] which passes its parameter only if it is an X*/
/*this too generates the warning: abstract type pattern X is unchecked since it is eliminated by erasure */
/*the output is pathological, must pay attention to that warning as it indicates very unsafe condition*/ 
def filter2[X](a : Any)(implicit tt: TypeTag[X]):Option[X] = a match {
  case aa : X => Some(aa)
  case _ => None
  } 
*/

/*filter3[X] trying to use the typetag now*/
/*
def filter3[X,Y](a : Y)(implicit tt: TypeTag[X], at: TypeTag[Y]):Option[X] = at.tpe match {
  case  tt.tpe => Some(a) // type mismatch here 
  case _ => None 
  }
*/

/*the typetag doesn't really want to give me a way to call the constructor which means it's probably a bad idea*/
def factory1[X]()(implicit tt: TypeTag[X]) = tt.tpe.members
/*
def unpack_array(a : Any) : Option[Array[_]] = a match {
  case aa : Array[_] => Some(aa) 
  case _ => None
 }
*/
def filterCast[X](a : Any)(implicit tt: ClassTag[X] ) : Option[X] = a match {
  case aa : X => Some(aa)    
  case _ => None 
  }

def unpack[X](a : Any)(implicit tt: ClassTag[X] ) : Option[X] = a match {
  case aa : X => Some(aa)    
  case _ => None 
  }


object Extractor {
  def extract[T](list: List[Any])(implicit tag: ClassTag[T]) =
    list.flatMap {
      case element: T => Some(element)
      case _ => None
    }
}



def typetag_of_array[Y : ClassTag ]( x : ClassTag[Array[Y]] )( implicit y : ClassTag[Y] ) = { println(y) ; Some( y ) }

def typetest[T]()( implicit tag : ClassTag[T] ) = tag 

def is_it_an_array[X](tag : ClassTag[X]) = tag match {
  case Array[_] => True
  case _ => False
  } 

object ArrayEx {
  //def typetag_of_array( x : ClassTag[_] ) = None  
  def unapply[Y : ClassTag ]( y : ClassTag[Y] ) = typetag_of_array(y) 
  }

def array_factory2[Y]()( implicit tag : ClassTag[Y] ) = Array[Y]()

/*the hard problem to solve is : given a type X, produce a factory for that type*/
def factory2[X : ClassTag]()(implicit tag : ClassTag[X]) : Option[ClassTag[X]] = tag match {
  case ArrayEx(y) => Some(tag) 
  case _ => None

  } 

/*the idiom  [X : ClassTag] for a parameterized type does something new, but I admit I have no idea what it all means */


val list: List[Any] = List(1, "string1", List(), "string2")
val result = Extractor.extract[String](list)
import java.lang.reflect.Method
def dir( example:Object ):Array[String] = { example.getClass().getMethods().map( { (m:Method)=> m.getName() } ) } 

println(result) // List(string1, string2) 
