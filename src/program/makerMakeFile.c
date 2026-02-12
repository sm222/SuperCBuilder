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

static ssize_t drawVarName(const char* name, const char* from, const int* fd) {
  if (from)
    return output(*fd, "F_%s\t\t=\t\t$(F_%s)%s/\n\n", capName(name), from ,name);
  return output(*fd, "F_%s\t\t=\t\t%s/\n\n", capName(name) ,name);
}

ssize_t drawFile(t_node* n, const char* name, const int fd) {
  return output(fd, "\t$(F_%s)%s%s\n", name, n->data.name, n->next ? "\\" : "");
}

//todo draw file in a object var

void putAllFiles(outFileData* data, t_node* tmp, const char* from, const int fd) {
  makeOutVarLast(from, &data->outVar);
  output(fd, "O_%s\t=\t\\\n", from);
  while (tmp) {
  if (IS_C_CPP(tmp) && !IS_FOLDER(tmp))
    drawFile(tmp, from, fd);
  tmp = tmp->next;
  }
  output(fd, "\n");
}

static void  buidFileAndFolder(outFileData* data, t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  char folderName[MAXPATHLEN];
  bzero(folderName, MAXPATHLEN);
  if (tmp && IS_FOLDER(tmp)) {
    memcpy(folderName, capName(tmp->data.name), strlen(tmp->data.name) + 1);
  }
  while (tmp) {
    if (IS_FOLDER(tmp)) {
      drawVarName(tmp->data.name, from ,fd);
      buidFileAndFolder(data, &tmp->child, folderName, fd);
      write(*fd, "\n", 1);
    }
    else if (IS_FILE(tmp)) {
      putAllFiles(data, tmp, from, *fd);
      return;
    }
    tmp = tmp->next;
  }
}


void printRoot(const char* root, outFileData* data) {
  output(data->fd, "F_%s\t\t=\t\t%s/\n", root, data->workingDirectory);
}

static ssize_t readList(t_node** head, outFileData* data) {
  const int fd = data->fd;
  const char* truckPath = strrchr(data->workingDirectory, '/');
  //! switch that line for windows or other case that path don't hold a /
  printRoot(truckPath + 1, data);
  //
  buidFileAndFolder(data ,head, truckPath + 1, &fd);
  return 0;
}

ssize_t drawName(const char* name, const int fd) {
  return output(fd, "NAME\t\t=\t\t%s\n\n", name);
}

static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  total += output(data->fd, "CC\t\t=\tcc\n");
  total += output(data->fd, "CXX\t\t=\tcc\n");
  total += output(data->fd, "\n\rDEBUG\t\t\t=\t\t-g\n\n", data->cCompiler);
  total += output(data->fd, "CFLAGS\t\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n", data->cCompiler);
  total += output(data->fd, "CCFLAGS\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n\n\n", data->cCompiler);
  return total;
}

static ssize_t drawObjectVar(outFileData* data) {
  output(data->fd, "SRCS_FILES\t\t=\t\\\n");
  t_outVar* tmp = data->outVar;
  while (tmp) {
    output(data->fd, "\t$(O_%s)\t\t\\\n", tmp->name);
    tmp = tmp->next;
  }
  output(data->fd, "\nOBJS	=	$(SRCS_FILES:.c=.o)\n\n");
  //SRCS_FILES
  //OBJS	=	$(SRCS_FILES:.cpp=.o)
  return 0;
}

static ssize_t drawMakeRule(outFileData* data) {
  output(data->fd, "all: ");
  //! add dependece
  output(data->fd, "$(NAME)\n\n");
  output(data->fd, "$(NAME): $(CC)\n\n");
  output(data->fd, "$(CC): $(OBJS)\n");
  output(data->fd, "\t$(CC) $(CFLAGS) $(OBJS)");
  //!add more var if needed
  output(data->fd, " -o $(NAME)\n\n");
  return 0;
}

static ssize_t drawEnd(outFileData* data) {
  output(data->fd, "clean:\n\t@rm -fv $(OBJS)\n\n");
  output(data->fd, "fclean: clean\n\t@rm -fv $(NAME)\n\n");
  output(data->fd, "re: fclean all\n\nPHONY:\n#END");
  return 0;
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
  drawObjectVar(data);
  drawMakeRule(data);
  drawEnd(data);
  freeOutVar(&data->outVar);
  closeFile(data);
  return totalBytes;
}