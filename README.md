bfr
===

nonblocking 8-bit-clean pipe buffer, 原代码所在网址: http://archive.ubuntu.com/ubuntu/pool/universe/b/bfr/bfr_1.6.orig.tar.gz

The program 'bfr' acts like a buffering dropin replacement for 'cat'... the
command line arguments mainly exist to you tune the specifics of that (and
let you see a nifty progress bar over stderr).  The audio varient, bfp, has
the same input semantics (and the same command-line args, plus a few audio-
related ones) although I usually find myself reading from the default stdin,
piped out of ogg123 or mpg321), and configures/writes /dev/dsp rather than
writing to stdout.  Please see the --help and/or the manpage if you need help
figuring out how to tweak it.

If you have any problems with the program, please let me know.  This isn't a
guarantee, but I will try.  I disclaim all liability, so don't sue me if I 
can't get something you want working.  Use at your own risk.  This software
is released under the condition that you follow the guidelines of the GNU
General Public License (included with the source in the file 'COPYING'), with
the following clarification: The GPL refers numerous times to "derivitave
work".  The author (me) hereby states that usage of the compiled program,
source unaltered, in a script or whatnot, for any purpose you may concoct
does NOT count as a "derivitave work".  Use freely. =)

You're welcome to tinker with it, and if you make an improvement, or have 
used it in a way that I'm unlikely to have envisioned, or (especially!) fix
a bug, please send it my way.  Above all, I hope its as useful to you as it
is to me. =)
