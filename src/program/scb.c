#include "scb.h"
#include "testFlags.h"
#include "MakerUtils.h"
#include <sys/ioctl.h>


struct winsize windowData;
bool           OnStdout = false;

static size_t getLongerName(t_node* node) {
  size_t longest = 0;
  if (!node)
    return 0;
  while (node) {
    if (node->data.nameLen > longest)
      longest = node->data.nameLen;
    node = node->next;
  }
  return longest;
}

size_t addLine(char* buff, int tab, bool colorMode, const size_t bufflen) {
  size_t total = 0;
  int color = 31;
  static bool l = false;
  int tabC = tab;
  while (tabC--) {
    color++;
    if (color == 37)
      color = 31;
  }
  while (tab > 0) {
    if (colorMode && OnStdout)
      total += snprintf(buff + total, bufflen - total,  CS"\e[%dm"CE"%c|%s", color, l ? '=' : '+', RESET);
    else
      total += snprintf(buff + total, bufflen - total, "%c|", l ? '=' : '+');
    tab--;
  }
  l = !l;
  return total;
}

t_node* printFiles(t_node* node, int tab, bool colorMode) {
  if (!node)
    return NULL;
  const size_t screen = (windowData.ws_col > 120 ? windowData.ws_col / 1.5 : windowData.ws_col);
  const size_t with = (OnStdout ? screen : 120) + 1;
  unsigned short items = 0;
  const size_t longest = getLongerName(node);
  while (node) {
    size_t lineLen = (tab * 4) * (colorMode ?  (MAX_COLORLEN * 2): 1);
    t_node* head = node;
    unsigned short text = (tab * 4);
    while (head && text + longest < with) {
      text += longest;
      items++;
      head = head->next;
    } 
    if (items == 0) { return NULL; }
    lineLen += text;
    char space[longest + 1];
    char buff[lineLen + 1];
    size_t off = addLine(buff, tab, colorMode, lineLen);
    memset(space, ' ', longest);
    space[longest] = 0;
    while (items--) {
      int size = longest - node->data.nameLen - 1;
      off += snprintf(buff + off, lineLen - off, "%s%.*s%c", \
        node->data.name, size, space, items ? ' ' : '\n');
      node = node->next;
    }
    put_str(buff, STDOUT_FILENO, false);
  }
  return NULL;
}

static void _printfolder(t_node* list, int tab, bool colorMode) {
  while (list && IS_FOLDER(list)) {
    size_t buffSize = list->data.nameLen + 3 + (tab * 4) * (colorMode ?  (MAX_COLORLEN * 2): 1);
    char buff[buffSize + 1];
    size_t off = addLine(buff, tab, colorMode, buffSize);
    snprintf(buff + off, buffSize - off, "*%s\n", list->data.name);
    put_str(buff, STDOUT_FILENO, false);
    if (list && list->child)
      _printfolder(list->child, tab + 1, colorMode);
  list = list->next;
  }
  printFiles(list, tab, colorMode);
}


void printfolder(t_node* list, bool colorMode) {
  if (isatty(STDOUT_FILENO) == 1) {
    OnStdout = true;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowData);
  }
  _printfolder(list, 0, colorMode);
  printNl(STDOUT_FILENO);
  fprintf(stderr, "%u %u\n", windowData.ws_col, windowData.ws_row);
}

static int testFolderList(const char* folder) {
  const size_t i = getArrayLen(DefaultIgnoreFolder);
  size_t j = 0;
  while (j < i) {
    if (strcmp(folder, DefaultIgnoreFolder[j]) == 0)
      return 1;
    j++;
  }
  return 0;
}


static int isValidFolder(const char* path) {
  if (path) {
    const char* dirTruck = strrchr(path, FILE_SEP);
    if (dirTruck && testFolderList(dirTruck)) {
      fprintf(stdout, "%s is ignore\n", dirTruck + 1);
      return 1;
    }
  }
  return 0;
}

static int getFileType(const char* fileName) {
  if (!fileName)
    return -1;
  const char* cut = strrchr(fileName, '.');
  if (!cut)
    return -1;
  for (int i = 0; fileType[i]; i++) {
    if (strcmp(fileType[i], cut) == 0) {
      return i;
    }
  }
  return -1;
}

static inline bool testDotsFiles(const char* name) {
  return (strncmp(".", name, 2) == 0 || strncmp("..", name, 3) == 0);
}

static const char* ignore = NULL;

//extractVar

int mapDir(const char* path, t_node** head, unsigned int maxDep) {
  /*only care if error happen of first try*/
  if (isValidFolder(path)) { return 1; }
  if (maxDep == 0) { return 2; }
  if (numberOfId() > MAX_NODE) { return 2;}
  struct dirent* de = NULL;
  DIR* dr = opendir(path);
  if (dr == NULL) {
    fprintf(stderr, "scb: can't open or read %s\n", path);
    return 1;
  }
  struct stat stats;
  char wd[PATH_MAX + 1];
  do {
    de = readdir(dr);
    if (de) {
      snprintf(wd, PATH_MAX, "%s/%s", path, de->d_name);
      if (lstat(wd, &stats) != 0) {
        perror(de->d_name);
        break ;
      }
      const int type = S_ISDIR(stats.st_mode) ? folder : getFileType(de->d_name);
      if (testDotsFiles(de->d_name) || \
      (type == unknown && type != folder) || isValidFolder(wd)) {
        continue ;
      }
      else if (ignore && testIsIgnore(de->d_name, ignore)) {
        fprintf(stdout, "%s is ignore\n", de->d_name);
        continue ;
      }
      else {
        t_node* t = makeNodeLast(de->d_name, type, head);
        if (!t) {
          perror("MapDir");
          break ;
        }
        if (IS_FOLDER(t)) {
          mapDir(wd, &t->child, --maxDep);
          t->data.fsize = getNodeLen(t->child);
        }
      }
    }
  } while (de != NULL);
  closedir(dr);
  return 0;
}


# include "MakerUtils.h"

static int grabAv(t_SCB* setting, int avSize) {
  if (avSize > 1) {
    const char* fileName = av_read(&setting->mainData->avNoFlags, 1);
    if (access(fileName, F_OK | R_OK) != 0) {
      fprintf(stderr, "scb: %s %s\n", fileName, strerror(errno));
      return 1;
    }
  }
  return 0;
}

static int setup(t_SCB* setting, void* mainData) {
  bzero(setting, sizeof(*setting));
  setting->mainData = mainData;
  if (getcwd(setting->originPath, PATH_MAX) != setting->originPath) {
    perror("getcwd");
    return 1;
  }
  const size_t avNb = av_len(&setting->mainData->avNoFlags);
  if (!avNb) {
    fprintf(stderr, "scb: no path given\n");
    return 1;
  } else if (avNb > 2) {
    fprintf(stderr, "scb: too manny args\n" \
      "\thow to use scb [flags] {path} [config file]\n");
    return 1;
  }
  const char* path = av_read(&setting->mainData->avNoFlags, 0);
  if (chdir(path)) {
    fprintf(stderr, "scb: invalid path: %s\n",path);
    return 1;
  }
  int err = grabAv(setting, avNb);
  if (getcwd(setting->path, PATH_MAX) != setting->path) {
    perror("getcwd");
    return 1;
  }
  return err;
}

static size_t getLenOfBuild(const char* const name) {
  size_t i = 0;
  while (name[i]) {
    if (name[i] == ':')
      break ;
    i++;
  }
  return i;
}


static int getBuildType(t_SCB* scb) {
  if (read_byte(scb->mainData->flags, flags_set_type)) {
    scb->buildType = fv_get_value(scb->mainData->flagValue, flags_set_type);
  }
  if (!scb->buildType)
    return 0;
  for (int i = 0; buildFileLanguage[i]; i++) {
    const size_t len = getLenOfBuild(buildFileLanguage[i]);
    if (superStrcmp(scb->buildType, buildFileLanguage[i], len) == 0 \
    && scb->buildType[len] == '\0') {
      return i;
    }
  }
  return notype;
}


#include "testFlags.h"

static int preOpenFile(outFileData* data, const char* fileName, int* maxDep) {
  int error = 0;
  data->configFile.name = (char*)fileName;
  if (openConfigFile(data, true))
    return 1;
  data->isOpen = true;
  error += checkIfFileValid(data);
  error += checkVar(data);
  if (error) {
    return error;
  }
  if (isVarInConfig(Ving, data->var)) {
    ignore = readVariableName(data, Ving);
  }
  //todo
  (void)maxDep;
  return 0;
}

int scb(void* data) {
  t_SCB  SCB;
  if (setup(&SCB, data)) {
    return 1;
  }
  int maxDep = 60;
  int buildTypde = getBuildType(&SCB);
  outFileData Outdata = makerSetup(&SCB, buildTypde);
  if (Outdata.target == unknown) {
    const char* const os = fv_get_value(SCB.mainData->flagValue, flags_target);
    fprintf(stderr, "scb: unknown target -> %s\n", os);
    SCB.error += 1;
  }
  //
  if (!SCB.error && av_len(&SCB.mainData->avNoFlags) == 2) {
    SCB.error += preOpenFile(&Outdata, av_read(&SCB.mainData->avNoFlags, 1), &maxDep);
  }
  if (buildTypde == notype) {
    fprintf(stderr, UNKNOWN_TYPE, SCB.buildType);
    SCB.error += 1;
  }
  if (!SCB.error) {
    SCB.error += mapDir(SCB.path, &SCB.node, maxDep);
    SCB.error += chdir(SCB.originPath);
  }
  if (!SCB.error) {
    //! add flag for visual
    moveFolderUp(&SCB.node);
    deledEmty(&SCB.node);
    printfolder(SCB.node, read_byte(SCB.mainData->flags, flags_color));
    if (!makerStart(&Outdata, av_read(&SCB.mainData->avNoFlags, 1)))
      SCB.error += runOutFile(&Outdata);
    else {
      SCB.error = 2;
    }
  }
  freeNode(&SCB.node);
  return SCB.error;
}

int setStart(void* in) {
  t_settings* ptr = in;
  ptr->programFt = &scb;
  ptr->ftsingle = testSingle;
  ptr->ftdouble = testDouble;
  return 0;
}