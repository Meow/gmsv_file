#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <gmodc/lua/interface.h>
#include "file.h"

static int file_handle_type;

static FILE *check_handle(luabase_t *LUA, int pos) {
  FILE *f = (FILE *)lua_get_user_type(LUA, pos, file_handle_type);

  if (f == NULL)
    lua_arg_error(LUA, pos, "open file handle expected");

  return f;
}

static int close_handle(luabase_t *LUA) {
  FILE *f = (FILE *)lua_get_user_type(LUA, 1, file_handle_type);

  if (f == NULL)
    return 0;

  lua_set_user_type(LUA, 1, NULL);

  return fclose(f) == 0 ? 1 : 0;
}

LUA_FUNCTION(l_handle_read) {
  FILE *f = check_handle(LUA, 1);
  char *buffer;
  size_t len;

  if (file_handle_read(f, &buffer, &len) != 1) {
    lua_push_bool(LUA, 0);
    return 1;
  }

  lua_push_string(LUA, buffer, (unsigned int)len);

  free(buffer);

  return 1;
}

LUA_FUNCTION(l_handle_write) {
  FILE *f = check_handle(LUA, 1);
  const char *contents;
  unsigned int len;

  lua_check_type(LUA, 2, TYPE_STRING);

  contents = lua_get_string(LUA, 2, &len);

  lua_push_bool(LUA, file_handle_write(f, contents, len));

  return 1;
}

LUA_FUNCTION(l_handle_close) {
  lua_push_bool(LUA, close_handle(LUA));

  return 1;
}

LUA_FUNCTION(l_handle_gc) {
  close_handle(LUA);

  return 0;
}

LUA_FUNCTION(l_file_open) {
  FILE *f;

  lua_check_type(LUA, 1, TYPE_STRING);
  lua_check_type(LUA, 2, TYPE_STRING);

  f = file_open(lua_get_string(LUA, 1, NULL), lua_get_string(LUA, 2, NULL));

  if (f == NULL) {
    lua_push_bool(LUA, 0);
    return 1;
  }

  lua_push_user_type(LUA, f, file_handle_type);

  return 1;
}

LUA_FUNCTION(l_file_write) {
  char *contents;

  lua_check_type(LUA, 1, TYPE_STRING);
  lua_check_type(LUA, 2, TYPE_STRING);

  contents = (char *)lua_get_string(LUA, 2, NULL);

  lua_push_bool(LUA, file_write(lua_get_string(LUA, 1, NULL), contents,
                                strlen(contents)));

  return 1;
}

LUA_FUNCTION(l_file_append) {
  char *contents;

  lua_check_type(LUA, 1, TYPE_STRING);
  lua_check_type(LUA, 2, TYPE_STRING);

  contents = (char *)lua_get_string(LUA, 2, NULL);

  lua_push_bool(LUA, file_append(lua_get_string(LUA, 1, NULL), contents,
                                 strlen(contents)));

  return 1;
}

LUA_FUNCTION(l_file_read) {
  char *buffer;

  lua_check_type(LUA, 1, TYPE_STRING);

  if (file_read(lua_get_string(LUA, 1, NULL), &buffer) != 1) {
    lua_push_bool(LUA, 0);
    return 1;
  }

  lua_push_string(LUA, buffer, 0);

  free(buffer);

  return 1;
}

LUA_FUNCTION(l_file_delete) {
  lua_check_type(LUA, 1, TYPE_STRING);

  lua_push_bool(LUA, file_delete(lua_get_string(LUA, 1, NULL)));

  return 1;
}

LUA_FUNCTION(l_file_mkdir) {
  lua_check_type(LUA, 1, TYPE_STRING);

  lua_push_bool(LUA, file_mkdir(lua_get_string(LUA, 1, NULL)));

  return 1;
}

GMOD_MODULE_OPEN() {
  int global_r, file_r;

  file_handle_type = lua_create_meta_table(LUA, "FileHandle");
  lua_push(LUA, -1);
  lua_set_field(LUA, -2, "__index");
  lua_push_cfunc(LUA, l_handle_gc);
  lua_set_field(LUA, -2, "__gc");
  lua_push_cfunc(LUA, l_handle_read);
  lua_set_field(LUA, -2, "read");
  lua_push_cfunc(LUA, l_handle_write);
  lua_set_field(LUA, -2, "write");
  lua_push_cfunc(LUA, l_handle_close);
  lua_set_field(LUA, -2, "close");
  lua_pop(LUA, 1);

  lua_push_special(LUA, LUA_SPECIAL_GLOB);
  global_r = lua_reference_create(LUA);

  lua_create_table(LUA);
  file_r = lua_reference_create(LUA);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "open", 0);
  lua_push_cfunc(LUA, l_file_open);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "write", 0);
  lua_push_cfunc(LUA, l_file_write);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "read", 0);
  lua_push_cfunc(LUA, l_file_read);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "append", 0);
  lua_push_cfunc(LUA, l_file_append);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "delete", 0);
  lua_push_cfunc(LUA, l_file_delete);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "mkdir", 0);
  lua_push_cfunc(LUA, l_file_mkdir);
  lua_set_table(LUA, -3);

  lua_reference_push(LUA, global_r);
  lua_push_string(LUA, "File", 0);
  lua_reference_push(LUA, file_r);
  lua_set_table(LUA, -3);
  lua_reference_free(LUA, file_r);

  lua_reference_free(LUA, global_r);

  return 0;
}

GMOD_MODULE_CLOSE() { return 0; }
