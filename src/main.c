/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <signal.h>
#include <time.h>
#include <alsa/asoundlib.h>

#include "alsa.h"
#include "options.h"
#include "wavefile.h"
#include "clip.h"

/* parsed options from getopt */
struct options opts;

/* global types for our sound card */
struct Alsa alsa;

struct Clip clip;

static
void signal_handler(int sig)
{
  int exit_status = 0;

  options_destroy(&opts);

  if(alsa.handle)
  {
    alsa_pcm_destroy(&alsa);
  }

  exit(exit_status);
}

int main(int argc, char *argv[])
{
  int rc;

  if (signal(SIGINT, signal_handler) == SIG_ERR)
    perror("installing SIGNT handler");
  if (signal(SIGTERM, signal_handler) == SIG_ERR)
    perror("installing SIGTERM handler");

  options_init(&opts);
  options_parse(&opts, argc, argv);

  if(opts.help || opts.error)
  {
    options_usage(argv[0]);
    return opts.error;
  }

  alsa.device = opts.alsa_device;
  alsa.sampling_rate = opts.sampling_rate;
  alsa.num_channels = opts.num_channels;

  struct wavheader header;

  /* Open PCM device for recording (capture). */

  if(alsa_pcm_open(&alsa))
  {
    exit(1);
  }

  if(alsa_pcm_parameters_set(&alsa))
  {
    exit(1);
  }

  clip.thresh_pos = 1000;
  clip.thresh_neg = -1000;
  clip.recording = 0;

  wavgen_header_init(&header);
  wavgen_set_sampling(&header, alsa.num_channels, alsa.sampling_rate, 16);

  int16_t *sample_ptr;
  while(1)
  {
    rc = alsa_pcm_read(&alsa);
    if(rc != alsa.frames)
    {
      continue;
    }

    sample_ptr = (int16_t *) alsa.pcm_buffer;
    clip_under_threshold(&clip, sample_ptr, alsa.pcm_buffer_size/2);

    if(clip.prev_recording == false && clip.recording == false)
    {
      memcpy(alsa.prev_pcm_buffer, alsa.pcm_buffer, alsa.pcm_buffer_size);
    }
    else if(clip.prev_recording == false && clip.recording == true)
    {
      printf("creating wavfile\n");
      clip_wavfile_create(&clip, &header, alsa.prev_pcm_buffer, alsa.pcm_buffer_size);
    }
    else if(clip.prev_recording == true && clip.recording == true)
    {
      clip_wavfile_write(&clip, alsa.pcm_buffer, alsa.pcm_buffer_size);
    }
    else if(clip.prev_recording == true && clip.recording == false)
    {
      printf("closing wavfile\n");
      clip_wavfile_close(&clip, &header, alsa.pcm_buffer, alsa.pcm_buffer_size);
    }
  }

  return 0;
}
