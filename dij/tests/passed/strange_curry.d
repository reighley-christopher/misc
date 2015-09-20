/* 

   SPEC the meaning of namespace join is this :
   SPEC if F == {A}
   SPEC and G == {B}
   SPEC then F:G == {A;B}
   SPEC this has the somewhat undesirable effect of
   SPEC ensuring that a variable which is used in F
   SPEC is disqualified as a return value from G

   SPEC the semantics of . is such that, if a is the
   SPEC first parameter variable in F then F.X is equivalent
   SPEC to {a := X}:F.
   another pathological case : 3 functions share
   a variable but the middle one has been curried
   out. Does the third one pick up the old value
   from the end of the first one?  Or does it use
   the curried value
   the former case is the desired behavior*/

!!(entry-point)
{
   f := { c := a+b }; /*one return value (c), two parameters (a, b)*/
   g := { a := d+e }.10; /*one return value (a), one parameter(e), one local(d)*/
   h := f:g; /*the return value of g will be shadowed away by the parameter in f*/          
   c := h.20.30.50.;
   !!(c-symbol)printf.[c-string:STRANGE CURRY -- %d (expect 50)\n].c.; 

/*
{a:=1}:({b:=a}.2):{return:=a} ->
{a:=1}:{a:=2}:{b:=a}:{return:=a} ->
no paramters, two return values both 2
*/
   f := { a := 1 };
   g := { b := a }.2;
   h := { return := a };

  /*a,b := (f:g:h).;*/
  z := (f:g:h);
  c := z.; /*as of the writing of this test double returns don't work*/ 
  printf.[c-string:STRANGE CURRY -- %d (expect 2)\n].c. 
}
