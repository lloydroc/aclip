#ifndef CLIP_H
#define CLIP_H

#include <stdlib.h>

struct Sample
{
  int32_t t;
  int16_t y;
};

struct Clip
{
  int recording;
  int16_t thresh_pos, thresh_neg;
  struct Sample thresh_first;
  struct Sample thresh_current;
};

#endif
