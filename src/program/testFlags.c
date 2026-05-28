#include "../dataType.h"
# include "../utils.h"

#include "testFlags.h"

const char* const helpList[] = {
  "--color  -c           color",                       // 1
  "--detach -d           detach path from root",       // 2
  "--buildtype=[type]    set the build type to value", // 4
  0x0
};

static void printHelp(void) {
  for (size_t i = 0; helpList[i]; i++) {
    fprintf(stdout, "%s \n", helpList[i]);
  }
}

static int readValueSingle(t_settings* settings, int c) {
  if (c == 'c') { set_byte(&settings->flags, flags_color, true); }
  else if (c == 'h') {
    printHelp();
    return 1;
  }
  else if (c == 'd') { set_byte(&settings->flags, flags_detach, true); }
  else {
    put_str_error(settings, RED,
      "%c: is unknow flag, call -h or --help to see the option\n", c);
    return 1;
  }
  return 0;
}

int   testSingle(void* data) {
  t_settings* castData = data;
  castData->jump = 1;
  if (!castData->av[castData->current][1]) {
    put_str_error(castData, RED, "flag was call with not params\n");
    return 2;
  }
  const size_t len = strlen(castData->av[castData->current]);
  int error = 0;
  for (size_t i = 1; i < len; i++) {
    error = readValueSingle(castData, castData->av[castData->current][i]);
    if (!read_byte(castData->flags, setting_continue_on_error) && error)
      break ;
  }
  return error;
}


///

int   testDouble(void* data) {
  t_settings* castData = data;
  castData->jump = 1;
  if (!castData->av[castData->current][2]) {
    put_str_error(castData, RED, "flag was call with not params\n");
    return 2;
  }
  int error = 0;
  const char* value = castData->av[castData->current] + 2;
  if (strncmp("detach", value, strlen("detach") + 1) == 0) {
    set_byte(&castData->flags, flags_detach, true);
  }
  else if (strncmp("buildtype=", value, strlen("buildtype=")) == 0) {
    set_byte(&castData->flags, flags_set_type, true);
    error = fv_add_last(&castData->flagValue, flags_set_type, value + 10);
    error = error < 0 ? error * -1 : error;
  } else {
    error = 1;
    fprintf(stderr, "scb: unknow flag %s\n", castData->av[castData->current]);
  }
  return error;
}