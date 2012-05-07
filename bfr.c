/* main program - Bfr.
   Copyright (C) 1999-2002  Mark Glines <mark@glines.org>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program (see: COPYING); if not, write to the Free 
   Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  

   See the file README for clarification of how the author wishes some 
   clauses of the GNU General Public License to be interpreted, as well as
   usage information. */

#include "bfr.h"

struct option options[] = {
	{ "help"      , 0, 0, 'h' },
	{ "fork"      , 0, 0, 'f' },
	{ "verbose"   , 0, 0, 'v' },
#ifdef DEBUG
	{ "debug"     , 0, 0, 'd' },
#endif
	{ "progress"  , 2, 0, 'p' },
	{ "initial"   , 1, 0, 'i' },
	{ "minimum"   , 1, 0, 'm' },
	{ "timeout"   , 1, 0, 't' },
	{ "throttle"  , 1, 0, 'T' },
	{ "speedcap"  , 1, 0, 'C' },
	{ "buffersize", 1, 0, 'b' },
#ifdef BUFPLAY
	{ "speed"     , 1, 0, 's' },
	{ "signed"    , 1, 0, 'S' },
	{ "channels"  , 1, 0, 'c' },
	{ "bytes"     , 1, 0, 'B' },
	{ "output"    , 1, 0, 'o' },
#endif
	{ 0           , 0, 0,  0  }
};

void signal_handler(int signal) {
	vverbose("quitting due to signal ");
	switch(signal) {
		case SIGABRT  : vverbose("SIGABRT");
						break;
		case SIGHUP   : vverbose("SIGHUP");
						break;
		case SIGINT   : vverbose("SIGINT");
						break;
		case SIGPIPE  : vverbose("SIGPIPE");
						break;
		case SIGQUIT  : vverbose("SIGQUIT");
						break;
		case SIGTERM  : vverbose("SIGTERM");
						break;
		case SIGUSR1  : vverbose("SIGUSR1");
						break;
		case SIGUSR2  : vverbose("SIGUSR2");
						break;
		default	   : /* this shouldn't happen btw */
						vverbose("%i (?)",signal);
	}
	vverbose("\n");
#ifdef BUFPLAY
	SND_IOCTL(SNDCTL_DSP_RESET,0);
#endif
	exit(0);
}

void showhelp(char *spork) {
	printf("%s v%s (c) 1999-2003 Mark Glines <mark@glines.org>\n",NAME,VERSION);
	printf("Usage follows:\n\n");

	printf("%s [-v|--verbose] [-t0|--threshold=0] [-T0|--timeout=0]\n",spork);
	printf("	[-b100|--bufsize=100] [-p<arg>|--progress=<arg>] [-m0|--minimum=0]\n");
	printf("	[-T90|--throttle=90] [-C0|--speedcap=0] [<input file or -> ...]\n");

#ifdef BUFPLAY
	printf("	[-s44100|--speed=44100] [-Sy|--signed=y] [-c2|--channels=2]\n");
	printf("	[-B2|--bytespersample=2]\n");
#endif
#ifdef DEBUG
	printf("	[-d|--debug]\n");
#endif
	   
	printf("\nshort --long       default desc\n");
	printf("----------------------------------------------------------------------------\n");
	printf("-h    --help       -       display this (hopefully) helpful message.\n\n");

	printf("-v    --verbose            enable verbosity (use twice for pedantic verbosity)\n\n");

#ifdef DEBUG
	printf("-d    --debug              enable debugging\n\n");

#endif
#ifdef BUFPLAY
	printf("-p    --progress   pCA     Enables \"progress mode\" (see manpage)\n\n");
#else
	printf("-p    --progress   k1k     Enables \"progress mode\" (see manpage)\n\n");
#endif

	printf("-m    --minimum    600k    set the amount of buffer to reach before output\n");
	printf("                           begins (to ensure a full stream even at start).\n\n");

	printf("-i    --initial    minimum Special case of --minimum to preload at the start\n");
	printf("                           of operation.  If unset, --mimumum value is used.\n\n");

	printf("-t    --timeout    0       time, in seconds, to wait before aborting if both\n");
	printf("                           input and output are locked.  0 = wait forever.\n\n");

	printf("-T    --throttle   90      after filling the buffer, the percentage to let the\n");
	printf("                           amount of onhand data to go down to before accepting\n");
	printf("                           more input.\n\n");

	printf("-C    --speedcap   0       If set to a non-zero value, %s will allow only\n",spork);
	printf("                           this many bytes to be output per second.\n\n");

	printf("-b    --buffersize 1M      full size of memory buffer.\n\n");

	printf("-f    --fork               forks off the read half, to work around buggy\n");
	printf("                           kernels which block even in nonblocking mode, such\n");
	printf("                           as when reading from a cdrom or nfs volume hangs\n\n");

#ifdef BUFPLAY
	printf("-o    --output    /dev/dsp selects the output audio device\n\n");
#else
	printf("-o    --output     -       selects the output device\n\n");
#endif

#ifdef BUFPLAY
	printf("-s    --speed      44100   incoming sound data bitrate.\n\n");

	printf("-S    --signed     y       Is the sound data signed (y) or unsigned (n)?\n\n");

	printf("-c    --channels   2       number of channels (1 = mono, 2 = stereo).\n\n");

	printf("-B    --bytes      2       bytes per sound sample (1=8bit, 2=16bit).\n\n");
#endif
	printf("For most numeric arguments, the letters 'k' and 'M' can be used to specify\n");
	printf("kilobytes and megabytes.  Decimals are not allowed, use '6500k' instead of\n");
	printf("'6.5M'.  '1k' means 1024, and '1M' means 1048576.  '80%%' can be used to\n");
	printf("specify 80%% of buffersize.\n\n");
	printf("If timeout is set to 0, the program will block forever.  Stdin/stdout are\n");
	printf("assumed if filenames are omitted, to allow dropin compatibility with 'cat'.\n");

	exit(0);
}

void set_state(state thestate) {
	mystate = thestate;
	vverbose("entering %s mode\n",modestrings[mystate]);
}

unsigned long long data_in_cache() {
	if((unsigned long long)readptr > (unsigned long long)writeptr) {
		/* [    w.......r    ] */
		vverbose("data_in_cache(): %Li\n",(unsigned long long)readptr-(unsigned long long)writeptr);
		return((unsigned long long)readptr-(unsigned long long)writeptr);
	}
	if((unsigned long long)writeptr > (unsigned long long)readptr) {
		/* [....r        w...] */
		vverbose("data_in_cache(): %Li\n",((unsigned long long)bufsize-(unsigned long long)writeptr)+(unsigned long long)readptr);
		return(((unsigned long long)bufsize-(unsigned long long)writeptr)+(unsigned long long)readptr);
	}
	debug("data_in_cache(): bottom of function reached!\n");
	return(0);
}

void update_time() {
	gettimeofday(&tv,NULL);
	if(prev_ts == 0) {
		prev_ts = tv.tv_sec;
		prev_tu = tv.tv_usec;
	}
	thetimeu = ((tv.tv_sec - prev_ts) * 1000000) + (tv.tv_usec - prev_tu);
	if(thetimeu >= 100000) {
		double tt, tin, tout;
		debug("update_tim(): updating running averages\n");
		thetimes = 0;
		while(thetimeu >= 1000000) {
			thetimes++;
			thetimeu -= 1000000;
		}
		tin = ((prev_rp >  readptr) ? ((bufsize-prev_rp)+( readptr)) : ( readptr - prev_rp));
		tout= ((prev_wp > writeptr) ? ((bufsize-prev_wp)+(writeptr)) : (writeptr - prev_wp));
		tt = thetimeu;
		tt /= 1000000;
		tt += thetimes;
		tt = 1/tt;
		tin  *= tt;
		tout *= tt;
		run_avg_i = ((run_avg_i * run_avg_t) + tin ) / (1.0+(float)run_avg_t);
		run_avg_o = ((run_avg_o * run_avg_t) + tout) / (1.0+(float)run_avg_t);

		prev_rp = readptr;
		prev_wp = writeptr;
		run_avg_t++;
		if(run_avg_t > 50)
			run_avg_t = 50;
		prev_ts = tv.tv_sec;
		prev_tu = tv.tv_usec;
		written_this_time = 0;
	}
}

void spit_progress() {
	float fbuf, fin, fout, ftotal;
	char *units1, *units2, tmpline[100];
	unsigned long buf, temp;
	static int lt = 0;
	int tt = 0;
	static unsigned long prev_spit_s = 0, prev_spit_u = 0;
	static unsigned long long lasttime = 0;
	update_time();
	temp = tv.tv_sec - prev_spit_s;
	if(((temp*1000000UL) + (tv.tv_usec - prev_spit_u) > 1000000UL)) {
		unsigned long long thistime;
		int i, point1, point2;
		switch(p_rate) {
			case 1000	 : units1 = "t/s";
							break;
			case 1024	 : units1 = "k/s";
							break;
			case 150000   : units1 = "Xd";
							break;
			case 176400   : units1 = "Xa";
							break;
			case 1000000  : units1 = "M/s";
							break;
			case 1048576  : units1 = "m/s";
							break;
			default       : if(p_rate == (bufsize/100))
								units1 = "%/s";
							else
								units1 = " blocks/s";
		}
		switch(p_units) {
			case 0		: units2 = "%";
							break;
			case 1000	 : units2 = "t";
							break;
			case 1024	 : units2 = "k";
							break;
			case 1000000  : units2 = "M";
							break;
			case 1048576  : units2 = "m";
							break;
			default	   : units2 = "b";
							p_units = 1;
		}
		fbuf = buf = data_in_cache();
		if(p_units > 0)
			fbuf /= p_units;
		else {
			fbuf *= 100;
			fbuf /= bufsize;
		}
		fin  = run_avg_i;
		fin /= p_rate;
		fout = run_avg_o;
		fout /= p_rate;
		ftotal = total_written;
		ftotal /= p_units;
		prev_spit_s = tv.tv_sec;
		prev_spit_u = tv.tv_usec;
		thistime = (data_in_cache() * (40.9 / bufsize));
		point1 = bfr_min(thistime,lasttime);
		point2 = bfr_max(thistime,lasttime);
		if(thistime > 40)
			thistime = 40;
		tt = sprintf(tmpline,"[ ");
		for(i=0;i<point1;i++)
			tmpline[tt++] = '#';
		if(thistime < lasttime)
			for(;i<point2;i++)
				tmpline[tt++] = '<';
		else
			for(;i<point2;i++)
				tmpline[tt++] = '>';
		for(;i<40;i++)
			tmpline[tt++] = '_';
		tt += snprintf(tmpline+tt,sizeof(tmpline)-tt," ][ %.1f%s->%02.1f%s->%.1f%s (%.1f%s) ]",fin,units1,fbuf,units2,fout,units1,ftotal,units2);
		lasttime = thistime;
		/* previous progress bar:
		 * lt = fprintf(stderr,"[(%s) IN: % 3.1f%s -> BUFFER: %02.1f%s -> OUT: % 3.1f%s]        \b\b\b\b\b\b\b\b",modestrings[mystate],fin,units1,fbuf,units2,fout,units1) - 16;
		 */
		while(tt < lt) {
			fputs("\b \b",stderr);
			lt--;
		}
		while(lt && lt--)
			fputc('\b',stderr);
		lt = fprintf(stderr,"%s",tmpline);
	}
}

int read_some() {
	/* 
	 * read a block (don't bother to read after a wrap; if we can still
	 * read, we'll get control again soon enough)
	 *
	 * if we get an error, go to WO mode
	 */

	int retval, wantedbytes;
	retval = wantedbytes = 0;

	if(readptr == bufsize) {
		if(writeptr == 0) {
			set_state(BF);
			return 0;
		}
		readptr = 0;
	}
	debug("read_some(): mystate = %s, readptr = %i, writeptr = %i\n", modestrings[mystate], readptr, writeptr);
	if(mystate == WO) {
		/* Riiight. */
		verbose("read_some(): called when mystate == WO\n");
		abort();
	}

	if(readptr >= writeptr) {
		/* [	w...........r	] */
		debug("read_some(): read type 2: ");
		wantedbytes = bufsize-readptr;
	} else
	if(writeptr > (readptr+1)) {
		/* [....r		   w....] */
		debug("read_some(): read type 3: ");
		wantedbytes = (writeptr-1)-readptr;
	}

	if(wantedbytes) {
		/* read() size cap, to avoid deadlock with nfs and such */
		if(wantedbytes > 4096)
			wantedbytes = 4096;

		debug("want %i bytes\n",wantedbytes);
		retval = read(infd, (buffer+readptr), wantedbytes);
	} else {
		debug("skipping read\n");
		set_state(BF);
		return 0;
	}

	if(retval < 0) {
		if(errno != EAGAIN) {
			perror(NAME);
			debug("read_some(): error on input; byebye stream\n");
			return 1;
		} else
			debug("read_some(): select() lied to us?  got EAGAIN\n");
	} else {
		if(retval == 0) {
			debug("read_some(): read() returned 0; EOF reached\n");
			return 1;
		} else {
			debug("read_some(): read %i bytes\n",retval);
			readptr += retval;
		}
	}

	if(((mystate == IN) && (data_in_cache() > initial)) || ((mystate == RO) && (data_in_cache() > threshold)))
		set_state(RW);
	
	return 0;
}

int bytes_to_write() {
	int wantedbytes = 0;
	if(writeptr == bufsize) {
		writeptr = 0;
		if(readptr == 0)
			return 0;
	}
	if(writeptr > readptr) {
		/* [....r		   w....] */
		debug("write type 1: ");
		wantedbytes = bufsize-writeptr;
	}
	else 
	if(readptr > writeptr) {
		/* [	w...........r	] */
		wantedbytes = data_in_cache();
		debug("write type 2: ");
	} 
	if(cap) {
		int temp;
		temp = cap - run_avg_o;
		temp -= written_this_time;
		if(temp < 0) {
			debug("cap hit: 0 bytes wanted\n");
			wantedbytes = 0;
		}
		if(temp >= wantedbytes)
			debug("cap miss: %i bytes wanted\n", wantedbytes);
	} else
		debug("%i bytes wanted\n", wantedbytes);
	return wantedbytes;
}

void write_some() {
	/* attempt to write a block
	 *
	 * if we get an error, the program dies immediately; this program's sole
	 * purpose is to output stuff
	 *
	 * also check to see if we can go to mode RW from mode BF 
	 */
	 
	int retval, wantedbytes;
	retval = 0;

	if(writeptr == 0) {
		if(readptr == 0) {
			if(mystate == WO)
				exit(0);
			return;
		}
	}
	debug("write_some(): mystate = %s, readptr = %i, writeptr = %i\n", modestrings[mystate], readptr, writeptr);
	if(mystate == RO || mystate == IN) {
		/* What chew thinkin foo? */
		verbose("write_some(): called when in a readonly mode!\n");
		abort();
	}

	wantedbytes = bytes_to_write();
			
	retval = write(1, (buffer + writeptr), wantedbytes);

	if(retval < 0) {
		/* any error other than EAGAIN is unacceptable */
		if(errno != EAGAIN) {
			perror(NAME);
			debug("write_some(): error on output!\n");
			abort();
		} else {
			debug("write_some(): select() lied to us?  got EAGAIN\n");
			return;
		}
	} else {
		if(retval > 0) {
			total_written += retval;
			writeptr += retval;
			if(cap)
				written_this_time += retval;
			debug("write_some(): wrote %i bytes\n",retval);
			if(writeptr == readptr) {
				/* We've caught up to the reader. */
				debug("write_some(): caught up\n");
				if(mystate == WO) {
					/* our work here is done! */
					exit(0);
				} else {
					/* time to rebuffer */
					/* FIXME: would it be useful to adjust settings here? */
					debug("write_some(): setting mode to RO to rebuffer\n");
					set_state(RO);
				}
			}
		} else {
			if((writeptr == readptr) && (mystate == WO) ) {
				/* our work here is done! */
				exit(0);
			}
		}
	}
		
	if((mystate == BF) && (data_in_cache() <= throttle)) {
		/* resume read()ing */
		set_state(RW);
	}
}

long long realamount(char *spork) {
	char *p;
	long long amount = 0;
	if(!spork) return 0;
	p = spork;
	amount = strtol(p, (char **)NULL, 0);
	while((*p >= '0') && (*p <= '9'))
		p++;

	switch(*p) {
		case 't': amount *= 1000;
				  break;
		case 'K':
		case 'k': amount *= 1024;
				  break;
		case 'M': amount *= 1000000;
				  break;
		case 'm': amount *= (1024*1024);
				  break;
		case 'b': amount *= 1000000000;
				  break;
		case 'G': 
		case 'g': amount *= (1024*1024*1024);
				  break;
		case '%': amount *= bufsize;
				  amount /= 100;
				  if(amount > bufsize) {
				  	verbose("WARNING! %s is higher than 100%%!  rounding down.\n",spork);
				  	amount = bufsize - 1;
				  }
				  break;
		case 0  : break;
		default : fprintf(stderr, "What is '%c' supposed to mean? (try --help)\n",*p);
				  abort();
	}
	return amount;
}
		
/* the main program *gasp* */
int main(int argc, char *argv[]) {
	int finished, retval, filenames, maxval, capping, should_fork;
	fd_set readfds, writefds, exceptfds;
	struct timeval thetime;
	char *optstr, tch;
	char *opt_bufsize, *opt_timeout, *opt_cap, *opt_throttle, *opt_min, *opt_init, *opt_progress, *outdev;
	int temp;
	
	/* defaults */
	opt_min      = "10%";
	opt_init     = NULL;
	opt_bufsize  = "5m";
	opt_timeout	 = "0";
	opt_cap      = "0";
	opt_throttle = "99%";
	opt_progress = NULL;
	should_fork  = 0;

	/* inits */
	infd		= 0;	   /* stdin */
	verbose		= 0;
	p_mode		= 0;
	finished	= 0;
	writeptr	= 0;
	readptr		= 0;
	total_written	= 0;
	filenames	= 0;
	mystate		= IN;
	stdin_mode	= 1;
#ifdef DEBUG
	dodebug		= false;
#endif
#ifdef BUFPLAY
	bytes		= 2;
	channels	= 2;
	speed		= 44100;
	sign		= 1;
	outdev 		= "/dev/dsp";
#else
	outdev 		= "-";
#endif

	optstr		= "hfvm:i:t:T:b:C:p::o:"
#ifdef DEBUG
					"d"
#endif
#ifdef BUFPLAY
					"s:B:c:S:"
#endif
					;


	
	retval = getopt_long(argc, argv, optstr, options, NULL);
	while(retval != EOF) {
		switch(retval) {
			case 'h': showhelp(argv[0]);
			case 'f': should_fork++;
					  break;
			case 'v': verbose++;
					  switch(verbose) {
						case 1 :verbose("verbosity enabled\n");
								break;
						case 2 :verbose("pedantic verbosity enabled\n");
								break;
						default:verbose("I cannot be any more verbose.  Try debugmode.\n");
					  }
					  break;
#ifdef DEBUG
			case 'd': dodebug++;
					  debug("debug output enabled\n");
					  break;
#endif
			case 'm': opt_min = optarg;
					  break;
					  
			case 'i': opt_init = optarg;
					  break;
					  
			case 't': opt_timeout = optarg;
					  break;
					  
			case 'T': opt_throttle = optarg;
					  break;
					  
			case 'C': opt_cap = optarg;
					  break;

			case 'b': opt_bufsize = optarg;
					  break;

			case 'p': opt_progress = optarg ? optarg : (char*)-1;
					  break;

			case 'o': outdev = optarg;
					  verbose("output device set to %s\n",outdev);
					  break;

#ifdef BUFPLAY
			case 's': speed = realamount(optarg);
					  if((speed < 2000) || (speed > 128000)) {
						  fprintf(stderr, "the speed is in Hz, and should be between 2000 and 128000 (not all sound\ncards will support these speeds tho)\n");
						  abort();
					  }
					  verbose("speed set to %i\n",speed);
					  break;

			case 'S': sign = (*optarg == 'y') ? 1 : 0;
					  verbose("%ssigned data selected\n", sign ? "" : "un");
					  break;

			case 'B': bytes = strtol(optarg,NULL,0);
					  if((bytes < 1) || (bytes > 2)) {
						  fprintf(stderr, "bytes is the number of bytes per sample, and should be 1 (8bit) or 2 (16bit)\n");
						  abort();
					  }
					  break;

			case 'c': channels = strtol(optarg,NULL,0);
					  if((channels < 1) || (channels > 128)) {
						  fprintf(stderr, "the number of channels should be 1 (mono), 2 (stereo) or more (not\nsupported by many sound cards, but up to 128 are allowed)\n");
						  abort();
					  }
					  verbose("channels set to %i\n",channels);
					  break;
#endif
		}
		retval = getopt_long(argc, argv, optstr, options, NULL);
	}

	/* process arguments */
	bufsize   = realamount(opt_bufsize);
	threshold = realamount(opt_min);
	initial   = realamount(opt_init ? opt_init : opt_min);
	timeout   = realamount(opt_timeout);
	cap       = realamount(opt_cap);
	throttle  = realamount(opt_throttle);

	/* progress bar */
	if(opt_progress) {
		/* FIXME: rewrite this */
#ifdef BUFPLAY
		if(opt_progress == (char*)-1) opt_progress = "pCA";
#else
		if(opt_progress == (char*)-1) opt_progress = "k1k";
#endif
		tch = *opt_progress;
		p_units = p_rate = 0;
		p_mode = 1;
		switch(tch) {
			case 't'  : p_units = 1000;
						break;
			case 'k'  : p_units = 1024;
						break;
			case 'b'  : p_units = 1;
						break;
			case 'M'  : p_units = 1000000;
						break;
			case 'm'  : p_units = 1024*1024;
						break;
			case '%'  :
			case 'p'  : p_units = 0;
						break;
		}
		opt_progress++;
		tch = *opt_progress;
		if(!tch)
			p_rate = 1024;
		else {
			if(tch == 'C') {
				opt_progress++;
				tch = *opt_progress;
				if(tch == 'D')
					p_rate = 150000;
				else 
					p_rate = 176400;
			} else
				p_rate = realamount(opt_progress);
		}
	}
	
	my_optind = optind;
	if(should_fork) {
		int mypipe[2], rv;
		verbose("forking\n");
		pipe(mypipe);
		rv = fork();
		if(!rv) {
			bufsize = 10240;
			outdev = "-";
			close(mypipe[0]);
			dup2(mypipe[1],1);
			p_mode = 0;
		} else
		if(rv > 0) {
			argv[argc-1] = "-";
			my_optind = argc - 1;
			close(mypipe[1]);
			dup2(mypipe[0],0);
		} else
			exit(fprintf(stderr,"Could not fork!\n"));
	}

	if(my_optind < argc) {
		if(strcmp("-",argv[my_optind])!=0) {
			infd = open(argv[my_optind],O_RDONLY);
			if(infd == -1)
				exit(fprintf(stderr,"Cannot open file: %s\n",argv[my_optind]));
			verbose("opening file %s: %i\n",argv[my_optind],infd);
			stdin_mode = 0;
		}
		my_optind++;
	}

	buffer = (char *)malloc(bufsize);
	if(buffer == NULL) {
		fprintf(stderr,"malloc()ing a buffer of size %i failed!\n",bufsize);
		fprintf(stderr,"Perhaps you don't have enough memory, perhaps you've\n");
		fprintf(stderr,"exceeded a memory usage quota.\n");
		exit(1);
	}
	if(strncmp(outdev,"-",2)) {
		vverbose("opening %s\n",outdev);
		temp = open(outdev,O_WRONLY|O_CREAT|O_TRUNC,0666);
		if(temp < 0) {
			perror(outdev);
			abort();
		}
		if(temp != 1) {
			if(dup2(temp,1) != 1) {
				fprintf(stderr,"dup2() failed!\n");
				abort();
			}
			close(temp);
		}
	}
#ifdef BUFPLAY
	if(!should_fork || strncmp(outdev,"-",2)) {
		vverbose("configuring %s\n",outdev);
		SND_IOCTL(SNDCTL_DSP_RESET, 0);
		SND_IOCTL(SNDCTL_DSP_SETFRAGMENT, 0x7fff000c);
		SND_IOCTL(SNDCTL_DSP_SETFMT,
			((sign)?((bytes==2)?SOUND16_S:AFMT_S8):((bytes==2)?SOUND16_U:AFMT_U8)));
		SND_IOCTL(SNDCTL_DSP_CHANNELS, channels);
		SND_IOCTL(SNDCTL_DSP_SPEED, speed);
	}
#endif
	vverbose("remapping signals\n");
	signal(SIGABRT, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGPIPE, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	vverbose("entering non-blocking mode\n");
	fcntl(infd, F_SETFL, O_NONBLOCK);
	fcntl(1, F_SETFL, O_NONBLOCK);
#ifndef DEBUG
	fcntl(2, F_SETFL, O_NONBLOCK);
#endif
	maxval = 2;
	if(infd >= maxval)
		maxval = infd + 1;
	if(1 >= maxval)
		maxval = 2;
		
	while(!finished) {

		/* clear them */
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		switch(mystate) {
			case IN:/* initial; special case of RO */
			case RO:/* read only (precaching before threshold is reached)
					 * flip to mode WO for read exceptions 
					 */
					 FD_SET(infd,&readfds);
					 FD_SET(1   ,&exceptfds);
					 break;
			case BF:/* buffer full (a nonpermanent version of WO)
					 */
					 FD_SET(1   ,&writefds);
					 FD_SET(1   ,&exceptfds);
					 break;
			case WO:/* write only (input is done)
					 */
					 FD_SET(1   ,&writefds);
					 FD_SET(1   ,&exceptfds);
					 break;
			case RW:/* read/write (full flow)
					 * flip to mode WO for read exceptions 
					 */
					 FD_SET(infd,&readfds);
					 FD_SET(1   ,&writefds);
					 FD_SET(1   ,&exceptfds);
		}
		if(cap) {
			update_time();
			capping = 0;
			if(FD_ISSET(1,&writefds) && !bytes_to_write()) {
				FD_CLR(1,&writefds);
				capping = 1;
			}
			thetime.tv_sec = 0;
			thetime.tv_usec = 100000;
		} else {
			capping = 0;
			if(timeout) {
				thetime.tv_sec = timeout;
				thetime.tv_usec = 0;
			} else if(p_mode) {
				thetime.tv_sec = 0;
				thetime.tv_usec = 200000;
			} else {
				thetime.tv_sec = 30;
				thetime.tv_usec = 0;
			}
		}
				
		retval = select(maxval, &readfds, &writefds, &exceptfds, &thetime);
		if(retval == -1) {
			if(errno != EINTR) {
#ifdef DEBUG
				int theerr = errno;
#endif
				debug("main(): select() returned error %i\n",theerr);
			} else
				debug("main(): select() returned EINTR: timeout = %i\n",timeout);
			if(timeout != 0 && !capping) 
				exit(1);
		} else
		if(FD_ISSET(1,&exceptfds)) {
			/* if I cannot write, this program no longer has a reason to run.*/
			debug("exception on output stream\n");
			abort();
		} else
		if(FD_ISSET(1,&writefds))
			write_some();
		if((FD_ISSET(infd,&readfds)) && (!FD_ISSET(infd,&exceptfds))) {
			if(read_some() == 1) {
				close(infd);
				infd = -1;
				while(infd == -1) {
					if(my_optind < argc) {
						if(strcmp("-",argv[my_optind])) {
							infd = open(argv[my_optind],O_RDONLY|O_NONBLOCK);
							verbose("opening file %s: %i\n",argv[my_optind],infd);
							if(infd == -1)
								perror(NAME);
						} else {
							/* stdin mode */
							if(!stdin_mode) {
								infd = 0;
								fcntl(0, F_SETFL, O_NONBLOCK);
								stdin_mode++;
							} else
								verbose("Uhh, I only get one stdin.\n");
						}
						my_optind++;
					} else {
						set_state(WO);
						infd = 0;
					}
				}
			}
		}
		if(FD_ISSET(infd,&exceptfds)) {
			/* stdin died */
			debug("main(): select() reported exception on input\n");
			set_state(WO);
		}
		if(p_mode == 1)
			spit_progress();
	}
	return 0;
}
