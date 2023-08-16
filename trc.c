/* trc.c - trace module.
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
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "trc.h"


int trc_create(trc_t **trc_rtn, uint64_t num_entries, uint32_t create_flags)
{
  trc_t *trc;
  int i;

  if (num_entries == 0) {
    return TRC_ERR_BAD_PARM;
  }

  if (! (create_flags & TRC_CREATE_FLAG_NO_OVERRIDE)) {
    /*
     * Allow env vars to override parameters.
     */
    char *env_var = getenv("TRC_NUM_ENTRIES");
    if (env_var != NULL) {
      CPRT_ATOI(env_var, num_entries);
    }

    env_var = getenv("TRC_CREATE_FLAGS");
    if (env_var != NULL) {
      CPRT_ATOI(env_var, create_flags);
    }
  }

  trc = (trc_t *)malloc(sizeof(trc_t));
  if (trc == NULL) { return TRC_ERR_NO_MEM; }

  trc->num_entries = num_entries;
  trc->create_flags = create_flags;
  trc->event_count = 0;
  trc->suppress_cnt = 0;

  CPRT_ENULL(trc->events = (trc_event_t *)malloc(sizeof(trc_event_t) * num_entries));
  if (trc->events == NULL) { free(trc); return TRC_ERR_NO_MEM; }

  /* Allocate physical memory for the event array. */
  for (i = 0; i < num_entries; i++) {
    trc->events[i].thread_id = 0;
    trc->events[i].file_name = NULL;
    trc->events[i].timestamp.tv_sec = 0;  trc->events[i].timestamp.tv_usec = 0;
  }

  *trc_rtn = trc;  /* Return the object. */

  return TRC_OK;
}  /* trc_create */


int trc_delete(trc_t *trc)
{
  free(trc->events);
  (*(volatile trc_event_t **)(&(trc->events))) = NULL;
  free(trc);

  return TRC_OK;
}  /* trc_create */


int trc_trace(trc_t *trc, char *file_name, uint64_t file_line, uint64_t p1, uint64_t p2)
{
  uint64_t i;

  if (trc->suppress_cnt > 0) {
    return 0;
  }

  if (trc->create_flags & TRC_CREATE_FLAG_ATOMIC_INC) {
    i = CPRT_ATOMIC_INC_VAL(&trc->event_count) - 1;  /* Get pre-increment value. */
  }
  else {
    i = trc->event_count++;
  }
  trc->events[i % trc->num_entries].p1 = p1;
  trc->events[i % trc->num_entries].p2 = p2;
  trc->events[i % trc->num_entries].file_name = file_name;
  trc->events[i % trc->num_entries].file_line = file_line;
  if (trc->create_flags & TRC_CREATE_FLAG_TIMESTAMP) {
    CPRT_TIMEOFDAY(&trc->events[i % trc->num_entries].timestamp, NULL);
  }
  if (trc->create_flags & TRC_CREATE_FLAG_THREAD_ID) {
    trc->events[i % trc->num_entries].thread_id = (uint64_t)(CPRT_GET_THREAD_ID());
  }

  return TRC_OK;
}  /* trc_trace */


void trc_suppress_inc(trc_t *trc)
{
  CPRT_ATOMIC_INC_VAL(&trc->suppress_cnt);
}  /* trc_suppress_inc */


void trc_suppress_dec(trc_t *trc)
{
  CPRT_ATOMIC_DEC_VAL(&trc->suppress_cnt);
}  /* trc_suppress_dec */


int trc_dump(trc_t *trc, FILE *out_fp)
{
  struct cprt_timeval timestamp;
  struct tm tm_buf;
  uint64_t cur_event_num;
  uint64_t num_loops;
  uint64_t i;

  trc_suppress_inc(trc);  /* Disable new traces while dumping. */

  CPRT_TIMEOFDAY(&timestamp, NULL);
  CPRT_LOCALTIME_R(&(timestamp.tv_sec), &tm_buf);
  fprintf(out_fp, "trc_dump: build: %s %s, dump: %04d/%02d/%02d %02d:%02d:%02d.%06d, event_count=%"PRIu32"\n",
      __DATE__, __TIME__,
      (int)tm_buf.tm_year + 1900, (int)tm_buf.tm_mon + 1, (int)tm_buf.tm_mday,
      (int)tm_buf.tm_hour, (int)tm_buf.tm_min, (int)tm_buf.tm_sec, (int)timestamp.tv_usec,
      trc->event_count);

  if (trc->event_count <= trc->num_entries) {  /* If not full. */
    cur_event_num = 0;
    num_loops = trc->event_count;
  } else {  /* Its full. */
    cur_event_num = trc->event_count - trc->num_entries;
    num_loops = trc->num_entries;
  }

  for (i = 0; i < num_loops; i++) {
    trc_event_t *ev = &trc->events[cur_event_num % trc->num_entries];
    fprintf(out_fp, "  ev[%"PRIu64"].thread_id=%"PRIu64", .p1=%"PRIu64", .p2=%"PRIu64", %s:%"PRIu64,
        cur_event_num, ev->thread_id, ev->p1, ev->p2, ev->file_name, ev->file_line);
    if (trc->create_flags & TRC_CREATE_FLAG_TIMESTAMP) {
      CPRT_LOCALTIME_R(&(ev->timestamp.tv_sec), &tm_buf);
      fprintf(out_fp, ", %04d/%02d/%02d %02d:%02d:%02d.%06d",
          (int)tm_buf.tm_year + 1900, (int)tm_buf.tm_mon + 1, (int)tm_buf.tm_mday,
          (int)tm_buf.tm_hour, (int)tm_buf.tm_min, (int)tm_buf.tm_sec, (int)ev->timestamp.tv_usec);
    }
    fprintf(out_fp, "\n");

    cur_event_num++;
  }

  trc_suppress_dec(trc);  /* Re-enable tracing. */

  return TRC_OK;
}  /* trc_dump */
