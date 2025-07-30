#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "cat_func.h"
#include "flagstruct.h"

int main(int argc, char* argv[]) {
  flag flags = {0};
  parcer(argc, argv, &flags);

  for (int i = optind; i < argc; i++) {
    flags.file_name = argv[i];
    file_opening(flags);
  }
  return 0;
}