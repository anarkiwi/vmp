all: vmp.d64 vmp.prg

freq.h: genfreq.py
	./genfreq.py

vmp.prg: vmp.c freq.h midi.h sid.h vessel.h
	cl65 -Osir -Cl vmp.c -o vmp.prg

vmp.d64: vmp.prg
	c1541 -format diskname,id d64 vmp.d64 -attach vmp.d64 -write vmp.prg vmp

upload: all
	ncftpput -p "" -Cv c64 vmp.prg /Temp/vmp.prg
