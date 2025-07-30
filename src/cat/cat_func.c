#include "cat_func.h"

#include <getopt.h>
#include <stdio.h>

#include "flagstruct.h"

void parcer(int argc, char** argv, flag* flags) {
  int opt = 0;
  char* short_options = "+beEnstTv";

  struct option long_options[] = {
      {"number-nonblank", no_argument, &flags->b, 1},
      {"number", no_argument, &flags->n, 1},
      {"squeeze-blank", no_argument, &flags->s, 1},
      {NULL, 0, NULL, 0}};

  while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'n':
        flags->n = 1;
        break;

      case 'b':
        flags->b = 1;
        break;

      case 's':
        flags->s = 1;
        break;

      case 'v':
        flags->v = 1;
        break;

      case 'e':
        flags->e = 1;
        flags->v = 1;
        break;

      case 'E':
        flags->e = 1;
        break;

      case 't':
        flags->t = 1;
        flags->v = 1;
        break;

      case 'T':
        flags->t = 1;
        break;

      case '?':
        printf("Error!");
        break;
    }
  }
}

void file_opening(flag flags) {
  FILE* fileName = fopen(flags.file_name, "r");
  if (fileName == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", flags.file_name);
  } else {
    char k, prevk = '\n';
    int isLineEmpty = 0, counterLine = 1;
    while ((k = fgetc(fileName)) != EOF) {
      if (flags.s == 1) {
        if (k == '\n') {
          if (isLineEmpty) continue;
        } else
          isLineEmpty = 0;
      }
      if (flags.n == 1 && flags.b != 1) {
        if (prevk == '\n') printf("%6d\t", counterLine++);
      }
      if (flags.b == 1) {
        if (k != '\n' && prevk == '\n') printf("%6d\t", counterLine++);
      }
      int flagv = 0;
      if (flags.v == 1) {
        if (k >= 0 && k < 32 && k != 9 && k != 10) {
          flagv = 1;
          printf("^%c", k + 64);
        } else if (k == 127) {
          flagv = 1;
          printf("^%c", k - 64);
        }
        //  else if (k > 127 && k < 160)
        //  printf("M-^%c", k - 64);
      }
      if (flags.e == 1)
        if (k == '\n') putc('$', stdout);
      if (flags.t == 1) {
        if (k == '\t') {
          k = k + 64;
          putc('^', stdout);
        }
      }
      isLineEmpty = (prevk == '\n' && k == '\n') ? 1 : 0;
      if (flags.v && !flagv)
        fprintf(stdout, "%c", k);
      else if (!flags.v)
        fprintf(stdout, "%c", k);
      prevk = k;
    }
    fclose(fileName);
  }
}