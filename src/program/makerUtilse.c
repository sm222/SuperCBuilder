#include "MakeUtilse.h"

const size_t bSize = 9999;

size_t output(int fd, const char* s, ...) {
  char buffer[bSize + 1];
  va_list list;
  va_start(list, s);
  const int len = vsnprintf(buffer, bSize, s, list);
  if (len > 0)
    write(fd, buffer, len);
  va_end(list);
  return len;
}


#include <time.h>

size_t header(int fd, const char* comment, const char* uName, const char* pName, const char* fType) {
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  size_t out = 0;
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(fd, "%s %s Make whit scb on %s\n",comment, fType, asctime(timeinfo));
  out += output(fd, "%s build by %s\n", comment, uName);
  out += output(fd, "%s project name -> %s\n", comment, pName);
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  return out;
}


int readConfigFile(outFileData* file) {
  
  return 0;
}

//todo
int openOutFile(outFileData* file) {
  (void)file;
  return 0;
}