#ifndef OPTIONS_H
#define OPTIONS_H
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

struct options {
  int help;
  int error;
  unsigned int sampling_rate;
  unsigned int num_channels;
  unsigned int thresh_y;
  float thresh_x_seconds;
  char alsa_device[256];
  int unconditional_record;
  char *filepath;
};

void options_usage(char *progname);

void options_init(struct options *options);
void options_destroy(struct options *options);

void options_parse(struct options *options, int argc, char *argv[]);

#endif
