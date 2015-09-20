!!(entry-point)
   {
   /*this will print ANON 2 twice, but the answer should be :
     but we do want to be able to absorb the 3 somehow
     the problem is that the anonymous parameters don't know which part of the
     anonymous memory space they are going to live in.
   */
   f := { x := ; printf.[c-string:ANON %d\n].x. }.!!(c-symbol)printf;
   f := (f.2):f ;
   f.3. ; 
   /**/
   g := (printf.[c-string:ANON %d\n]):(printf.[c-string:ANON %d\n]);
   g.4.; 
   h := (printf.[c-string:ANON %d %d\n].5):(printf.[c-string:%d %d\n].6);
   h.7. 
   }
