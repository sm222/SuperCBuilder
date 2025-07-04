#ifndef  PARSING_H
# define PARSING_H

# include "dataType.h"

# ifndef PROG_NAME
#  define PROG_NAME "base"
# endif

int setFlag(const char* line, const size_t n, t_setting* programSetting);
int setVerboseFlag(const char* line, t_setting* programSetting);


#endif