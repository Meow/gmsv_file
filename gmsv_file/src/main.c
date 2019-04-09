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

LUA_FUNCTION(l_file_new) {
  lua_push_string(LUA, "not implemented", 0);

  return 1;
}

LUA_FUNCTION(l_file_write) {
  char *contents;

  lua_check_type(LUA, 1, TYPE_STRING);
  lua_check_type(LUA, 2, TYPE_STRING);

  contents = (char*)lua_get_string(LUA, 2, NULL);

  lua_push_bool(LUA, file_write(lua_get_string(LUA, 1, NULL), contents, strlen(contents)));

  return 1;
}

LUA_FUNCTION(l_file_append) {
  char *contents;

  lua_check_type(LUA, 1, TYPE_STRING);
  lua_check_type(LUA, 2, TYPE_STRING);

  contents = (char*)lua_get_string(LUA, 2, NULL);

  lua_push_bool(LUA, file_append(lua_get_string(LUA, 1, NULL), contents, strlen(contents)));

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

  lua_push_special(LUA, LUA_SPECIAL_GLOB);
  global_r = lua_reference_create(LUA);

  lua_create_table(LUA);
  file_r = lua_reference_create(LUA);

  lua_reference_push(LUA, file_r);
  lua_push_string(LUA, "open", 0);
  lua_push_cfunc(LUA, l_file_new);
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

GMOD_MODULE_CLOSE() {
  return 0;
}
