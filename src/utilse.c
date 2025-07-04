# include "dataType.h"
#include "flags.h"

size_t getArrayLen(const char* const* array) {
  size_t i = 0;
  if (!array)
    return 0;
  while (array[i]) { i++; }
  return i;
}

int  int_strlen(const char* s) {
  int i = 0;
  if (!s)
    return -1;
  while (s[i])
    i++;
  return i;
}

void  help(int i[2]) {
  if (i[0] != 0) {
    if (i[0] == 1) {
      fprintf(stderr, "%s\n", singleFlags[i[1]]);
    }
    if (i[0] == 2) {
      fprintf(stderr, "%s\n", verboseFlags[i[1]]);
    }
    return ;
  }
  fprintf(stderr, "\n");
  size_t single = getArrayLen(singleFlags);
  for (size_t i = 0; i < single; i++) {
    fprintf(stderr, "%s\n", singleFlags[i]);
  }
  for (int i = 0; i < 2; i++) { fprintf(stderr, "\n"); }
  size_t Verbose = getArrayLen(verboseFlags);
  for (size_t i = 0; i < Verbose; i++) {
    fprintf(stderr, "%s\n", verboseFlags[i]);
  }
  fprintf(stderr, "\n");
}