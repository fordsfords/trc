/* trc.h */
/*
# This code and its documentation is Copyright 2002-2021 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/trc
*/
#ifndef TRC_H
#define TRC_H

#include "cprt.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define TRC_ERR(_trc_err) CPRT_ASSERT((_trc_err) == TRC_OK)


struct trc_event_s {
  uint64_t thread_id;
  uint64_t p1;  /* Application-specific parameter. */
  uint64_t p2;  /* Application-specific parameter. */
  uint64_t file_line;
  char     *file_name;
  struct cprt_timeval timestamp;
};
typedef struct trc_event_s trc_event_t;


#define TRC_CREATE_FLAG_NO_OVERRIDE 0x0000000000000001
#define TRC_CREATE_FLAG_ATOMIC_INC  0x0000000000000002
#define TRC_CREATE_FLAG_TIMESTAMP   0x0000000000000004
#define TRC_CREATE_FLAG_THREAD_ID   0x0000000000000008
struct trc_s {
  uint32_t num_entries;   /* Allocated size of event array. */
  uint32_t event_count;   /* Number of events that have happened so far. */
  uint32_t create_flags;
  uint32_t suppress_cnt;  /* If > 0, prevents trace. */
  trc_event_t *events;
};
typedef struct trc_s trc_t;


/* Return codes. */
#define TRC_OK 0
#define TRC_ERR_BAD_PARM -1
#define TRC_ERR_NO_MEM   -2


int trc_create(trc_t **trc_rtn, uint64_t num_entries, uint32_t create_flags);
int trc_delete(trc_t *trc);
int trc_trace(trc_t *trc, char *file_name, uint64_t file_line, uint64_t p1, uint64_t p2);
void trc_suppress_inc(trc_t *trc);
void trc_suppress_dec(trc_t *trc);
int trc_dump(trc_t *trc, FILE *out_fp);


#ifdef __cplusplus
}
#endif

#endif  /* TRC_H */
