#include "integer_set.h"
#include <stdlib.h>
#define SHIFT 5
#define MASK 0x3F
typedef unsigned int* BitVector;
typedef unsigned int BitVectorSlice;
typedef unsigned int SetElement;

VALUE rb_IntegerSet;
VALUE rb_IntegerSet_Set;

static VALUE allocate(VALUE klass) {
  BitVector bitvector = malloc(sizeof(BitVectorSlice));
  return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, bitvector);
}

static BitVectorSlice integer_set_test_membership(BitVector bitvector, SetElement candidate) {
  return bitvector[candidate >> SHIFT] & 1 << (candidate & MASK);
}

VALUE integer_set_initialize(VALUE self, VALUE max_size) {
  BitVector bitvector = NULL;
  BitVector ptr_check = NULL;
  SetElement c_size = NUM2UINT(max_size);
  size_t slots = c_size / (sizeof(BitVectorSlice) * 8) + 1;

  Data_Get_Struct(self, BitVectorSlice, bitvector);

  ptr_check = (BitVector) realloc(bitvector, sizeof(BitVectorSlice) * slots);

  if (ptr_check == NULL) {
    rb_raise(rb_eNoMemError, "No memory left for set of max_size %u", c_size);
  }

  for (size_t i = 0; i < slots; i++) {
    bitvector[i] = 0;
  }
  rb_iv_set(self, "@max_size", max_size);
  return self;
}

VALUE integer_set_add(VALUE self, VALUE member) {
  SetElement c_member = NUM2UINT(member);
  BitVector bitvector;
  Data_Get_Struct(self, BitVectorSlice, bitvector);

  bitvector[c_member >> SHIFT] |= 1 << (c_member & MASK);
  return self;
}

VALUE integer_set_include(VALUE self, VALUE member) {
  SetElement c_member = NUM2UINT(member);
  BitVector bitvector;
  Data_Get_Struct(self, BitVectorSlice, bitvector);

  if (integer_set_test_membership(bitvector, c_member)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE integer_set_each(VALUE self) {
  SetElement max_size = NUM2UINT(rb_iv_get(self, "@max_size"));
  BitVector bitvector;
  Data_Get_Struct(self, BitVectorSlice, bitvector);

  for (SetElement i = 0; i < max_size; i++) {
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
  rb_define_attr(rb_IntegerSet_Set, "max_size", 1, 0);
  rb_define_method(rb_IntegerSet_Set, "initialize", integer_set_initialize, 1);
  rb_define_method(rb_IntegerSet_Set, "add", integer_set_add, 1);
  rb_define_method(rb_IntegerSet_Set, "include?", integer_set_include, 1);

  /* Implement enumerable interface */
  rb_define_method(rb_IntegerSet_Set, "each", integer_set_each, 0);
  rb_include_module(rb_IntegerSet_Set, rb_mEnumerable);

  /* Alias methods */
  rb_define_alias(rb_IntegerSet_Set, "member?", "include?");
  rb_define_alias(rb_IntegerSet_Set, "<<", "add");
}

