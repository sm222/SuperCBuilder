#include "../dataType.h"
# include "../utils.h"

#include "testFlags.h"

const char* const helpList[] = {
  "--help             -h  print this message",
  "--color            -c  color",                             // 4
  "--detach           -d  detach path from root",             // 8
  "--buildtype=[type]     set the build type to value",       // 16
  "--target=[system]      linux, windows, mac, etc",          // 32
  "--info             -i  print all valid flags and keywords",// 64
  0x0
};

static void printHelp(void) {
  for (size_t i = 0; helpList[i]; i++) {
    put_str(helpList[i], STDOUT_FILENO, true);
  }
}

# include "MakerUtils.h"

static char* const line = \
"================================================";

static void printKeyword(void) {
  size_t len = 0;
  const int maxlen = 10;
  printf("%s\nreserve Vars\n= = = = = = =\n", line);
  for (size_t i = 0; reservedVarNames[i]; i++) {
    const char* l = "          ";
    const int nlen = strlen(reservedVarNames[i]);
    if (len >= 40) {
      len = 0;
      l = "\n";
    }
    len += printf("%s%.*s", reservedVarNames[i], maxlen - nlen, l);
  }
  printf("\n%s\nKeyword\n= = = = = = =\n", line);
  for (size_t i = 0; keyWords[i]; i++) {
    const char* l = "          ";
    const int nlen = strlen(keyWords[i]);
    if (len >= 40) {
      len = 0;
      l = "\n";
    }
    len += printf("%s%.*s", keyWords[i], maxlen - nlen, l);
  }
  printf("\n%s\n", line);
}

static int readValueSingle(t_settings* settings, int c) {
  if (c == 'c') { set_byte(&settings->flags, flags_color, true); }
  else if (c == 'h') {
    printHelp();
    return 1;
  }
  else if (c == 'd') { set_byte(&settings->flags, flags_detach, true); }
  else if (c == 'i') {
    printKeyword();
    return 1;
  }
  else {
    put_str_error(settings, RED,
      "scb %c: is unknow flag, call -h or --help to see the option", c);
    return 1;
  }
  return 0;
}

int   testSingle(void* data) {
  t_settings* castData = data;
  castData->jump = 1;
  if (!castData->av[castData->current][1]) {
    put_str_error(castData, RED, "scb: flag was call with not params");
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
    put_str_error(castData, RED, "csb: flag was call with not params");
    return 2;
  }
  int error = 0;
  const char* value = castData->av[castData->current] + 2;
  if (strncmp("detach", value, strlen("detach") + 1) == 0) {
    set_byte(&castData->flags, flags_detach, true);
  }
  else if (strncmp("help", value, 5) == 0) {
    printHelp();
    return 1;
  }
  else if (strncmp("buildtype=", value, 10) == 0) {
    set_byte(&castData->flags, flags_set_type, true);
    error = fv_add_last(&castData->flagValue, flags_set_type, value + 10);
    error = error < 0 ? error * -1 : error;
  }
  else if (strncmp("target=", value, 7) == 0) {
    set_byte(&castData->flags, flags_target, true);
    fv_add_last(&castData->flagValue, flags_target, value + 7);
  } else {
    error = 1;
    fprintf(stderr, "scb: unknow flag %s\n", castData->av[castData->current]);
  }
  return error;
}