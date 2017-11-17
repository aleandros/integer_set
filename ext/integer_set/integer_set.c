#include "integer_set.h"

VALUE rb_mIntegerSet;

void
Init_integer_set(void)
{
  rb_mIntegerSet = rb_define_module("IntegerSet");
}
