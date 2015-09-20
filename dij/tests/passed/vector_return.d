/*
test returning multiple values
test a function whose variables are all return values
test assigning from the very end of the stack
*/
!!(entry-point)
{
  z := { a := 2; b := 3};
  c,d := z.; 
  !!(c-symbol)printf.[c-string:VECTOR RETURN -- %d %d (expect 2 3)\n].c.d. 
}
