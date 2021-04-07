#include "alsa.h"

int
alsa_pcm_open(struct Alsa *alsa)
{
  int rc = snd_pcm_open(&alsa->handle, alsa->device, SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0)
  {
    fprintf(stderr, "unable to open pcm device \"%s\": %s\n", alsa->device, snd_strerror(rc));
  }
  return rc;
}

int
alsa_pcm_parameters_set(struct Alsa *alsa)
{
  int dir = 0;
  int rc = 0;
  unsigned int val;
  unsigned int byte_rate;

  byte_rate = alsa->num_channels * alsa->bytes_per_sample;

  alsa->frames = alsa->sampling_rate / byte_rate;
  alsa->pcm_buffer_size = alsa->frames * byte_rate;

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&alsa->params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(alsa->handle, alsa->params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(alsa->handle, alsa->params, SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(alsa->handle, alsa->params, SND_PCM_FORMAT_S16_LE);

  /* set channels */
  snd_pcm_hw_params_set_channels(alsa->handle, alsa->params, alsa->num_channels);

  /* sampling rate */
  snd_pcm_hw_params_set_rate_near(alsa->handle, alsa->params, &alsa->sampling_rate, &dir);

  /* Set period size to 32 frames. */
  snd_pcm_hw_params_set_period_size_near(alsa->handle, alsa->params, &alsa->frames, &dir);

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_rate(alsa->params, &val, &dir);
  printf("rate: %d ", val);
  snd_pcm_hw_params_get_channels(alsa->params, &val);
  printf("channels: %d ", val);
  snd_pcm_hw_params_get_period_size(alsa->params, &alsa->frames, &dir);
  printf("frames: %ld\n", alsa->frames);


  alsa->pcm_buffer = malloc(alsa->pcm_buffer_size);
  if(alsa->pcm_buffer == 0)
  {
    perror("malloc ALSA PCM Buffer");
    return 1;
  }

  alsa->prev_pcm_buffer = malloc(alsa->pcm_buffer_size);
  if(alsa->prev_pcm_buffer == 0)
  {
    perror("malloc ALSA PCM Previous Buffer");
    return 1;
  }

  rc = snd_pcm_hw_params(alsa->handle, alsa->params);
  if (rc < 0)
  {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
  }
  return rc;
}

int
alsa_pcm_read(struct Alsa *alsa)
{
    int rc = snd_pcm_readi(alsa->handle, alsa->pcm_buffer, alsa->frames);
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(alsa->handle);
    } else if (rc < 0) {
      fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
    } else if (rc != (int)alsa->frames) {
      fprintf(stderr, "short read, read %d frames expected %ld\n", rc, alsa->frames);
    }
    return rc;
}

int
alsa_pcm_destroy(struct Alsa* alsa)
{
  int ret = 0;
  ret = snd_pcm_drain(alsa->handle);
  if(ret)
    perror("snd_pcm_drain\n");

  ret = snd_pcm_close(alsa->handle);
  if(ret)
    perror("snd_pcm_close\n");

  return ret;
}
