CLEANFILES = test$(EXEEXT)
CFLAGS    := -Wall
EXTRA_DIST = test.c
STRACE     = #strace

test$(EXEEXT): test.c
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) test.c -o test$(EXEEXT)

all: test$(EXEEXT)

testit: ../bfrdebug$(EXEEXT) test$(EXEEXT) data
	@echo
	@echo IMPORTANT!  The following MD5sums should all match!
	@echo
	@md5sum test.randomdata.* | sort
	@echo
	@echo If they do not, you can consider this test to have failed.
	@echo
	@rm -f test.random*

data: data0 data1 data2 data3 data4 data5 data6 data7 data8 data1f data2f data3f data4f data5f data6f data7f data8f

data0:
	cat /dev/urandom | head -c 200000 > test.randomdata.real

data1: data0
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -b 300000 >test.randomdata.test1

data2: data0
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -b 1024 >test.randomdata.test2

data3: data0 test$(EXEEXT)
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test3

data4: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	$(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 test.randomtemp1 test.randomtemp2 test.randomtemp3 test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test4

data5: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test5

data6: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 -t 1 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test6

data7: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 -t 0 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test7

data8: data0 temp1 temp2 temp3 temp4
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvdb 131072 -m 4096 -C 10k test.randomtemp1 test.randomtemp2 - test.randomtemp4 | cat >test.randomdata.test8

data1f: data0
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -fb 300000 >test.randomdata.test1f

data2f: data0
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -fb 1024 >test.randomdata.test2f

data3f: data0 test$(EXEEXT)
	cat test.randomdata.real | $(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test3f

data4f: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	$(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 test.randomtemp1 test.randomtemp2 test.randomtemp3 test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test4f

data5f: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test5f

data6f: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 -t 1 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test6f

data7f: data0 temp1 temp2 temp3 temp4 test$(EXEEXT)
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 -t 0 test.randomtemp1 test.randomtemp2 - test.randomtemp4 | ./test$(EXEEXT) -n 700000 -b 4096 >test.randomdata.test7f

data8f: data0 temp1 temp2 temp3 temp4
	cat test.randomtemp3 | $(STRACE) ../bfrdebug$(EXEEXT) -vvfdb 131072 -m 4096 -C 10k test.randomtemp1 test.randomtemp2 - test.randomtemp4 | cat >test.randomdata.test8f

temp1: data0
	tail -c 700000 test.randomdata.real | head -c 50000 >test.randomtemp1

temp2: data0
	tail -c 150000 test.randomdata.real | head -c 50000 >test.randomtemp2

temp3: data0
	tail -c 100000 test.randomdata.real | head -c 50000 >test.randomtemp3

temp4: data0
	tail -c 50000 test.randomdata.real | head -c 50000  >test.randomtemp4
