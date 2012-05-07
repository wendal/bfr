#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void showhelp(char *spork) {
    printf("test (c) 1999-2001 Mark Glines <mark@glines.org>\n");
    printf("Usage follows:\n\n");

    printf("%s [-b xxx|--blocksize=xxx] [-t xxx|--time=xxx]\n\n",spork);

    printf("short --long       default desc\n");
    printf("----------------------------------------------------------------------------\n");
    printf("-h    --help               this (probably uninformative) message\n\n");

    printf("-b    --blocksize= 1024    set the buffer block size\n\n");

    printf("-n    --nano=      0       amount of nanoseconds to delay\n\n");

    printf("-s    --secs=      0       second delay count\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int blocksize, i, done, temp;
    char *buffer;
    struct timespec timebetween;

    done                = 0;
    blocksize           = 1024;
    timebetween.tv_sec  = 0;
    timebetween.tv_nsec = 0;
    
    if(argc==1) showhelp(argv[0]);
    for(i=1;i<argc;i++) {
        if(*argv[i] == '-') {
            if((char)*(argv[i]+1) == '-') {
                /* long-style switch */
                if(strstr(argv[i], "--help")) {
                    showhelp(argv[1]);
                } else
                if(strstr(argv[i], "--blocksize=")) {
                    sscanf(argv[i],"--blocksize=%i",&blocksize);
                } else
                if(strstr(argv[i], "--nano=")) {
                    sscanf(argv[i],"--nano=%li",&timebetween.tv_nsec);
                } else
                if(strstr(argv[i], "--secs=")) {
                    sscanf(argv[i],"--secs=%i",(int*)&timebetween.tv_sec);
                }
            } else 
            if((char)*(argv[i]+1) == 0) {
            } else {
                /* short-style switch */
                if(*(argv[i]+1) == 'h') {
                    showhelp(argv[1]);
                } else
                if(*(argv[i]+1) == 'b') {
                    i++;
                    sscanf(argv[i],"%i",&blocksize);
                } else
                if(*(argv[i]+1) == 'n') {
                    i++;
                    sscanf(argv[i],"%li",&timebetween.tv_nsec);
                } else
                if(*(argv[i]+1) == 's') {
                    i++;
                    sscanf(argv[i],"%i",(int*)&timebetween.tv_sec);
                }
            }
        }
    }
    buffer = (char *)malloc(blocksize);
    while(done==0) {
        temp=read(0,buffer,blocksize);
        if(temp==0) {
            exit(0);
        }
        write(1,buffer,temp);
        nanosleep(&timebetween,NULL);
    }
    return 0;
}
