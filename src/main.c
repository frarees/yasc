#include <stdio.h>
#include "adr.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: yasc <file>\n");
    return 0;
  }

  ASin *x = asin_open(argv[1]);
  adr_programa(x);
  printf("Object: \n");
  code_get(x->code);
  printf("Output: \n");
  code_run(x->code);
  asin_close(x);
  return 0;
}
