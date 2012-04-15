
!!(entry-point)
{

  object_manifest :=
     {
     sum_internal := 0 ;
     add_to := 1 ;
     get_both := 2 ;
     get_index := 3
     };

   object_constructor := {
     storage_process :=
     {
       method := co.;
       value := 0;
       do
         method == 0 -> value := co. ;  co. := 0 ; method := co. []
         method == 1 -> co. := value ; method := co. []
       od
     };
  
    storage_template := 
        {co. := 0 ; co. := value; co. }\{ co. := 1 ; return := co. };
     storage1 := storage_template.(~storage_process);
     storage2 := storage_template.(~storage_process);

   /*this method takes no parameter, and returns no values*/
   method1 := {
              a_ := a + b           
              };

   /*this method takes a parameter, but returns no values*/
   method2 := {
              a_ := a + c;
              b_ := b + c
              };

   /*this method takes a parameter, and returns two values*/
   method3 := {
              return1 := a;
              return2 := b
              };

   /*this method takes a parameter, and returns one value*/
   method4 := {
              if
                 parameter == 0 -> return := a []
                 parameter == 1 -> return := b []
              fi
              };

   dispatch := object_namespace:{
               if
                  method == sum_internal -> return := 
                      { storage1. := 
                       method1.(storage1.).(storage2.). }.storage1.storage2 []
                  method == add_to -> return :=
                      { storage1.,storage2. :=
                       method2.(storage1.).(storage2.). }.storage1.storage2 []
                  method == get_both -> return := method3.(storage1.).(storage2.) []
                  method == get_index -> return := method4..(storage1.).(storage2.) []
               fi
               }..storage1.method1.storage2.method2.method3.method4

   }.object_manifest;

   object_manifest:{
   object1 := object_constructor.;
   object2 := object_constructor.;
   object1.add_to.1.;
   object1.sum_internal..;
   object2.add_to.5.;
   a1, b1 := object1.get_both..;
   a2, b2 := object2.get_index.0., object2.get_index.1.;
   printf.[c-string:  RESULT = %d %d %d %d\n].a1.b1.a2.b2. 
   }.!!(c-symbol)printf.

}
