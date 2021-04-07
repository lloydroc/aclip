#ifndef ALSA_H
#define ALSA_H

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

struct Alsa
{
  char *device;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int sampling_rate;
  unsigned int num_channels;
  unsigned int bytes_per_sample;
  snd_pcm_uframes_t frames;
  size_t pcm_buffer_size;
  char *pcm_buffer, *prev_pcm_buffer;
};

int
alsa_pcm_open(struct Alsa *alsa);

int
alsa_pcm_destroy(struct Alsa *alsa);

int
alsa_pcm_parameters_set(struct Alsa *alsa);

int
alsa_pcm_read(struct Alsa *alsa);

#endif
