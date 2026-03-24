# include "makerBash.h"

static ssize_t drawVar(outFileData* data, int name) {
  const char* value = readVariableName(data, name);
  return output(data->fd, "%s=\"%s\"\n", reserveVarName[name], value);
}

static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  const char* l1 = "$CC\n";
  total += output(data->fd, "%s\n", l1);
  (void)data;
  return total;
}

static ssize_t drawNameAndStartVar(outFileData* data) {
  ssize_t total = 0;
  total += drawVar(data, Vcc);
  total += drawVar(data, Vcxx);
  total += printNl(data->fd);
  total += drawVar(data, Vname);
  total += drawVar(data, Vnamex);
  total += printNl(data->fd);
  total += drawVar(data, VCFLAGS);
  total += drawVar(data, VCXXFLAGS);
  total += printNl(data->fd);
  return total;
}

static ssize_t drawFile(outFileData* data) {
  ssize_t total = 0;
  t_node* tmp = data->scb->node;
  for (; tmp; tmp = tmp->next) {
    total += output(data->fd, "\"%s\"\n", tmp->data.name);
  }
  return total;
}

ssize_t buildBash(outFileData* data) {
  (void)data;
  ssize_t totalBytes = 0;
  if (!newFile("build.sh", data))
    return -1;
  const char* hardcodePname = strrchr(data->scb->originPath, FILE_SEP) + 1;
  totalBytes += output(data->fd, "#!/bin/bash\n\n");
  totalBytes += header(data, findCommentFromType(data->outputType), getenv("USER"), hardcodePname, "bash");
  //
  totalBytes += drawNameAndStartVar(data);
  totalBytes += drawFile(data);
  totalBytes += drawCompiler(data);
  //
  close(data->fd);
  return totalBytes;
}