#include "file.h"

int _create_directory(const char *dirname);

#ifdef _WIN32
extern char *getcwd(char *buf, int size);
extern int mkdir(const char *dirname);
extern int access(const char *name, int mode);
extern char *strrchr(const char *str, int needle);

#define F_OK 0

int _create_directory(const char *dirname) { return mkdir(dirname); }
#else
int _create_directory(const char *dirname) { return mkdir(dirname, 0777); }
#endif

int create_folders_recursive(const char *fn) {
  char *buf;
  size_t i, len = strlen(fn);

  buf = (char *)malloc(len + 1);

  if (!buf)
    return 0;

  for (i = 0; i < len; i++) {
    buf[i] = fn[i];

    if (fn[i] == '/') {
      buf[i + 1] = 0;
      if (access(buf, F_OK) == -1) {
        if (_create_directory(buf) != 0) {
          free(buf);
          return 0;
        }
      }
    }
  }

  free(buf);

  if (access(fn, F_OK) == 0)
    return 1;

  return _create_directory(fn) == 0 ? 1 : 0;
}

int find_last_occurence(const char *str, const char ch) {
  char *chr = strrchr(str, ch);

  if (chr != NULL)
    return chr - str;
  return -1;
}

int setup_directory_for_write(const char *fn) {
  char *target_name;
  int result, pos = find_last_occurence(fn, '/');

  if (pos == -1)
    return 0;

  target_name = (char *)malloc(pos + 2);

  if (!target_name)
    return 0;

  memcpy(target_name, fn, pos + 1);
  target_name[pos + 1] = 0;

  result = create_folders_recursive(target_name);
  free(target_name);

  return result;
}

char *concat(const char *s1, const char *s2) {
  char *result = malloc(strlen(s1) + strlen(s2) + 1);

  if (!result)
    return NULL;

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

int check_mode(const char *mode) {
  int i;

  switch (mode[0]) {
  case 'r':
  case 'w':
  case 'a':
    break;
  default:
    return 0;
  }

  for (i = 1; mode[i] != 0; i++) {
    if (i > 2 || (mode[i] != 'b' && mode[i] != '+'))
      return 0;
  }

  return 1;
}

/* Returns the full path of a file inside garrysmod/, to be freed by the
   caller, or NULL if the name is not allowed. */
char *setup_directory(const char *filename) {
  char current_folder[4096], *folder, *result;

  if (!check_filename(filename))
    return NULL;

  if (getcwd(current_folder, sizeof(current_folder)) == NULL)
    return NULL;

  folder = concat(current_folder, "/garrysmod/");

  if (!folder)
    return NULL;

  result = concat(folder, filename);
  free(folder);

  return result;
}

FILE *file_open(const char *filename, const char *mode) {
  FILE *f;
  char *fn;

  if (!check_mode(mode))
    return NULL;

  fn = setup_directory(filename);

  if (!fn)
    return NULL;

  f = fopen(fn, mode);
  free(fn);

  return f;
}

int file_handle_read(FILE *f, char **out, size_t *out_len) {
  long pos, end;
  char *buf;

  pos = ftell(f);

  if (pos < 0 || fseek(f, 0L, SEEK_END) != 0)
    return 0;

  end = ftell(f);
  fseek(f, pos, SEEK_SET);

  if (end < pos)
    return 0;

  buf = (char *)malloc(end - pos + 1);

  if (!buf)
    return 0;

  *out_len = fread(buf, 1, end - pos, f);
  buf[*out_len] = '\0';

  *out = buf;

  return 1;
}

int file_handle_write(FILE *f, const void *data, size_t len) {
  /* A stream opened for update needs a seek between reading and writing. */
  fseek(f, 0L, SEEK_CUR);

  return fwrite(data, 1, len, f) == len ? 1 : 0;
}

int file_write(const char *filename, void *data, size_t len) {
  FILE *f;
  char *fn = setup_directory(filename);

  if (!fn)
    return 0;

  if (!setup_directory_for_write(fn)) {
    free(fn);
    return 0;
  }

  f = fopen(fn, "wb");
  free(fn);

  if (f == NULL) {
    return 0;
  }

  fwrite(data, 1, len, f);
  fclose(f);

  return 1;
}

int file_append(const char *filename, void *data, size_t len) {
  FILE *f = file_open(filename, "ab");

  if (f == NULL) {
    return 0;
  }

  fseek(f, 0, SEEK_END);
  fwrite(data, 1, len, f);
  fclose(f);

  return 1;
}

int file_read(const char *filename, char **out) {
  FILE *f = file_open(filename, "rb");
  int len;
  char *buf;

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
  char *fn = setup_directory(filename);
  int result;

  if (!fn)
    return 0;

  result = remove(fn) == 0 ? 1 : 0;
  free(fn);

  return result;
}

int file_mkdir(const char *dirname) {
  char *fn = setup_directory(dirname);
  int result;

  if (!fn)
    return 0;

  result = access(fn, F_OK) == 0 ? 0 : create_folders_recursive(fn);
  free(fn);

  return result;
}
