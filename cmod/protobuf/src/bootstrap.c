#include "pbc.h"
#include "map.h"
#include "context.h"
#include "pattern.h"
#include "proto.h"
#include "alloc.h"
#include "bootstrap.h"
#include "stringpool.h"
#include "array.h"
#include "descriptor.pbc.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

/*

// Descriptor

// google.protobuf.Descriptor.proto encoded in descriptor.pbc.h with proto pbc.file .

package pbc;

message field {
    optional string name = 1;
    optional int32 id = 2;
    optional int32 label = 3;	// 0 optional 1 required 2 repeated
    optional int32 type = 4;	// type_id
    optional string type_name = 5;
    optional int32 default_int = 6;
    optional string default_string = 7;
    optional double default_real = 8;
}

message file {
    optional string name = 1;
    repeated string dependency = 2;

    repeated string message_name = 3;
    repeated int32 message_size = 4;
    repeated field message_field = 5;

    repeated string enum_name = 6;
    repeated int32 enum_size = 7;
    repeated string enum_string = 8;
    repeated int32 enum_id = 9;
}

*/

typedef struct {
  pbc_slice name;
  int32_t id;
  int32_t label;
  int32_t type;
  pbc_slice type_name;
  int32_t default_integer;
  pbc_slice default_string;
  double default_real;
} field_t;

typedef struct {
  pbc_slice name; // string
  pbc_array dependency; // string
  pbc_array message_name; // string
  pbc_array message_size; // int32
  pbc_array message_field; // field_t
  pbc_array enum_name; // string
  pbc_array enum_size; // int32
  pbc_array enum_string; // string
  pbc_array enum_id; // int32
} file_t;

static void set_enum_one(pbc_env* p, file_t* file, const char* name, int start, int sz) {
  map_kv* table = (map_kv*)_pbcM_malloc(sz * sizeof(map_kv));
  int i;
  for (i = 0; i < sz; i++) {
    pbc_var id;
    pbc_var string;
    _pbcA_index(file->enum_id, start + i, id);
    _pbcA_index(file->enum_string, start + i, string);
    table[i].id = (int)id->integer.low;
    table[i].pointer = (void*)string->s.str;
  }
  _pbcP_push_enum(p, name, table, sz);

  _pbcM_free(table);
}

static void set_enums(pbc_env* p, file_t* file) {
  int n = pbc_array_size(file->enum_size);
  int i;
  int start = 0;
  for (i = 0; i < n; i++) {
    pbc_var name;
    _pbcA_index(file->enum_name, i, name);
    pbc_var var;
    _pbcA_index(file->enum_size, i, var);
    set_enum_one(p, file, name->s.str, start, (int)var->integer.low);
    start += var->integer.low;
  }
}

static void set_default(_field* f, field_t* input) {
  switch (f->type) {
    case PTYPE_DOUBLE:
    case PTYPE_FLOAT:
      f->default_v->real = input->default_real;
      break;
    case PTYPE_STRING:
    case PTYPE_ENUM:
      f->default_v->m = input->default_string;
      break;
    default:
      f->default_v->integer.low = input->default_integer;
      break;
  }
}

static void set_msg_one(pbc_pattern* FIELD_T, pbc_env* p, file_t* file, const char* name,
                        int start, int sz, pbc_array queue) {
  _message* m = _pbcP_create_message(p, name);
  int i;
  for (i = 0; i < sz; i++) {
    pbc_var _field_;
    _pbcA_index(file->message_field, start + i, _field_); // get field pb binary
    field_t field;

    int ret = pbc_pattern_unpack(FIELD_T, &_field_->m, &field); // extract pb binary
    if (ret != 0) {
      continue;
    }
    _field f;
    f.id = field.id;
    f.name = (const char*)field.name.buffer;
    f.type = field.type;
    f.label = field.label;
    f.type_name.n = (const char*)field.type_name.buffer;
    set_default(&f, &field);

    _pbcP_push_field_to_message(m, &f, queue);

    // don't need to close pattern since no array
  }
  _pbcP_build_message_idmap(m);
}

static void set_msgs(pbc_pattern* FIELD_T, pbc_env* p, file_t* file, pbc_array queue) {
  int n = pbc_array_size(file->message_size);
  int i;
  int start = 0;
  for (i = 0; i < n; i++) {
    pbc_var name;
    _pbcA_index(file->message_name, i, name);
    pbc_var sz;
    _pbcA_index(file->message_size, i, sz);
    set_msg_one(FIELD_T, p, file, name->s.str, start, (int)sz->integer.low, queue);
    start += sz->integer.low;
  }
}

static void set_field_one(pbc_env* p, _field* f) {
  const char* type_name = f->type_name.n;
  if (f->type == PTYPE_MESSAGE) {
    f->type_name.m = (_message*)_pbcM_sp_query(p->msgs, type_name);
  } else if (f->type == PTYPE_ENUM) {
    f->type_name.e = (_enum*)_pbcM_sp_query(p->enums, type_name);
    const char* str = f->default_v->s.str; // data from set_default
    if (str && str[0]) {
      int err = _pbcM_si_query(f->type_name.e->name, str, &(f->default_v->e.id));
      if (err < 0)
        goto _default;
      f->default_v->e.name = (const char*)_pbcM_ip_query(f->type_name.e->id, f->default_v->e.id);
    } else {
    _default:
      memcpy(f->default_v, f->type_name.e->default_v, sizeof(pbc_var)); // default_v data from _pbcP_push_enum
    }
  }
}

// deal with nested message reference
void _pbcB_register_fields(pbc_env* p, pbc_array queue) {
  int sz = pbc_array_size(queue);
  int i;
  for (i = 0; i < sz; i++) {
    pbc_var atom;
    _pbcA_index(queue, i, atom);
    _field* f = (_field*)atom->m.buffer;
    set_field_one(p, f);
  }
}

static void _set_string(_pattern_field* f) {
  f->ptype = PTYPE_STRING;
  f->ctype = CTYPE_VAR;
  f->defv->s.str = "";
  f->defv->s.len = 0;
}

static void _set_int32(_pattern_field* f) {
  f->ptype = PTYPE_INT32;
  f->ctype = CTYPE_INT32;
}

static void _set_double(_pattern_field* f) {
  f->ptype = PTYPE_DOUBLE;
  f->ctype = CTYPE_DOUBLE;
}

static void _set_message_array(_pattern_field* f) {
  f->ptype = PTYPE_MESSAGE;
  f->ctype = CTYPE_ARRAY;
}

static void _set_string_array(_pattern_field* f) {
  f->ptype = PTYPE_STRING;
  f->ctype = CTYPE_ARRAY;
}

static void _set_int32_array(_pattern_field* f) {
  f->ptype = PTYPE_INT32;
  f->ctype = CTYPE_ARRAY;
}

#define SET_PATTERN(pat, idx, pat_type, field_name, type) \
  pat->f[idx].id = idx + 1; \
  pat->f[idx].offset = offsetof(pat_type, field_name); \
  _set_##type(&pat->f[idx]);

#define F(idx, field_name, type) SET_PATTERN(FIELD_T, idx, field_t, field_name, type)
#define D(idx, field_name, type) SET_PATTERN(FILE_T, idx, file_t, field_name, type)

static int register_internal(pbc_env* p, pbc_slice* slice) {
  pbc_pattern* FIELD_T = _pbcP_new(p, 8);
  F(0, name, string);
  F(1, id, int32);
  F(2, label, int32);
  F(3, type, int32);
  F(4, type_name, string);
  F(5, default_integer, int32);
  F(6, default_string, string);
  F(7, default_real, double);

  pbc_pattern* FILE_T = _pbcP_new(p, 10);

  D(0, name, string);
  D(1, dependency, string_array);
  D(2, message_name, string_array);
  D(3, message_size, int32_array);
  D(4, message_field, message_array);
  D(5, enum_name, string_array);
  D(6, enum_size, int32_array);
  D(7, enum_string, string_array);
  D(8, enum_id, int32_array);

  int ret = 0;

  file_t file;
  int r = pbc_pattern_unpack(FILE_T, slice, &file); // extract pb binary according to FILE_T, save in &file
  if (r != 0) {
    ret = 1;
    goto _return;
  }

  _pbcM_sp_insert(p->files, (const char*)file.name.buffer, NULL);

  pbc_array queue;
  _pbcA_open(queue);

  set_enums(p, &file);
  set_msgs(FIELD_T, p, &file, queue);
  _pbcB_register_fields(p, queue); // deal with nested message reference

  _pbcA_close(queue);
  pbc_pattern_close_arrays(FILE_T, &file);

_return:
  _pbcM_free(FIELD_T);
  _pbcM_free(FILE_T);
  return ret;
}

void _pbcB_init(pbc_env* p) {
  pbc_slice slice = {pbc_descriptor, sizeof(pbc_descriptor)};
  register_internal(p, &slice);
}
