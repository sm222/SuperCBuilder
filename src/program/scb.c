#include "scb.h"


static int addLine(char* buff, int tab, const size_t max, bool colorMode) {
  int total = 0;
  int color = 91;
  static bool l = false;
  while (tab) {
    if (colorMode) {
      total += snprintf(buff + total, max - total, 
        CS"\e[%dm"CE"%c  |"RESET, color, l ? '\\' : '/');
    } else {
      total += snprintf(buff + total, max - total, "%c  |", l ? '\\' : '/');
    }
    color++;
    if (color == 97)
      color = 91;
    tab--;
  }
  l = !l;
  return total;
}

void _printfolder(t_node* list, int tab, bool colorMode) {
  const int size = 80;
  const size_t buffLen = size * 4;
  int currentSize = 0;
  char buff[buffLen];
  int len = 0;
  while (list) {
    if (currentSize == 0 || list->data.type == folder ) {
      len = addLine(buff, tab, buffLen, colorMode);
      len += snprintf(buff + len, buffLen - len, "%s ", list->data.name);
      currentSize += len;
    }
    else {
      len = snprintf(buff, buffLen, "%s ", list->data.name);
      currentSize += len;
    }
    write(STDOUT_FILENO, buff, len);
    if (list->data.type == folder || !list->next || currentSize >= size) {
      write(STDOUT_FILENO, "\n", 1);
      currentSize = 0;
    }
    if (list && list->child)
      _printfolder(list->child, tab + 1, colorMode);
    list = list->next;
  }
  if (list)
    list = list->next;
}

void printfolder(t_node* list, bool colorMode) {
  _printfolder(list, 0, colorMode);
  write(STDOUT_FILENO, "\n", 1);
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

static bool testDotsFiles(const char* name) {
  return (strncmp(".", name, 2) == 0 || strncmp("..", name, 3) == 0);
}

int mapDir(const char* path, t_node** head, unsigned int maxDep) {
  if (isValidFolder(path) || maxDep == 0)
    return 1; // only care if error happen of first try
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
      stat(wd, &stats);
      const int type = S_ISDIR(stats.st_mode) ? folder : getFileType(de->d_name);
      if (testDotsFiles(de->d_name) || (type == unknown && type != folder) || isValidFolder(wd)) {
        continue ;
      } else {
        t_node* t = makeNodeLast(de->d_name, type, head);
        if (type == folder) {
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

static int grabAv(t_SCB* setting, int avSize, const char* path) {
  if (avSize > 1) {
    const char* fileName = av_read(&setting->mainData->avNoFlags, 1);
    if (access(fileName, F_OK | R_OK) != 0) {
      fprintf(stderr, "scb: test %s %s\n", fileName, strerror(errno));
      return 1;
    }
  }
  return 0;
}

static int setup(t_SCB* setting, void* mainData) {
  bzero(setting, sizeof(*setting));
  setting->mainData = mainData;
  getcwd(setting->originPath, PATH_MAX);
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
  int err = grabAv(setting, avNb, path);
  getcwd(setting->path, PATH_MAX);
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

# include <ctype.h>

int superStrcmp(const char* s1, const char* s2, size_t n) {
  if(!s1 || !s2 || n == 0)
    return -1;
  while (*s1 && *s2 && tolower(*s1) == tolower(*s2) && --n) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

static int getBuildType(t_SCB* scb) {
  if (!scb->buildType)
    return 0;
  for (int i = 0; buildFileLanguage[i]; i++) {
    const size_t len = getLenOfBuild(buildFileLanguage[i]);
    if (superStrcmp(scb->buildType, buildFileLanguage[i], len) == 0 \
    && scb->buildType[len] == '\0') {
      return i;
    }
  }
  return 111;
}


#include "testFlags.h"

int scb(void* data) {
  t_SCB  SCB;
  if (setup(&SCB, data)) {
    return 1;
  }
  //
  int maxDep = 60;
  SCB.error = mapDir(SCB.path, &SCB.node, maxDep);
  chdir(SCB.originPath);
  if (!SCB.error) {
    moveFolderUp(&SCB.node);
    deledEmty(&SCB.node);
    //! add flag for visual
    printfolder(SCB.node, read_byte(SCB.mainData->flags, flags_color));
    outFileData data = makerSetup(&SCB, getBuildType(&SCB));
    SCB.error = makerStart(&data, av_read(&SCB.mainData->avNoFlags, 1));
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