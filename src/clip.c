#include "clip.h"

int
clip_init(struct Clip *clip)
{
  clip->prev_recording = false;
  clip->recording = false;
  clip->sample_counter = 0L;
  return 0;
}

bool
clip_under_threshold(struct Clip *clip, int16_t pcm_buffer[], size_t pcm_buffer_size)
{
  int16_t pcm_value;
  bool under_threshold = true;
  clip->prev_recording = clip->recording;
  for(int i=0; i<pcm_buffer_size; i++)
  {
    pcm_value = pcm_buffer[i];
    bool meets_thresh = pcm_value > clip->thresh_pos || pcm_value < clip->thresh_neg;
    if(meets_thresh)
    {
      printf("meets thresh ");
      under_threshold = false;
      clip->thresh_current.t = clip->sample_counter+i;
      clip->thresh_current.y = pcm_value;
      if(clip->recording == 0)
      {
        clip->thresh_first.t = clip->thresh_current.t;
        clip->thresh_first.y = pcm_value;
        clip->recording = true;
        clip->prev_recording = false;
        clip->thresh_first_time = time(NULL);
      }
    }
  }
  clip->sample_counter += pcm_buffer_size;
  return under_threshold;
}

int
clip_wavfile_create(struct Clip *clip, struct wavheader *header, char *pcm_buffer_first, size_t pcm_buffer_size)
{
  return 0;
}

int
clip_wavfile_write(struct Clip *clip, char *pcm_buffer, size_t pcm_buffer_size)
{
  return 0;
}

int
clip_wavfile_close(struct Clip *clip, struct wavheader *header, char *pcm_buffer, size_t pcm_buffer_size)
{
  return 0;
}


