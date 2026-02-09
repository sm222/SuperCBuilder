#ifndef  MAKE_UTISE_H
# define MAKE_UTISE_H

# include "node.h"
# include "../dataType.h"
# include "../utilse.h"
# include "fileType.h"
# include <fcntl.h>
# include "scb.h"
# include <stdarg.h>


static const char* const buildFileLanguage[] = {
  "Makefile:#",
  0x0,
};

typedef struct {
  int     fd;
  int     configFd;
  char*   filename;
  char**  var;
  size_t  varByte;
  size_t  varArray;
  t_node* header;
  int     outputType;
} outFileData;

outFileData  makerSetup(t_SCB* in, int mode);
void         makerStart(outFileData* data);

size_t      output(int fd, const char* s, ...);
size_t      header(int fd, const char* comment, const char* uName, const char* pName, const char* fType);

bool        newFile(char* name, outFileData* data);
void        closeFile(outFileData* data);

#endif