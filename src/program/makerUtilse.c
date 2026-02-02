#include "MakeUtilse.h"

const size_t bSize = 9999;


# define NL '\n'
# define B_SIZE 101

#  define BUFFER_SIZE 100

typedef struct s_info {
  char*  tmp;
  char*  tmp2;
  char   readtmp[BUFFER_SIZE + 1];
  int    rv;
  size_t  cut;
}  t_info;

static void* ft_sfree(void* ptr) {
  free(ptr);
  return NULL;
}

static char* ft_strjoin(char* sfree, char* s2) {
  size_t s1_i = strlen(sfree ? sfree : "");
  size_t s2_i = strlen(s2 ? s2 : "");
  char* new = calloc(s1_i + s2_i + 1, sizeof(char));

  if (!new) {
    sfree = ft_sfree(sfree);
    return (NULL);
  }
  while (s1_i + s2_i-- > s1_i)
    new[s1_i + s2_i] = s2[s2_i];
  while (s1_i--)
    new[s1_i] = sfree[s1_i];
  if (*new == 0)
    new = ft_sfree(new);
  return (ft_sfree(sfree), new);
}

static char* ft_tiny_split(char* s, size_t* cut) {
  char* new = NULL;
  size_t  i = 0;

  while (s[i])
    if (s[i++] == '\n')
      break ;
  new = calloc(i + 1, sizeof(char));
  if (!new)
    return (new = ft_sfree(new));
  *cut = i;
  while (i--)
    new[i] = s[i];
  return (new);
}

static char  ft_find_nl(char* s) {
  size_t  i = 0;

  while (s && s[i]) {
    if (s[i] == '\n')
      return ('\n');
    i++;
  }
  return ('0');
}

static char* safe_return(char**  book, t_info* t_val) {
  t_val->tmp = ft_tiny_split(*book, &t_val->cut);
  if (!t_val->tmp) {
    *book = ft_sfree(*book);
    return (NULL);
  }
  t_val->tmp2 = *book;
  *book = ft_strjoin(NULL, *book + t_val->cut);
  t_val->tmp2 = ft_sfree(t_val->tmp2);
  return (t_val->tmp);
}

static char* get_next_line(int fd) {
  static char* book = NULL;
  t_info    t_val;

  if (fd < 0 || BUFFER_SIZE <= 0 || fd > 255)
    return (book = ft_sfree(book));
  if (!book)
    book = calloc(1, sizeof(char));
  if (!book)
    return (NULL);
  t_val.rv = 0;
  while (ft_find_nl(book) == '0') {
    bzero(t_val.readtmp, BUFFER_SIZE + 1);
    t_val.rv = read(fd, t_val.readtmp, BUFFER_SIZE);
    if (t_val.rv <= 0)
      break ;
    book = ft_strjoin(book, t_val.readtmp);
    if (!book)
      return (NULL);
  }
  if (t_val.rv == -1 || (t_val.rv <= 0 && *book == 0)) {
    book = ft_sfree(book);
    return (NULL);
  }
  return (safe_return(&book, &t_val));
}

size_t output(int fd, const char* s, ...) {
  char buffer[bSize + 1];
  va_list list;
  va_start(list, s);
  const int len = vsnprintf(buffer, bSize, s, list);
  if (len > 0)
    write(fd, buffer, len);
  va_end(list);
  return len;
}


#include <time.h>

size_t header(int fd, const char* comment, const char* uName, const char* pName, const char* fType) {
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  size_t out = 0;
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(fd, "%s %s Make whit scb on %s\n",comment, fType, asctime(timeinfo));
  out += output(fd, "%s build by %s\n", comment, uName);
  out += output(fd, "%s project name -> %s\n", comment, pName);
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  return out;
}

#include <ctype.h>

int super_strcmp(const char* s1, const char* s2) {
  if(!s1 || !s2)
    return -1;
  while (tolower(*s1) == tolower(*s2)) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int read_file(outFileData* file) {
  const size_t startSize = 10;
  file->var = calloc(startSize, sizeof(char*));
  size_t numberLine = 0;
  size_t rawFileSize = startSize;
  char* line = "line";
  while (line) {
    line = get_next_line(file->fd);
    file->var[numberLine] = line;
    file->varByte += strlen(line ? line : "");
    numberLine++;
    if (numberLine > rawFileSize / 2) {
      rawFileSize *= 2;
      file->var = realloc(file->var, sizeof(char*) * rawFileSize);
    }
  }
  file->varArray = numberLine;
  return 0;
}


/*

int readConfigFile(outFileData* file) {

  return 0;
}

//todo
int openOutFile(outFileData* file) {
  (void)file;
  return 0;
}
*/