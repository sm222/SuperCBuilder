# include "MakerUtilse.h"

# include <ctype.h>

char* capName(const char* name) {
  static char newName[MAXPATHLEN + 20];
  size_t len = strlen(name);
  newName[len] = 0;
  while (len--) {
    newName[len] = (char)toupper(name[len]);
  }
  return newName;
}

static void drawVarName(const char* name, const char* from, const int* fd) {
  char outBuffer[MAXPATHLEN + 20];
  if (from)
    sprintf(outBuffer, "F_%s\t\t=\t\t$(F_%s)%s/\n\n", capName(name), from ,name);
  else
    sprintf(outBuffer, "F_%s\t\t=\t\t%s/\n\n", capName(name) ,name);
  write(*fd, outBuffer, strlen(outBuffer));
}

ssize_t drawFile(t_node* n, const char* name, const int fd) {
  char outBuffer[MAXPATHLEN + 20];
  sprintf(outBuffer, "\t\t$(%s)%s\\\n", name, n->data.name);
  return write(fd, outBuffer, strlen(outBuffer));
}

//todo draw file in a object var

static void  buidFileAndFolder(t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  char folderName[MAXPATHLEN];
  bzero(folderName, MAXPATHLEN);
  memcpy(folderName, capName("root_folder"), strlen("root_folder") + 1);
  if (tmp && tmp->data.type == folder) {
    memcpy(folderName, capName(tmp->data.name), strlen(tmp->data.name) + 1);
  }
  while (tmp) {
    if (tmp->data.type == folder) {
      drawVarName(tmp->data.name, from ,fd);
      buidFileAndFolder(&tmp->child, folderName, fd);
      write(*fd, "\n", 1);
    }
    else if (tmp->data.type % 2 == 0) {
      drawFile(tmp, from, *fd);
    }
    tmp = tmp->next;
  }
}

static ssize_t readList(t_node** head, outFileData* data) {
  const int fd = data->fd;
  buidFileAndFolder(head, NULL ,&fd);
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
  readList(&data->header, data);

  closeFile(data);
  return totalBytes;
}