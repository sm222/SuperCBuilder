# include "MakerUtils.h"
# include "testFlags.h"
# include <ctype.h>


static ssize_t drawHeader(outFileData* data, const char* name) {
  ssize_t total = 0;
  total += output(data->fd, "\ncmake_minimum_required(VERSION 3.23)\n");
  const char* nameValue = name;
  if (isVarInConfig(Vname, data->var))
    nameValue = readVariableName(data, Vname);
  total += output(data->fd, "\nproject(%s)\n", name);
  total += output(data->fd, "\nadd_executable(%s)", nameValue);
  return total;
}

# define CMAKEFILENAME "CMakeLists.txt"

ssize_t buildCmake(outFileData* data) {
  ssize_t totalBytes = 0;
  const char* hardcodePname = strrchr(data->scb->originPath, FILE_SEP) + 1;
  if (!newFile(CMAKEFILENAME, data))
    return -1;
  const char* const user = getenv("USER");
  totalBytes += scbHeader(data, findCommentFromType(data->outputType), user, hardcodePname, "cmake");
  totalBytes += drawHeader(data, hardcodePname);
  //!totalBytes += drawCompiler(data);
  //!totalBytes += drawName(hardcodePname, data);
  //
  //!totalBytes += readList(&data->scb->node, data);
  //!totalBytes += drawObjectVar(data);
  //!totalBytes += drawMakeRule(data);
  //!totalBytes += drawEnd(data);
  close(data->fd);
  freeOutVar(&data->outVar);
  output(STDOUT_FILENO, "outfile -> %s/"CMAKEFILENAME"\n", data->scb->originPath);
  return totalBytes;
}