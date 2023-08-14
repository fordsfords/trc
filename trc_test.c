/* trc_test.c - trace module test.
 * See https://github.com/fordsfords/trc
 * This tries to be portable between Mac, Linux, and Windows.
 */
/*
# This code and its documentation is Copyright 2023 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/trc
*/

#include "cprt.h"

#include <stdio.h>
#include <string.h>


/* Options and their defaults */
int o_testnum = 0;


char usage_str[] = "Usage: trc_test [-h] [-t testnum]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  printf("%s\n", usage_str);
  printf("Where:\n"
      "  -h : print help\n"
      "  -t testnum : run specified test\n");
  exit(0);
}


int main(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "ht:")) != EOF) {
    switch (opt) {
      case 't':
        CPRT_ATOI(optarg, o_testnum);
        break;
      case 'h':
        help();
        break;
      default:
        usage(NULL);
    }  /* switch opt */
  }  /* while getopt */

  if (optind != argc) { usage("Extra parameter(s)"); }

  switch(o_testnum) {
    case 0:  /* no test */
      break;

    case 1:
      fprintf(stderr, "CPRT_ASSERT\n");
      CPRT_ASSERT(o_testnum == 1 && "internal test fail");
      CPRT_ASSERT(o_testnum != 1 && "should fail");
      break;

    case 2:
    {
      FILE *perr_fp;
      fprintf(stderr, "CPRT_PERRNO\n");
      perr_fp = fopen("file_not_exist", "r");
      if (perr_fp == NULL) {
        CPRT_PERRNO("errno should be 'file not found':");
      } else {
        CPRT_ABORT("Internal test failure: 'file_not_exist' appears to exist");
      }
      break;
    }

    case 3:
    {
      FILE *perr_fp;
      fprintf(stderr, "CPRT_EOK0\n");
      perr_fp = fopen("trc_test.c", "r");
      if (perr_fp == NULL) {
        CPRT_ABORT("Internal test failure: 'trc_test.c' appears to not exist");
      } else {
        CPRT_EOK0(fclose(perr_fp) && "internal test fail");
        CPRT_EOK0(fclose(perr_fp) && "should fail with bad file descr");
      }
      break;
    }

    case 4:
    {
      FILE *perr_fp;
      fprintf(stderr, "CPRT_ENULL\n");
      CPRT_ENULL(perr_fp = fopen("trc_test.c", "r")); /* should be OK. */
      CPRT_ENULL(perr_fp = fopen("file_not_exist", "r")); /* should fail. */
      break;
    }

    case 5:
      fprintf(stderr, "CPRT_ABORT\n");
      CPRT_ABORT("CPRT_ABORT test");
      break;

    case 6:
      fprintf(stderr, "CPRT_SLEEP_SEC\n");
      CPRT_SLEEP_SEC(1);
      fprintf(stderr, "Done\n");
      break;

    case 7:
      fprintf(stderr, "CPRT_SLEEP_MS 1000\n");
      CPRT_SLEEP_MS(1000);
      fprintf(stderr, "Done\n");
      break;

    case 8:
    {
      char *str, *word, *context;

      fprintf(stderr, "STRTOK_PORT\n");
      str = strdup("abc,xyz,123");
      for (word = strtok_r(str, ",", &context);
          word != NULL;
          word = strtok_r(NULL, ",", &context)) {
        printf("word='%s'\n", word);
      }

      break;
    }

    default: /* CPRT_ABORT */
      CPRT_ABORT("unknown option, aborting.");
  }

  return 0;
}  /* main */
