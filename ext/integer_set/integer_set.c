#include "integer_set.h"
#include <stdlib.h>
#define SHIFT 5
#define MASK 0x3F
typedef unsigned int* BitVector;
typedef unsigned int BitVectorSlice;
typedef unsigned int SetElement;
typedef struct SizedBitVector {
  BitVector vector;
  SetElement max_size;
  size_t size;
} SizedBitVector;

VALUE rb_IntegerSet;
VALUE rb_IntegerSet_Set;

static VALUE allocate(VALUE klass) {
  struct SizedBitVector* bitvector = malloc(sizeof(SizedBitVector));
  bitvector->vector = malloc(sizeof(BitVectorSlice));
  return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, bitvector);
}

static BitVectorSlice integer_set_test_membership(BitVector bitvector, SetElement candidate) {
  return bitvector[candidate >> SHIFT] & 1 << (candidate & MASK);
}

VALUE integer_set_initialize(VALUE self, VALUE max_size) {
  SizedBitVector* bitvector = NULL;
  BitVector ptr_check = NULL;
  SetElement c_size = NUM2UINT(max_size);
  size_t slots = c_size / (sizeof(BitVectorSlice) * 8) + 1;

  Data_Get_Struct(self, SizedBitVector, bitvector);

  ptr_check = (BitVector) realloc(bitvector->vector, sizeof(BitVectorSlice) * slots);

  if (ptr_check == NULL) {
    rb_raise(rb_eNoMemError, "No memory left for set of max_size %u", c_size);
  }

  for (size_t i = 0; i < slots; i++) {
    bitvector->vector[i] = 0;
  }
  bitvector->max_size = NUM2UINT(max_size);
  bitvector->size = 0;
  return self;
}

VALUE integer_set_add(VALUE self, VALUE member) {
  SetElement c_member = NUM2UINT(member);
  SizedBitVector* bitvector;
  size_t index = c_member >> SHIFT;
  SetElement mask = 1 << (c_member & MASK);
  Data_Get_Struct(self, SizedBitVector, bitvector);

  if ((bitvector->vector[index] & mask) == 0) {
    bitvector->size++;
  }

  bitvector->vector[index] |= mask;
  return self;
}

VALUE integer_set_include(VALUE self, VALUE member) {
  SetElement c_member = NUM2UINT(member);
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);
  return integer_set_test_membership(bitvector->vector, c_member);
}

VALUE integer_set_each(VALUE self) {
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);

  for (SetElement i = 0; i < bitvector->max_size; i++) {
    if (integer_set_test_membership(bitvector->vector, i)) {
      rb_yield(UINT2NUM(i));
    }
  }

  return self;
}

VALUE integer_set_empty(VALUE self) {
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);

  if (bitvector->size == 0) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE integer_set_delete(VALUE self, VALUE member) {
  SetElement c_member = NUM2UINT(member);
  size_t index = c_member >> SHIFT;
  SetElement mask = 1 << (c_member & MASK);
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);

  if (bitvector->vector[index] & mask) {
    bitvector->size--;
  }

  bitvector->vector[index] &= ~mask;
  return self;
}

VALUE integer_set_max_size(VALUE self) {
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);
  return UINT2NUM(bitvector->max_size);
}

VALUE integer_set_size(VALUE self) {
  SizedBitVector* bitvector;
  Data_Get_Struct(self, SizedBitVector, bitvector);
  return UINT2NUM(bitvector->size);
}

void Init_integer_set(void) {
  rb_IntegerSet = rb_define_module("IntegerSet");
  rb_IntegerSet_Set = rb_define_class_under(rb_IntegerSet, "Set", rb_cObject);
  rb_define_alloc_func(rb_IntegerSet_Set, allocate);
  rb_define_method(rb_IntegerSet_Set, "max_size", integer_set_max_size, 0);
  rb_define_method(rb_IntegerSet_Set, "size", integer_set_size, 0);
  rb_define_method(rb_IntegerSet_Set, "initialize", integer_set_initialize, 1);
  rb_define_method(rb_IntegerSet_Set, "add", integer_set_add, 1);
  rb_define_method(rb_IntegerSet_Set, "delete", integer_set_delete, 1);
  rb_define_method(rb_IntegerSet_Set, "include?", integer_set_include, 1);
  rb_define_method(rb_IntegerSet_Set, "empty?", integer_set_empty, 0);

  /* Implement enumerable interface */
  rb_define_method(rb_IntegerSet_Set, "each", integer_set_each, 0);
  rb_include_module(rb_IntegerSet_Set, rb_mEnumerable);

  /* Alias methods */
  rb_define_alias(rb_IntegerSet_Set, "member?", "include?");
  rb_define_alias(rb_IntegerSet_Set, "<<", "add");
  rb_define_alias(rb_IntegerSet_Set, "length", "size");
}

