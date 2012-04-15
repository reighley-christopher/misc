!!(entry-point){
namespace := { printf := ; printf.[c-string:JOIN TEST\n]. }.printf ; 
left := { printf.[c-string:LEFT %d\n].x. }.printf ;
right := namespace:{ printf.[c-string:RIGHT\n]. ; return := 42 }; 
joint := left\right;
joint. := 37;
!!(c-symbol)printf.[c-string:JOINT %d\n].(joint.).
}
