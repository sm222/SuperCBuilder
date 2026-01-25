#ifndef  PARSING_H
# define PARSING_H

# include "dataType.h"

# ifndef PROG_NAME
#  define PROG_NAME "base"
# endif



int env_parsing(t_setting* setting);
//
int parsing_get_single(t_setting* setting);
int parsing_get_double(t_setting* setting);

#endif