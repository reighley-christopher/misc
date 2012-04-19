/*this test:
uses the tilde operator to create a coroutine.
calls a function from the left.
calls a function without arguments.
passes a function as a parameter.
*/
!!(entry-point)
{
   co := {  co. := in;  out := co. }.( ~({ 
            x := co.;
            printf.[c-string:first %d\n].x.;
            co. := 2;
            x := co.;
            printf.[c-string:third %d\n].x.;
            co. := 4
          }..printf) ) ;
   !!(c-symbol)printf.[c-string:second %d\n].(co.1.).;
   printf.[c-string:fourth %d\n].( co.3. ).
}
