#ifndef CLIP_H
#define CLIP_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct Sample
{
  int32_t t;
  int16_t y;
};

struct Clip
{
  bool prev_recording;
  bool recording;
  int16_t thresh_pos, thresh_neg;
  struct Sample thresh_first, thresh_current;
  time_t thresh_first_time;
  unsigned long sample_counter;
};

int
clip_init(struct Clip *clip);

bool
clip_under_threshold(struct Clip *clip, int16_t pcm_buffer[], size_t pcm_buffer_size);

int
clip_keep_recording(struct Clip *clip);

/*
int
clip_generate_wavfile(struct Clip *clip, char *pcm_buffer_first, 
*/

#endif
