!!(entry-point)
{
!!(c-symbol)printf.[c-string:Hello %s I function today\n].([c-string:Junior]). ;
f := { ret := a+printf } /*this line should cause some trouble*/
}

