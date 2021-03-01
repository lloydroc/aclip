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

int
get_filename(char *outstr)
{
  time_t t;
  struct tm *tmp;
  t = time(NULL);
  tmp = localtime(&t);
  char timestr[100];

  if (tmp == NULL) {
      perror("localtime");
      return 1;
  }

  if(strftime(timestr, sizeof(timestr), "%F-%T", tmp) == 0) {
    fprintf(stderr, "strftime returned 0");
    return 2;
  }
  sprintf(outstr, "clip_");
  strncat(outstr, timestr, sizeof(timestr)+1);
  strncat(outstr, ".wav", 5);
  return 0;
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
  size_t header_size = sizeof(header);
  size_t written;
  FILE *file = NULL;

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

  char filename[100];

  int16_t *sample_ptr;
  int32_t sample = 0;
  while(1)
  {
    rc = alsa_pcm_read(&alsa);
    if(rc != alsa.frames)
    {
      continue;
    }

    sample_ptr = (int16_t *) alsa.pcm_buffer;
    for(int i=0;i<alsa.pcm_buffer_size/2; i++)
    {
      int meets_thresh = *sample_ptr > clip.thresh_pos || *sample_ptr < clip.thresh_neg;
      if(meets_thresh)
      {
        clip.thresh_current.t = sample;
        clip.thresh_current.y = *sample_ptr;
        if(clip.recording == 0)
        {
          clip.thresh_first.t = sample;
          clip.thresh_first.y = *sample_ptr;
          clip.recording = 1;

          /* get the name of the file to write */
          if(get_filename(filename))
          {
            goto exit_out;
          }

          printf("recording %s %d ", filename, sample);

          /* open file for writing */
          file = fopen(filename, "w");
          if(file == NULL)
          {
            perror("opening file");
            goto exit_out;
          }

          puts("opened file");

          written = fwrite(&header, 1, header_size, file);
          if(written != header_size)
          {
            fprintf(stderr, "bytes written %d not equal to header size %d\n", written, header_size);
            goto exit_out;
          }

          /* skip ahead past the header to write the samples */
          /* cannot seek if the file is empty ... */
          /*if(fseek(file, header_size, SEEK_SET))
          {
            perror("fseek");
            goto exit_out;
          }*/

          fflush(stdout);
        }
     }
     else if(clip.recording && ((sample-clip.thresh_current.t) > 16000))
      {
        clip.recording = 0;
        printf("%d %d %.2f\n", sample, sample-clip.thresh_current.t, (float)(sample-clip.thresh_first.t)/8000.0);

        long data_size = ftell(file);
        if(data_size == -1L)
        {
          perror("ftell");
          goto exit_out;
        }

        data_size -= sizeof(header_size);

        wavgen_set_data_size(&header, data_size);

        rewind(file);
        written = fwrite(&header, 1, header_size, file);
        if(written != header_size)
        {
          fprintf(stderr, "bytes written %d not equal to header size %d\n", written, header_size);
          goto exit_out;
        }

        fclose(file);
      }

      sample++;
    }

    if(clip.recording)
    {
      written = fwrite(alsa.pcm_buffer, 1, alsa.pcm_buffer_size, file);
      if(written != alsa.pcm_buffer_size)
      {
        fprintf(stderr, "bytes written %d not equal to buffer size %d\n", written, alsa.pcm_buffer_size);
        goto exit_out;
      }
    }
  }

exit_out:
  if(file)
    fclose(file);

  return 0;
}
