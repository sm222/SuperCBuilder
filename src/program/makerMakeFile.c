# include "MakerUtilse.h"

# include <ctype.h>

char* capName(const char* name) {
  static char newName[MAXPATHLEN + 20];
  size_t len = strlen(name);
  bzero(newName, MAXPATHLEN + 20);
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
  return output(fd, "\t$(F_%s)%s%s\n", name, n->data.name, n->next ? "\\" : "");
}

//todo draw file in a object var

void putAllFiles(t_node* tmp, const char* from, const int fd) {
  output(fd, "O_%s\t=\t\\\n", from);
  while (tmp) {
  if (tmp->data.type % 2 == 0 && tmp->data.type != folder)
    drawFile(tmp, from, fd);
  tmp = tmp->next;
  }
  output(fd, "\n");
}

static void  buidFileAndFolder(t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  char folderName[MAXPATHLEN];
  bzero(folderName, MAXPATHLEN);
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
      putAllFiles(tmp, from, *fd);
      return;
    }
    tmp = tmp->next;
  }
}


void printRoot(const char* root, outFileData* data) {
  output(data->fd, "F_%s\t\t=\t\t%s\n", root, data->workingDirectory);
}

static ssize_t readList(t_node** head, outFileData* data) {
  const int fd = data->fd;
  const char* truckPath = strrchr(data->workingDirectory, '/');
  //! switch that line for windows or other case that path don't hold a /
  printRoot(truckPath + 1, data);
  buidFileAndFolder(head, truckPath + 1, &fd);
  return 0;
}

ssize_t drawName(const char* name, const int fd) {
  return output(fd, "NAME\t\t=\t\t%s\n\n", name);
}

static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  total += output(data->fd, "\n\rDEBUG\t\t\t=\t\t-g\n\n", data->cCompiler);
  total += output(data->fd, "CFLAGS\t\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n", data->cCompiler);
  total += output(data->fd, "CXXFLAGS\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n\n\n", data->cCompiler);
  return total;
}

ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  const char* hardcodeName = "sm222";//! change later
  const char* hardcodePname = "scb";//!
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data->fd, "#", hardcodeName, hardcodePname, "Makefile");
  totalBytes += drawCompiler(data);
  totalBytes += drawName(hardcodePname, data->fd);
  readList(&data->header, data);
  closeFile(data);
  return totalBytes;
}