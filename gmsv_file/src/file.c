#include "file.h"

int _create_directory(const char *dirname);

#ifdef _WIN32
extern int getcwd(char *buf, size_t size);
extern int mkdir(const char *dirname);
extern int access(const char *name, int mode);
extern char *strrchr(const char *str, int needle);

#define F_OK 0

int _create_directory(const char *dirname) { return mkdir(dirname); }
#else
int _create_directory(const char *dirname) {
  struct stat st = {0};
  if (stat(dirname, &st) == -1)
    return mkdir(dirname, 0777);
}
#endif

int create_folders_recursive(const char *fn) {
  char buf[64];
  int i, p = 0;

  for (i = 0; i < strlen(fn); i++) {
    buf[p] = fn[i];
    p++;

    switch (fn[i]) {
    case '/':
    case 0:
      buf[p] = 0;
      if (access(buf, F_OK) == -1) {
        if (_create_directory(buf) != 0) {
          return 0;
        }
      }
    }
  }

  return _create_directory(fn) == 0 ? 1 : 0;
}

int find_last_occurence(const char *str, const char ch) {
  char *chr = strrchr(str, ch);

  if (chr != NULL)
    return chr - str;
  return -1;
}

int setup_directory_for_write(const char *fn) {
  char target_name[256];
  int i, pos = find_last_occurence(fn, '/');

  if (pos != -1) {
    for (i = 0; i <= pos; i++) {
      target_name[i] = fn[i];
    }

    target_name[pos + 1] = 0;

    return create_folders_recursive(target_name);
  }

  return 0;
}

char *concat(const char *s1, const char *s2) {
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

int check_filename(const char *fn) {
  int i;
  char prev = 0;

  switch (fn[0]) {
  case '/':
  case '~':
    return 0;
  }

  for (i = 0; i < strlen(fn); i++) {
    switch (fn[i]) {
    case '\b':
    case '\r':
    case ':':
    case '\\':
    case '$':
    case '~':
    case '%':
      return 0;
    };

    if (fn[i] == '.' && prev == '.')
      return 0;

    prev = fn[i];
  }

  return 1;
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
  char *fn;

  if (!check_filename(filename)) return 0;

  fn = (char *)filename;
  setup_directory(&fn);

  if (!setup_directory_for_write(fn))
    return 0;

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
  char *fn;

  if (!check_filename(filename)) return 0;

  fn = (char *)filename;
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
  char *buf, *fn;

  if (!check_filename(filename)) return 0;

  fn = (char *)filename;
  setup_directory(&fn);

  f = fopen(fn, "rb");

  if (f == NULL) {
    return 0;
  }

  fseek(f, 0L, SEEK_END);
  len = ftell(f);
  fseek(f, 0L, SEEK_SET);
  buf = (char *)malloc(len + 1);

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
  char *fn = (char *)filename;

  if (!check_filename(filename)) return 0;

  setup_directory(&fn);

  return remove(fn) == 0 ? 1 : 0;
}

int file_mkdir(const char *dirname) {
  char *fn = (char *)dirname;

  if (!check_filename(dirname)) return 0;

  setup_directory(&fn);

  return create_folders_recursive(fn);
}
