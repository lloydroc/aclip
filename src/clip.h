#ifndef CLIP_H
#define CLIP_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "wavefile.h"

struct Sample
{
  unsigned long t;
  int16_t y;
};

struct Clip
{
  bool prev_recording;
  bool recording;
  int16_t thresh_pos, thresh_neg;
  unsigned long thresh_t;
  struct Sample thresh_first, thresh_current;
  time_t thresh_first_time;
  unsigned long sample_counter, frame_counter;
  FILE *file;
  char filename[64];
};

int
clip_init(struct Clip *clip);

bool
clip_under_threshold(struct Clip *clip, int16_t pcm_buffer[], size_t pcm_buffer_size);

int
clip_wavfile_create(struct Clip *clip, struct wavheader *header, char *pcm_buffer_first, size_t pcm_buffer_size);

int
clip_wavfile_write(struct Clip *clip, char *pcm_buffer, size_t pcm_buffer_size);

int
clip_wavfile_close(struct Clip *clip, struct wavheader *header, char *pcm_buffer, size_t pcm_buffer_size);

#endif
