#ifndef GMSV_FILE_FILE_H
#define GMSV_FILE_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

int file_write(const char *filename, void *data, size_t len);
int file_append(const char *filename, void *data, size_t len);
int file_read(const char *filename, char **buffer);
int file_delete(const char *filename);
int file_mkdir(const char *dirname);

#endif