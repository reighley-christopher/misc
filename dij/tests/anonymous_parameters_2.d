!!(entry-point)
   {
   f := { x := ; printf.[c-string:%d].x }.!!(c-symbol)printf;
   f := (f.2):f;
   f.3; /*this prints 23*/
   g := (print.[c-string:%d]):(printf.[c-string:%d]);
   g.4. /*the only thing this can reasonably be expected to do is print 44*/
   h := (printf.[c-string:%d %d].5):(printf.c-string:%d %d].6);
   h.7. /*then this should print 5767*/ 
   }
