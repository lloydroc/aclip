#include "wavefile.h"

void
wavgen_header_init(struct wavheader *header)
{
  header->chunkId[0] = 'R';
  header->chunkId[1] = 'I';
  header->chunkId[2] = 'F';
  header->chunkId[3] = 'F';

  header->format[0] = 'W';
  header->format[1] = 'A';
  header->format[2] = 'V';
  header->format[3] = 'E';

  header->subChunk1Id[0] = 'f';
  header->subChunk1Id[1] = 'm';
  header->subChunk1Id[2] = 't';
  header->subChunk1Id[3] = ' ';

  header->subChunk1Size = 16;
  header->audioFormat = 1;

  header->subChunk2Id[0] = 'd';
  header->subChunk2Id[1] = 'a';
  header->subChunk2Id[2] = 't';
  header->subChunk2Id[3] = 'a';
}

void
wavgen_set_sampling(struct wavheader *header, uint16_t num_channels, uint16_t sample_rate, uint16_t bits_per_sample)
{
  int total_bytes_per_sample = num_channels*bits_per_sample/8;
  header->numChannels = num_channels;
  header->sampleRate = sample_rate;
  header->byteRate = sample_rate*total_bytes_per_sample;
  header->blockAlign = total_bytes_per_sample;
  header->bitsPerSample = bits_per_sample;
}

void
wavgen_set_data_size(struct wavheader *header, uint32_t data_size)
{
  header->subChunk2Size = data_size;
  header->chunkSize = data_size+36;
}

void
wavgen_print_info(struct wavheader *header)
{
  int bytes_per_sample = header->bitsPerSample / 8;
  int total_samples = header->subChunk2Size / bytes_per_sample;
  int total_samples_per_channel = total_samples / header->numChannels;
  float duration = ((float) total_samples_per_channel / (float) header->sampleRate);

  printf("Wavfile information\n");
  printf("--------------------------------------\n");
  printf("sampling rate: %d\n", header->sampleRate);
  printf("bits per sample: %d\n", header->bitsPerSample);
  printf("bytes per sample: %d\n", bytes_per_sample);
  printf("channels: %d\n", header->numChannels);
  printf("total data bytes: %d\n", header->subChunk2Size);
  printf("total samples: %d\n", total_samples);
  printf("total samples per channel: %d\n", total_samples_per_channel);
  printf("duration: %0.2fs\n", duration);
  printf("--------------------------------------\n");
}

int
wavgen_read_file(struct wavheader *header, char *filename, void **data)
{
  int ret = 0;
  size_t bytes_read;
  FILE *file = fopen(filename, "r");
  if(file == NULL)
  {
    perror("wavgen_read_file_header: opening file");
    return 1;
  }

  bytes_read = fread(header, 1, sizeof(struct wavheader), file);
  if(bytes_read != sizeof(struct wavheader))
  {
    perror("error reading WAVE file header");
    ret = 2;
    goto close_file;
  }

  if(strncmp(header->chunkId, "RIFF", 4))
  {
    fprintf(stderr, "unable to find RIFF ChunkID in file %s\n", filename);
    ret = 3;
    goto close_file;
  }

  if(strncmp(header->format, "WAVE", 4))
  {
    fprintf(stderr, "format is not WAVE in file %s\n", filename);
    ret = 4;
    goto close_file;
  }

  if(header->audioFormat != 1)
  {
    fprintf(stderr, "Audio Format is not PCM in file %s\n", filename);
    ret = 5;
    goto close_file;
  }

  if(strncmp(header->subChunk2Id, "data", 4))
  {
    fprintf(stderr, "subChunk2ID doesn't contain the \"data\" string in file %s\n", filename);
    ret = 6;
    goto close_file;
  }

  *data = malloc(header->subChunk2Size);
  if(*data == NULL)
  {
    perror("out of memory");
    ret = 7;
    goto close_file;
  }

  bytes_read = fread(*data, 1, header->subChunk2Size, file);
  if(bytes_read != header->subChunk2Size)
  {
    perror("error reading WAVE file data");
    ret = 8;
    goto close_file;
  }

close_file:
  fclose(file);

  return ret;
}

int
wavgen_write_file(struct wavheader *header, char *filename, size_t num_bytes, int8_t *data)
{
  size_t written;
  size_t header_size = sizeof(struct wavheader);
  int ret = 0;

  FILE *file = fopen(filename, "w");
  if(file == NULL)
  {
    perror("opening file");
    ret = 1;
    goto wavgen_write_file_return;
  }

  written = fwrite(header, 1, header_size, file);
  if(written != header_size)
  {
    fprintf(stderr, "bytes written %zd not equal to header size %zd\n", written, header_size);
    ret = 1;
    goto wavgen_write_file_return;
  }

  wavgen_set_data_size(header, num_bytes);
  written = fwrite(data, 1, header->subChunk2Size, file);
  if(written != header->subChunk2Size)
  {
    fprintf(stderr, "bytes written not equal to data size\n");
    ret = 1;
    goto wavgen_write_file_return;
  }

wavgen_write_file_return:
  fclose(file);

  return ret;
}
