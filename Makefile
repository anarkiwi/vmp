all: vmp.d64 vmp.prg

vmp.prg: vmp.c
	cl65 -Osir -Cl vmp.c -o vmp.prg

vmp.d64: vmp.prg
	c1541 -format diskname,id d64 vmp.d64 -attach vmp.d64 -write vmp.prg vmp
