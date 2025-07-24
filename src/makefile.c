# include "program.h"

static void header(t_setting* setting) {
  
}

int build(t_setting* setting, t_FilesList* root) {
  int error = 0;
  if (!root) {
    fprintf(stderr, "scb: no fille provided\n");
    return ++error;
  }

  return error;
}