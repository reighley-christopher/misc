!!(entry-point)
{
  if
    0 -> !!(c-symbol)printf.[c-string:IF TEST -- THIS SHOULD NOT RUN\n]. []
    -1 -> !!(c-symbol)printf.[c-string:IF TEST -- THIS SHOULD RUN\n]. []
  fi
}
