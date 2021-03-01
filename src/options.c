#include "options.h"

void options_usage(char *progname)
{
  printf("\nUsage: %s [OPTIONS] TEXT\n\n", progname);
  printf("Options:\n\
-h, --help                  Print this menu\n\
--sampling-rate INTEGER     Sampling Rate [8000]\n\
--num-channels INTEGER      Number of Channels [1]\n\
--alsa-device STRING        Alsa device to connect [default]\n\
--record                    Unconditionally Record Audio\n\
\n\
");
  return;
}

void options_init(struct options *options)
{
  options->help = 0;
  options->error = 0;
  options->sampling_rate = 8000;
  options->num_channels = 1;
  options->alsa_device = "default";
  options->unconditional_record = 0;
}

void options_parse(struct options *options, int argc, char *argv[])
{
  int c;
  static struct option long_options[] = {
    {"help",                      no_argument,       0,  0 },
    {"num-channels",              required_argument, 0,  0 },
    {"sampling-rate",             required_argument, 0,  0 },
    {"alsa-device",               required_argument, 0,  0 },
    {"record",                          no_argument, 0,  0 },
    {0,                                           0, 0,  0 }
  };

  while (1) {
      int option_index = 0;
      c = getopt_long(argc, argv, "h", long_options, &option_index);
      if (c == -1)
          break;

      switch (c) {
      case 0:
          if(strcmp("help", long_options[option_index].name) == 0)
          {
            options->help = 1;
            break;
          }
          if(strcmp("sampling-rate", long_options[option_index].name) == 0)
          {
            options->sampling_rate = atoi(optarg);
            break;
          }
          if(strcmp("num-channels", long_options[option_index].name) == 0)
          {
            options->num_channels = atoi(optarg);
            break;
          }
          if(strcmp("alsa-device", long_options[option_index].name) == 0)
          {
            options->alsa_device = strdup(optarg);
            break;
          }
          if(strcmp("record", long_options[option_index].name) == 0)
          {
            options->unconditional_record = 1;
            break;
          }

          else
          {
            options->error = 1;
            break;
          }
          break;

        case 'h':
          options->help = 1;
          break;

        default:
          options->error = 1;
      }
  }

  // TODO the extra unparsed options ...

}

void options_destroy(struct options *options)
{
  free(options->alsa_device);
}
