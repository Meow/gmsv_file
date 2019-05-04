#include "file.h"

int _create_directory(const char *dirname);

#ifdef _WIN32
extern int getcwd(char *buf, size_t size);
extern int mkdir(const char *dirname);

int _create_directory(const char *dirname) {
  return mkdir(dirname);
}
#else
int _create_directory(const char *dirname) {
  struct stat st = {0};
  if (stat(dirname, &st) == -1)
    return mkdir(dirname, 0777);
}
#endif

char* concat(const char *s1, const char *s2) {
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

void setup_directory(char **directory) {
  char current_folder[256], *result;
  getcwd(current_folder, sizeof(current_folder));
  result = concat(current_folder, "/garrysmod/");
  *directory = concat(result, *directory);
  free(result);
}

int file_write(const char *filename, void *data, size_t len) {
  FILE *f;
  char *fn = (char*)filename;

  setup_directory(&fn);

  f = fopen(fn, "wb");

  if (f == NULL) {
    return 0;
  }

  fwrite(data, 1, len, f);
  fclose(f);

  return 1;
}

int file_append(const char *filename, void *data, size_t len) {
  FILE *f;
  char *fn = (char*)filename;

  setup_directory(&fn);

  f = fopen(fn, "ab");

  if (f == NULL) {
    return 0;
  }

  fseek(f, 0, SEEK_END);
  fwrite(data, 1, len, f);
  fclose(f);

  return 1;
}

int file_read(const char *filename, char **out) {
  FILE *f;
  int len;
  char *buf, *fn = (char*)filename;

  setup_directory(&fn);

  f = fopen(fn, "rb");

  if (f == NULL) {
    return 0;
  }

  fseek(f, 0L, SEEK_END);
  len = ftell(f);
  fseek(f, 0L, SEEK_SET);
  buf = (char*)malloc(len + 1);

  if (!buf) {
    fclose(f);
    return 0;
  }

  fread(buf, 1, len, f);
  fclose(f);

  buf[len] = '\0';

  *out = buf;

  return 1;
}

int file_delete(const char *filename) {
  char *fn = (char*)filename;
  setup_directory(&fn);

  return remove(fn) == 0 ? 1 : 0;
}

int file_mkdir(const char *dirname) {
  char *fn = (char*)dirname;
  setup_directory(&fn);

  return _create_directory(fn) == 0 ? 1 : 0;
}
