!!(entry-point)
   {
   f := {printf.[c-string:f %d\n].one }.!!(c-symbol)printf;
   g := {printf.[c-string:g %d %d\n].two.one }.printf;
   h := g\f;
   h := h.1;
   h.; /*expect to print f 1*/
   h. := 2; /*expect to print h 1 2*/
   }
