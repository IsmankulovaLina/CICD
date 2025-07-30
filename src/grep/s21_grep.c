#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include "flagstruct.h"

int regex_comp(flag *flags, int argc, char **argv);
void regex_exec(regex_t preg, flag flags, int file_quantity);
int parcer(flag *flags, int argc, char **argv);
int flag_f(flag *flags);
void flag_e(flag *flags);
void default_pattern(flag *flags, char **argv);
void default_reader(flag flags, int file_quantity, int *counter,
                    int *match_counter, regex_t preg, char *line, int line_len);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "grep: option requires an argument -- e\nusage: grep "
            "[-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] [-C[num]]\n     "
            "   [-e pattern] [-f file] [--binary-files=value] [--color=when]\n "
            "       [--context[=num]] [--directories=action] [--label] "
            "[--line-buffered]\n        [--null] [pattern] [file ...]\n");
  } else {
    flag flags = {0};
    flags.pattern = (char *)calloc(1, sizeof(char));

    if (parcer(&flags, argc, argv) == 0) regex_comp(&flags, argc, argv);
    free(flags.pattern);
  }
  return 0;
}

int parcer(flag *flags, int argc, char **argv) {
  char *options_str = "e:ivclnhsf:o";
  int opt = 0, exit_flag = 0;

  while ((opt = getopt_long(argc, argv, options_str, 0, 0)) != -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        flag_e(flags);
        break;

      case 'i':
        flags->i = 1;
        break;

      case 'v':
        flags->v = 1;
        break;

      case 'c':
        flags->c = 1;
        break;

      case 'l':
        flags->l = 1;
        break;

      case 'n':
        flags->n = 1;
        break;

      case 'h':
        flags->h = 1;
        break;

      case 's':
        flags->s = 1;
        break;

      case 'f':
        flags->f = 1;
        if (flag_f(flags)) exit_flag = 1;
        break;

      case 'o':
        flags->o = 1;
        break;
    }
  }
  if (!flags->f && !flags->e) default_pattern(flags, argv);
  if (!exit_flag) {
    if (flags->pattern[strlen(flags->pattern) - 1] == '|') {
      flags->pattern[strlen(flags->pattern) - 1] = '\0';
    }
  }

  return exit_flag;
}

int regex_comp(flag *flags, int argc, char **argv) {
  regex_t preg;
  int file_quantity = argc - optind;
  int regcomp_res = 0;

  if (flags->i)
    regcomp_res = regcomp(&preg, flags->pattern, REG_EXTENDED | REG_ICASE);
  else
    regcomp_res = regcomp(&preg, flags->pattern, REG_EXTENDED);

  if (regcomp_res != 0) {
    // char errbuff[512];
    // regerror(regcomp_res, &preg, errbuff, sizeof(errbuff));
    // printf("%s", errbuff);
  } else {
    for (int i = optind; i < argc; i++) {
      flags->file_name = argv[i];
      regex_exec(preg, *flags, file_quantity);
    }
  }
  regfree(&preg);
  return 0;
}

void regex_exec(regex_t preg, flag flags, int file_quantity) {
  (void)flags;

  FILE *file_name = fopen(flags.file_name, "r");
  if (file_name == NULL) {
    if (!flags.s)
      fprintf(stderr, "grep: %s: No such file or directory\n", flags.file_name);
  } else {
    char *line = NULL;
    int line_len;
    size_t len = 0;
    int counter = 0;
    int match_counter = 0;

    while ((line_len = getline(&line, &len, file_name)) != -1) {
      counter++;
      default_reader(flags, file_quantity, &counter, &match_counter, preg, line,
                     line_len);

      regmatch_t pm[2];
      int regexec_res = regexec(&preg, line, 2, pm, 0);
      if (regexec_res != REG_NOMATCH && regexec_res != 0) {
        char regerrbuff[512];
        regerror(regexec_res, &preg, regerrbuff, sizeof(regerrbuff));
        if (!flags.s) {
          fprintf(stderr, "Regex match failed: %s\n", regerrbuff);
        }
      }
    }

    if (flags.c && !flags.l) {
      if (file_quantity > 1 && !flags.h) printf("%s:", flags.file_name);
      printf("%d\n", match_counter);
    }
    if (flags.l && match_counter > 0 && !flags.c)
      printf("%s\n", flags.file_name);
    if (flags.c && flags.l) {
      if (file_quantity > 1 && !flags.h) {
        printf("%s:", flags.file_name);
        if (match_counter > 0)
          printf("1\n%s\n", flags.file_name);
        else
          printf("0\n");
      } else {
        if (match_counter > 0)
          printf("1\n%s\n", flags.file_name);
        else
          printf("0\n");
      }
    }
    free(line);
    counter = 0, match_counter = 0;
    fclose(file_name);
  }
}

int flag_f(flag *flags) {
  int exit_flag = 0;

  FILE *f_file_name = fopen(optarg, "r");
  if (f_file_name == NULL) {
    exit_flag = 1;
    if (!flags->s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
    }
  } else {
    char *f_line = NULL;
    int f_line_len;
    size_t f_len = 0;
    int f_counter = 0;

    while ((f_line_len = getline(&f_line, &f_len, f_file_name)) != -1) {
      f_counter++;
      if (f_line[f_line_len - 1] == '\n' && f_line_len > 1) {
        f_line[f_line_len - 1] = '\0';
      }
      flags->pattern =
          realloc(flags->pattern, strlen(flags->pattern) + f_line_len + 2);
      strcat(flags->pattern, f_line);
      strcat(flags->pattern, "|");
    }
    if (f_counter == 0) {
      exit_flag = 1;
    }
    free(f_line);
    fclose(f_file_name);
  }
  return exit_flag;
}

void flag_e(flag *flags) {
  int pattern_len = 0;

  pattern_len = strlen(optarg);
  flags->pattern =
      (char *)realloc(flags->pattern, strlen(flags->pattern) + pattern_len + 2);
  strcat(flags->pattern, optarg);
  strcat(flags->pattern, "|");
}

void default_pattern(flag *flags, char *argv[]) {
  if (!flags->e && !flags->f) {
    int pattern_len = strlen(argv[optind]);
    flags->pattern = (char *)realloc(flags->pattern,
                                     strlen(flags->pattern) + pattern_len + 2);
    strcat(flags->pattern, argv[optind]);
    optind++;
  }
}

void default_reader(flag flags, int file_quantity, int *counter,
                    int *match_counter, regex_t preg, char *line,
                    int line_len) {
  regmatch_t pm[2];
  int regexec_res = regexec(&preg, line, 2, pm, 0);

  if (line[line_len - 1] != '\n') {
    strcat(line, "\n");
  }
  if ((regexec_res == 0 && !flags.v) ||
      (regexec_res == REG_NOMATCH && flags.v)) {
    if (file_quantity > 1 && !flags.h && !flags.c && !flags.l) {
      printf("%s:", flags.file_name);
    }
    if (flags.c || flags.l) {
      *match_counter = *match_counter + 1;
    } else if (flags.o) {
      char *new_line = line;
      while (regexec(&preg, new_line, 2, pm, 0) == 0) {
        if (flags.n) {
          printf("%d:", *counter);
        }
        for (int i = (int)pm[0].rm_so; i < (int)pm[0].rm_eo; i++) {
          printf("%c", new_line[i]);
        }
        printf("\n");
        new_line = new_line + (int)pm[0].rm_eo;
      }
    } else if (flags.n && !flags.o) {
      printf("%d:%s", *counter, line);
    } else {
      printf("%s", line);
    }
  }
}