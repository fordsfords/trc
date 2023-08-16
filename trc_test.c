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

#include "trc.h"


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

  printf("Test %d...", o_testnum);

  switch(o_testnum) {
    case 0:  /* no test */
      printf("OK\n");
      break;

    case 1:
    {
      trc_t *trc;  int err;  int i;
      FILE *out_fd;

      err = trc_create(&trc, 10, TRC_CREATE_FLAG_NO_OVERRIDE);  TRC_ERR(err);
      CPRT_ASSERT(trc->num_entries == 10);
      CPRT_ASSERT(trc->event_count == 0);
      CPRT_ASSERT(trc->create_flags == TRC_CREATE_FLAG_NO_OVERRIDE);

      err = trc_trace(trc, __FILE__, __LINE__, 11, 12);  TRC_ERR(err);
      CPRT_ASSERT(trc->event_count == 1);
      CPRT_ASSERT(trc->events[0].timestamp.tv_sec == 0);
      CPRT_ASSERT(trc->events[0].thread_id == 0);
      CPRT_ASSERT(trc->events[0].p1 == 11);
      CPRT_ASSERT(trc->events[0].p2 == 12);

      trc_suppress_inc(trc);
      err = trc_trace(trc, __FILE__, __LINE__, 11, 12);  TRC_ERR(err);
      CPRT_ASSERT(trc->event_count == 1);
      trc_suppress_dec(trc);

      for (i = 1; i < 10; i++) {
        err = trc_trace(trc, __FILE__, __LINE__, 11+i, 12+i);  TRC_ERR(err);
        CPRT_ASSERT(trc->event_count == i + 1);
      }
      CPRT_ASSERT(trc->events[0].p1 == 11);
      CPRT_ASSERT(trc->events[9].p1 == 20);

      err = trc_trace(trc, __FILE__, __LINE__, 98, 99);  TRC_ERR(err);
      CPRT_ASSERT(trc->events[0].p1 == 98);
      CPRT_ASSERT(trc->events[0].p2 == 99);

      CPRT_ENULL(out_fd = fopen("dump1.x", "w"));
      err = trc_dump(trc, out_fd);  TRC_ERR(err);
      fclose(out_fd);

      err = trc_delete(trc);  TRC_ERR(err);
      printf("OK\n");
      break;
    }

    case 2:
    {
      trc_t *trc;  int i;
      FILE *out_fd;

      TRC_ERR(trc_create(&trc, 10, 0));
      /* The following values must match the corresponding env vars in tst.sh. */
      CPRT_ASSERT(trc->num_entries == 11);
      CPRT_ASSERT(trc->create_flags == 0x0f);

      TRC_ERR(trc_trace(trc, __FILE__, __LINE__, 11, 12));
      CPRT_ASSERT(trc->event_count == 1);
      CPRT_ASSERT(trc->events[0].timestamp.tv_sec != 0);
      CPRT_ASSERT(trc->events[0].thread_id != 0);
      CPRT_ASSERT(trc->events[0].p1 == 11);
      CPRT_ASSERT(trc->events[0].p2 == 12);

      trc_suppress_inc(trc);
      TRC_ERR(trc_trace(trc, __FILE__, __LINE__, 11, 12));
      CPRT_ASSERT(trc->event_count == 1);
      trc_suppress_dec(trc);

      for (i = 1; i < 11; i++) {
        TRC_ERR(trc_trace(trc, __FILE__, __LINE__, 11+i, 12+i));
        CPRT_ASSERT(trc->event_count == i + 1);
      }
      CPRT_ASSERT(trc->events[0].p1 == 11);
      CPRT_ASSERT(trc->events[10].p1 == 21);

      TRC_ERR(trc_trace(trc, __FILE__, __LINE__, 98, 99));
      CPRT_ASSERT(trc->events[0].p1 == 98);
      CPRT_ASSERT(trc->events[0].p2 == 99);

      CPRT_ENULL(out_fd = fopen("dump2.x", "w"));
      TRC_ERR(trc_dump(trc, out_fd));
      fclose(out_fd);

      TRC_ERR(trc_delete(trc));
      printf("OK\n");
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
