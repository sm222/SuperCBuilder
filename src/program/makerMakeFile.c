# include "MakerUtilse.h"

static void drawVarName(const char* name) {
  size_t i = 0;
  while () {
  
  }
}

static void  buidFileAndFolder(t_node* head, const int* fd) {
  t_node* tmp = head;
  while (tmp) {
    if (tmp->data.type == folder) {
      drawVarName(tmp->data.name);
    }
  }
}

static ssize_t readList(t_node* head, outFileData* data) {
  const int fd = data->fd;
  buidFileAndFolder(head, &fd);
  return 0;
}

ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  const char* hardcodeName = "sm222";//! change later
  const char* hardcodePname = "scb" ;//!
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data->fd, "#", hardcodeName, hardcodePname, "Makefile");
  readList(data->header, data);
  closeFile(data);
  return totalBytes;
}