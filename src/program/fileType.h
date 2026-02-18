#ifndef  FILE_TYPE_H
# define FILE_TYPE_H

typedef enum {
  unknown    = -1,
  folder     = 0,
  cFile      = 2,
  hFile      = 3,
  cppFile    = 4,
  hppFile    = 5,
  tppFile    = 6,
  configFile = 7,
} t_file_type;


static const char* const fileType[] = {
  "unknown",
  "folder",
  ".c",
  ".h",
  ".cpp",
  ".hpp",
  ".tpp",
  ".scb",
  0x0,
};

static const char* const DefaultIgnoreFolder[] = {
  "/.git",
  "/.vscode",
  0x0,
};

#endif