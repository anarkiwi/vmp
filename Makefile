srcs = vmp.c freq.h midi.h sid.h vessel.h pbmult.h
targets = vmp-de00.d64 vmp-de00.prg vmp-d420.d64 vmp-d420.prg vmp.d64 vmp.prg

all: $(targets)

pbmult.h: ./genpbmult.py
	./genpbmult.py

freq.h: genfreq.py
	./genfreq.py

vmp.prg: $(srcs)
	cl65 -Osir -Cl $< -o $@

vmp.d64: vmp.prg
	c1541 -format diskname,id d64 $@ -attach $@ -write $< vmp

vmp-d420.prg: $(srcs)
	cl65 -Osir -Cl -DD420 $< -o $@

vmp-d420.d64: vmp-d420.prg
	c1541 -format diskname,id d64 $@ -attach $@ -write $< vmp

vmp-de00.prg: $(srcs)
	cl65 -Osir -Cl -DDE00 $< -o $@

vmp-de00.d64: vmp-de00.prg
	c1541 -format diskname,id d64 $@ -attach $@ -write $< vmp

clean:
	rm -f $(targets)

upload: all
	for i in $(targets) ; do ncftpput -p "" -Cv c64 $$i /Temp/$$i ; done
