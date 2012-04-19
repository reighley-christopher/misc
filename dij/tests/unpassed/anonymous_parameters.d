!!(entry-point)
{
   f := { 
      x:= ;
      do
      !(x==0) -> x := ; printf[c-string:%d\n].x. ; x := []
      od
      }.!!(c-symbol)printf;
   i := 1;
   do
      i < 10 -> f := f.i ; i := i+1 []
   od;
   f := f..12.0.;
   f.11.
}
