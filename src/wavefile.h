#ifndef WAVEFILE_H
#define WAVEFILE_H
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * Header for WAVE format.
 * Note char[] are in big endian.
 * All other fields are in little endian
 */
struct wavheader {
  // Riff chunk descriptor
  char     chunkId[4];
  uint32_t chunkSize;
  char     format[4];

  // Format sub-chunk
  char     subChunk1Id[4];
  uint32_t subChunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;

  // data sub-chunk
  char     subChunk2Id[4];
  uint32_t subChunk2Size;
};

void
wavgen_header_init(struct wavheader *header);

void
wavgen_print_info(struct wavheader *header);

void
wavgen_set_sampling(struct wavheader *header, uint16_t num_channels, uint16_t sample_rate, uint16_t bits_per_sample);

void
wavgen_set_data_size(struct wavheader *header, uint32_t data_size);

int
wavgen_read_file(struct wavheader *header, char *filename, void **data);

int
wavgen_write_file(struct wavheader *header, char *filename, size_t num_bytes, int8_t *data);

#endif
