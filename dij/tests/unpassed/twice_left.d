!!(entry-point)
   {
   f := {printf.[c-string:LEFT -- %d (expect 1) \n].one. }.!!(c-symbol)printf;
   g := {printf.[c-string:LEFT -- %d %d (expect 1 2)\n].two.one. }.printf;
   h := g\f;
   h := h.1;
   h.; /*expect to print f 1*/
   h. := 2 /*expect to print h 1 2*/
   }
