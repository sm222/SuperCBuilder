#ifndef  __TESTFLAGS__
# define __TESTFLAGS__

enum {
  flags_color    = 4,
  flags_detach   = 8,
  flags_set_type = 16,
  flags_target   = 32,
  flags_info     = 64,
  flags_6        = 128,
  flags_7        = 256,
  flags_8        = 512,
  flags_9        = 1024,
  flags_10       = 2048,
};


int   testSingle(void* data);
int   testDouble(void *data);

#endif
