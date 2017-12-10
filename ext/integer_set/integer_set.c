#include "integer_set.h"
#include <stdlib.h>
#define SHIFT 5
#define MASK 0x3F
typedef unsigned int* BitVector;

VALUE rb_IntegerSet;
VALUE rb_IntegerSet_Set;

static VALUE allocate(VALUE klass) {
  BitVector bitvector = malloc(sizeof(unsigned int));
  return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, bitvector);
}

static char integer_set_test_membership(BitVector bitvector, unsigned int candidate) {
  return bitvector[candidate >> SHIFT] & 1 << (candidate & MASK);
}

VALUE integer_set_initialize(VALUE self, VALUE size) {
  BitVector bitvector = NULL;
  BitVector ptr_check = NULL;
  unsigned int c_size = NUM2UINT(size);
  size_t slots = c_size / (sizeof(unsigned int) * 8) + 1;

  Data_Get_Struct(self, unsigned int, bitvector);

  ptr_check = (BitVector) realloc(bitvector, sizeof(unsigned int) * slots);

  if (ptr_check == NULL) {
    rb_raise(rb_eNoMemError, "No memory left for set of size %u", c_size);
  }

  for (size_t i = 0; i < slots; i++) {
    bitvector[i] = 0;
  }
  rb_iv_set(self, "@size", size);
  return self;
}

VALUE integer_set_insert(VALUE self, VALUE member) {
  BitVector bitvector;
  unsigned int c_member = NUM2UINT(member);

  Data_Get_Struct(self, unsigned int, bitvector);

  bitvector[c_member >> SHIFT] |= 1 << (c_member & MASK);
  return self;
}

VALUE integer_set_include(VALUE self, VALUE member) {
  BitVector bitvector;
  unsigned int c_member = NUM2UINT(member);

  Data_Get_Struct(self, unsigned int, bitvector);

  if (integer_set_test_membership(bitvector, c_member)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE integer_set_each(VALUE self) {
  BitVector bitvector;
  unsigned int size = NUM2UINT(rb_iv_get(self, "@size"));

  Data_Get_Struct(self, unsigned int, bitvector);

  for (unsigned int i = 0; i < size; i++) {
    if (integer_set_test_membership(bitvector, i)) {
      rb_yield(UINT2NUM(i));
    }
  }

  return self;
}

void Init_integer_set(void) {
  rb_IntegerSet = rb_define_module("IntegerSet");
  rb_IntegerSet_Set = rb_define_class_under(rb_IntegerSet, "Set", rb_cObject);
  rb_define_alloc_func(rb_IntegerSet_Set, allocate);
  rb_define_attr(rb_IntegerSet_Set, "size", 1, 0);
  rb_define_method(rb_IntegerSet_Set, "initialize", integer_set_initialize, 1);
  rb_define_method(rb_IntegerSet_Set, "insert", integer_set_insert, 1);
  rb_define_method(rb_IntegerSet_Set, "include?", integer_set_include, 1);
  rb_define_method(rb_IntegerSet_Set, "each", integer_set_each, 0);
  rb_include_module(rb_IntegerSet_Set, rb_mEnumerable);
}

