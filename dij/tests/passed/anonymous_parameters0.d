!!(entry-point)
{
   f := { 
      x := ;
      do
      !(x==0) -> printf.[c-string:ANONYMOUS_PARAMETER -- %d\n].x. ; x :=  []
      od
      }.!!(c-symbol)printf;
   i := 1;
   do
      !(i == 10) -> f := f.i ; i := i+1 []
   od ;   
   /*f := f..12.0.; */ /*note the curry at the front of the call here, this breaks it*/
   f.10.  
}
