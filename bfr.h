#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifdef BUFPLAY
#include <sys/mman.h>
#endif
#include "getopt.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

typedef enum {false, true} logical;
typedef enum { IN, RO, BF, WO, RW } state;

#ifndef DEBUG
# define debug(args...)
#else
# define debug(args...)  if(dodebug==true) { fprintf(stderr, ## args); }
logical dodebug;
#endif
#define verbose(args...)  if(verbose >= 1) { fprintf(stderr, ## args); }
#define vverbose(args...)  if(verbose >= 2) { fprintf(stderr, ## args); }

#ifdef BUFPLAY
# include <sys/soundcard.h>
# if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define SOUND16_S AFMT_S16_LE
#  define SOUND16_U AFMT_U16_LE
# else
#  define SOUND16_S AFMT_S16_BE
#  define SOUND16_U AFMT_U16_BE
# endif
# define SND_IOCTL(x,y) { int sndtemp = (y); ioctl(1,(x),&sndtemp); }
# ifdef DEBUG
#  define NAME "bfpdebug"
# else
#  define NAME "bfp"
# endif
int speed, channels, bytes, sign;
#endif

#ifndef NAME
# ifdef DEBUG
#  define NAME "bfrdebug"
# else
#  define NAME "bfr"
# endif
#endif

#define bfr_min(a,b) ((a<b)?a:b)
#define bfr_max(a,b) ((a<b)?b:a)

char *buffer;
unsigned char verbose, progress;
state mystate;
char *modestrings[] = {"IN","RO","BF","WO","RW"};
int initial, threshold, bufsize, writeptr, readptr, timeout, infd, outfd, throttle, my_optind, stdin_mode, p_units, p_rate, p_cdmode, p_mode, cap;
int prev_rp = 0, prev_wp = 0, run_avg_t = 0, prev_ts = 0, prev_tu, thetimes, thetimeu, written_this_time = 0;
unsigned long run_avg_i, run_avg_o;
unsigned long long total_written;
struct timeval tv;

typedef struct {
	int argnum;
	void *next;
} argchain;

argchain *inchain_begin = NULL;
argchain *inchain_this = NULL;
