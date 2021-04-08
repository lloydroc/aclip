#include "clip.h"

int
clip_init(struct Clip *clip)
{
  clip->prev_recording = false;
  clip->recording = false;
  clip->sample_counter = 0L;
  clip->frame_counter = 0L;
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
      //printf("\rframe: %08ld value: %08d=%08d of %08d", clip->frame_counter, i, pcm_value, pcm_buffer_size);
      bool meets_thresh = pcm_value > clip->thresh_pos || pcm_value < clip->thresh_neg;

      if(meets_thresh)
	{
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

  if(under_threshold == false)
    {
      clip->recording = true;
      clip->frame_counter++;
    }
  else if( (clip->sample_counter - clip->thresh_current.t) > clip->thresh_t)
    {
      clip->recording = false;
    }

  if(clip->prev_recording == false && clip->recording == false)
    {
      clip->frame_counter = 0L;
    }

  return under_threshold;
}

static int
clip_get_filename_duration(struct Clip *clip, float seconds)
{
  struct tm *tmp;
  tmp = localtime(&clip->thresh_first_time);
  char timestr[32];

  if (tmp == NULL) {
    perror("localtime");
    return 1;
  }

  if(strftime(timestr, sizeof(timestr), "%F-%T", tmp) == 0)
   {
    fprintf(stderr, "strftime returned 0");
    return 2;
  }

  if(seconds > 0.0)
    {
      sprintf(clip->filename, "%sclip_%.2f_", clip->filepath, seconds);
    }
  else
    {
      sprintf(clip->filename, "%sclip_", clip->filepath);
    }

  strncat(clip->filename, timestr, sizeof(timestr)+1);
  strncat(clip->filename, ".wav", 5);
  return 0;
}

int
clip_wavfile_create(struct Clip *clip, struct wavheader *header, char *pcm_buffer_first, size_t pcm_buffer_size)
{
  ssize_t written;
  size_t header_size = sizeof(struct wavheader);

  if(clip_get_filename_duration(clip, -1.0))
    {
      return 1;
    }

  clip->file = fopen(clip->filename, "w");
  if(clip->file == NULL)
    {
      perror("opening file");
      return 2;
    }

  written = fwrite(&header, 1, header_size, clip->file);
  if(written != header_size)
    {
      fprintf(stderr, "bytes written %zd not equal to header size %zd\n", written, header_size);
      return 3;
    }

  if(clip_wavfile_write(clip, pcm_buffer_first, pcm_buffer_size))
    {
      return 4;
    }

  return 0;
}

int
clip_wavfile_write(struct Clip *clip, char *pcm_buffer, size_t pcm_buffer_size)
{
  ssize_t written;
  written = fwrite(pcm_buffer, 1, pcm_buffer_size, clip->file);
  if(written != pcm_buffer_size)
    {
      fprintf(stderr, "bytes written %zd not equal to PCM buffer size %zd\n", written, pcm_buffer_size);
      return 1;
    }

  return 0;
}

int
clip_wavfile_close(struct Clip *clip, struct wavheader *header, char *pcm_buffer, size_t pcm_buffer_size)
{
  int ret = 0;
  long pcm_bytes;
  size_t header_size = sizeof(struct wavheader);
  ssize_t written;

  // to rename the file with the duration
  float seconds;
  char *orig_filename, *new_filename;

  if(clip_wavfile_write(clip, pcm_buffer, pcm_buffer_size))
    {
      ret = 1;
      goto clip_wavfile_close_exit;
    }

  pcm_bytes = ftell(clip->file);
  if(pcm_bytes == -1L)
    {
      perror("ftell");
      ret = 2;
      goto clip_wavfile_close_exit;
    }

  pcm_bytes -= header_size;

  wavgen_set_data_size(header, pcm_bytes);

  rewind(clip->file);
  written = fwrite(header, 1, header_size, clip->file);
  if(written != header_size)
    {
      fprintf(stderr, "bytes written %zd not equal to header size %zd\n", written, header_size);
      ret = 3;
      goto clip_wavfile_close_exit;
    }

  pcm_bytes /= header->numChannels;
  pcm_bytes /= header->bitsPerSample/8;
  seconds = (float) pcm_bytes;
  seconds /= (float) header->sampleRate;

  orig_filename = strdup(clip->filename);
  clip_get_filename_duration(clip, seconds);
  new_filename = clip->filename;

  rename(orig_filename, new_filename);

  free(orig_filename);

 clip_wavfile_close_exit:
  fclose(clip->file);

  return ret;
}
