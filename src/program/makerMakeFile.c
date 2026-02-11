# include "MakerUtilse.h"

# include <ctype.h>

static char lastF[MAXPATHLEN];

static void drawVarName(const char* name, const int* fd) {
  size_t i = 0;
  char outBuffer[MAXPATHLEN + 20];
  size_t rptr = 2;
  memcpy(outBuffer, "F_", 2);
  while (name[i]) {
    outBuffer[rptr] = (char)toupper(name[i]);
    i++;
    rptr++;
  }
  memcpy(lastF, outBuffer, rptr);
  sprintf(outBuffer + rptr, "\t\t=\t\t%s/\n", name);
  write(*fd, outBuffer, strlen(outBuffer));
}

ssize_t drawFile(char* name, const int fd) {
  char outBuffer[MAXPATHLEN + 20];
  sprintf(outBuffer, "\t\t$(%s)%s\\\n", lastF, name);
  return write(fd, outBuffer, strlen(outBuffer));
}

//todo draw file in a object var

static void  buidFileAndFolder(t_node* head, const int* fd) {
  t_node* tmp = head;
  while (tmp) {
    if (tmp->data.type == folder) {
      drawVarName(tmp->data.name, fd);
      buidFileAndFolder(tmp->child, fd);
    }
    else if (tmp->data.type % 2 == 0) {
      drawFile(tmp->data.name, *fd);
    }
    tmp = tmp->next;
  }
}

static ssize_t readList(t_node* head, outFileData* data) {
  const int fd = data->fd;
  buidFileAndFolder(head, &fd);
  return 0;
}

ssize_t drawName(const char* name, const int fd) {
  char buff[MAXPATHLEN + 20];
  memcpy(buff, "NAME\t\t=\t\t", 9);
  const size_t nlen = strlen(name);
  memcpy(buff + 9, name, nlen);
  buff[nlen + 9] = '\n';
  buff[nlen + 10] = '\n';
  buff[nlen + 11] = '\0';
  return write(fd, buff, nlen + 11);
}

ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  const char* hardcodeName = "sm222";//! change later
  const char* hardcodePname = "scb";//!
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data->fd, "#", hardcodeName, hardcodePname, "Makefile");
  totalBytes += drawName(hardcodePname, data->fd);
  readList(data->header, data);
  closeFile(data);
  return totalBytes;
}