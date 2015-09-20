!!(entry-point)
{
  i := 0;
  do
    !(i == 10) -> !!(c-symbol)printf.[c-string:DO LOOP -- %d\n].i.;i := i+1 []
  od
}
