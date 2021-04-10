#include "options.h"

void
options_usage(char *progname)
{
  printf("\nUsage: %s [OPTIONS]\n\n", progname);
  printf("Options:\n\
-h, --help                  Print this menu\n\
--sampling-rate INTEGER     Sampling Rate [48000]\n\
--num-channels INTEGER      Number of Channels [1]\n\
--y-thresh INTEGER          Y-Axis Threshold [1000]\n\
--x-thresh-seconds DECIMAL  X-Axis Threshold in Seconds [3.0]\n\
--alsa-device STRING        Alsa device to connect [default]\n\
--record                    Unconditionally Record Audio\n\
--out-directory STRING      Directory to save files in [./]\n\
\n\
");
  return;
}

void
options_init(struct options *options)
{
  options->help = 0;
  options->error = 0;
  options->sampling_rate = 48000;
  options->num_channels = 1;
  options->thresh_y = 1000;
  options->thresh_x_seconds = 3.0;
  strncpy(options->alsa_device, "default", 255);
  options->unconditional_record = 0;
  options->filepath = NULL;
}

static int
test_open_directory(char *testdir)
{
  DIR* dir = opendir(testdir);
  if (dir) {
    closedir(dir);
    return 0;
  }
  perror(testdir);
  return 1;
}

void
options_parse(struct options *options, int argc, char *argv[])
{
  int c;
  static struct option long_options[] = {
					 {"help",                      no_argument,       0,  0 },
					 {"num-channels",              required_argument, 0,  0 },
					 {"sampling-rate",             required_argument, 0,  0 },
					 {"y-thresh",                  required_argument, 0,  0 },
					 {"x-thresh-seconds",          required_argument, 0,  0 },
					 {"alsa-device",               required_argument, 0,  0 },
					 {"out-directory",             required_argument, 0,  0 },
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
      if(strcmp("y-thresh", long_options[option_index].name) == 0)
	{
	  options->thresh_y = atoi(optarg);
	  break;
	}
      if(strcmp("x-thresh-seconds", long_options[option_index].name) == 0)
	{
	  options->thresh_x_seconds = atof(optarg);
	  break;
	}
      if(strcmp("alsa-device", long_options[option_index].name) == 0)
	{
	  strncpy(options->alsa_device, optarg, 255);
	  break;
	}
      if(strcmp("record", long_options[option_index].name) == 0)
	{
	  options->unconditional_record = 1;
	  break;
	}
      if(strcmp("out-directory", long_options[option_index].name) == 0)
	{
	  options->filepath = strndup(optarg, 4094);
	  if(test_open_directory(options->filepath))
            {
              options->error = 1;
            }
	  else
            {
              if(options->filepath[strlen(options->filepath)-1] != '/')
		{
		  strcat(options->filepath, "/");
		}
            }
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

  if(options->filepath == NULL)
    {
      options->filepath = strndup("./", 2);
    }
  // TODO the extra unparsed options ...

}

void options_destroy(struct options *options)
{
  free(options->filepath);
}
