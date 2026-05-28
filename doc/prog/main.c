#include <stdio.h>
# include "foo.h"
# include "bar/bar.h"

static void helloTo(const char* name) {
  printf("hello %s!\n", name);
}

int main(void) {
  printf("Hello wold!\n");
  helloTo(foo());
  helloTo(bar());
}