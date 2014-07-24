#!/usr/bin/python

from sys import argv,stdout
from struct import unpack

ifile = open(argv[1], 'r')

def pab(num):
	stdout.write("0b")

	for i in range(8):
		stdout.write("{0}".format((num>>i)&1))

pseq = 0
pmfr = 0

for i in range(2):
	foo = ifile.read(131088)
	hpos = foo.find("aDtromtu")
	dh = foo[hpos:hpos+50]

	seqcount = dh[32:36]
	mfrcount = dh[36:40]
	discrete = dh[40:42]
	ncofreq = dh[42:50]

	seqnum = unpack("HH", seqcount)
	seqnum = seqnum[0]*65536 + seqnum[1]
	mfrnum = unpack("HH", mfrcount)
	mfrnum = mfrnum[0]*65536 + mfrnum[1]
	disnum = unpack("BB", discrete)
	nconum = unpack("H"*4, ncofreq)
	
	stdout.write("seq: {0} ({1}), mfr: {2} ({3}), disc: ".format(seqnum, seqnum-pseq, mfrnum, mfrnum-pmfr))
	pab(disnum[0])
	stdout.write("-")
	pab(disnum[1])
	stdout.write(", nco: 0x{0:02x}{1:04x}{2:02x}".format(nconum[1],nconum[2],nconum[3]>>8))
	ncosng = (nconum[1]<<24) + (nconum[2]<<8) + (nconum[3]>>8)
	stdout.write(", ncoc: 0x{0:08x}".format(ncosng))
	stdout.write("\n")

#	ncothing = unpack("Q", ncofreq)[0]
#	print ncothing, " = ", hex(int(round(ncothing)))

	pseq = seqnum
	pmfr = mfrnum
	
	
