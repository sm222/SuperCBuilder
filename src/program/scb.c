#include "scb.h"
#include "testFlags.h"
#include "MakerUtils.h"
#include <sys/ioctl.h>
#include <assert.h>


struct winsize windowData;
bool           OnStdout = false;

size_t getLongerName(t_node* node) {
  size_t longest = 0;
  if (!node)
    return 1;
  while (node) {
    if (node->data.nameLen > longest)
      longest = node->data.nameLen;
    node = node->next;
  }
  return longest;
}

size_t addLine(int tab, const int* colorMode, int mode) {
  static bool l = false;
  bool        wasTab = false;
  const int size = 9999;
  char line[size + 1];
  int color = 31 + (mode ? 10: 0);
  int tabC = tab;
  while (tabC--) {
    color++;
    if (color == 37 + (mode ? 10: 0))
    color = 31 + (mode ? 10: 0);
  }
  char c = mode ? '>' : '#';
  size_t total = 0;
  if (tab && *colorMode && OnStdout) {
    total += snprintf(line + total, size - total, "\e[%dm", color);
    wasTab = true;
  }
  while (tab--) {
    l = !l;
    total += snprintf(line + total, size - total, "%c%c", l ? c : ' ', !l ? c : ' ');
  }
  if (wasTab) { total += snprintf(line + total, size - total, "\e[0m"); }
  write(STDOUT_FILENO, line, total);
  # ifdef DEBUG_COLOR_TRUE
  assert(*colorMode == 1);
  # endif
  return total;
}

size_t putSpace(char* space, size_t max, t_node* const n, const int* colorMode) {
  size_t l = 0;
  const int size = (max - n->data.nameLen) - (colorMode ? 1 : 0);
  space[size] = 0;
  l += put_str(space, STDOUT_FILENO, false);
  space[size] = ' ';
  return l;
}

t_node* printFiles(t_node* node, int tab, const int* colorMode) {
  if (!node)
    return NULL;
  const int with = OnStdout ? (windowData.ws_col / 1.8f) : 180;
  const size_t space_len = getLongerName(node) + 2;
  char  space[space_len + 1];
  memset(space, ' ', space_len);
  space[space_len + 1] = 0;
  while (node) {
    int total = 0;
    addLine(tab, colorMode, false);
    while (node && total < with) {
      if (*colorMode) {
        char cb[MAX_COLORLEN * 2 + 2];
        sprintf(cb, "\e[4%dm \e[0m", node->data.type);
        put_str(cb, STDOUT_FILENO, false);
      }
      total += put_str(node->data.name, STDOUT_FILENO, false);
      total += putSpace(space, space_len, node, colorMode);
      node = node->next;
    }
    printNl(STDOUT_FILENO);
  }
  return NULL;
}

static void _printfolder(t_node* list, int tab, const int* colorMode) {
  while (list && IS_FOLDER(list)) {
    addLine(tab, colorMode, true);
    put_str(list->data.name, STDOUT_FILENO, true);
    if (list && list->child)
      _printfolder(list->child, tab + 1, colorMode);
    list = list->next;
  }
  printFiles(list, tab, colorMode);
}


void printfolder(t_node* list, const int colorMode) {
  bzero(&windowData, sizeof(windowData));
  if (isatty(STDOUT_FILENO) == 1) {
    OnStdout = true;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowData);
  }
  const int C_colormode = colorMode;
  _printfolder(list, 0, &C_colormode);
  printNl(STDOUT_FILENO);
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
        fprintf(stderr, "%s is ignore\n", de->d_name);
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
    if (!makerStart(&Outdata))
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