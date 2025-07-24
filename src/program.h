#ifndef  PROGRAM_H
# define PROGRAM_H

# include "dataType.h"
# include "utilse.h"

// add lib type for unix and windows

enum {
  unknown = -1,
  folder,
  cFile,
  hFile,
  cppFile,
  hppFile,
  tppFile,
};

static const char* const fileType[] = {
  "unknown",
  "folder",
  ".c",
  ".h",
  ".cpp",
  ".hpp",
  ".tpp",
  0x0,
};

static const char* const ignoreFolder[] = {
  "/.git",
  "/.vscode",
  0x0,
};

typedef struct {
  t_setting*  setting;
  int         maxDep;
  char**      ignoreFolders;     // /abc
  char**      ignoreFiles;       //  hello.c
  char**      ignoreFilesTypes;  // .txt
} programParam;

//  node
/*

*/
int          program(t_setting* setting);

#endif