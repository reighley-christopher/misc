/* the purpose of this experiment is to force
   the variables which would normally be bound
   to the first memory slot in g to be bound to
   the second one in f:g, and yet to be curried
   properly 

   another pathological case : 3 functions share
   a variable but the middle one has been curried
   out. Does the third one pick up the old value
   from the end of the first one?  Or does it use
   the curried value
   the former case is the desired behavior*/

!!(entry point)
{
   f := { c := a+b };
   g := { a := d+e }.1;
   h := f:g;

   f := { a := 1 };
   g := { b := a }.2;
   h := { return := a };
}
