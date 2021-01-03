// Copyright 2021 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <stdio.h>
#include "vessel.h"
#include "freq.h"
#include "midi.h"
#include "sid.h"


void initvessel(void) {
  register unsigned char i = 0;
  unsigned char *sidreg = NULL;
  unsigned chmask = 0;

  VOUT;
  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)baseptrs.sid[i];
    chmask >>= 1;
    if (sidreg != NULL) {
      chmask |= 0x8000;
      VCMD(7); // enable messages for this channel.
      VW(0b01110000 | i);
    }
  }
  VCMD(5); // enable channels
  VW(chmask >> 8);
  VW(chmask & 0xff);
}

void initsid(void) {
  register unsigned char i = 0;
  register unsigned char j = 0;
  unsigned char *sidreg = NULL;

  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)baseptrs.sid[i];
    if (sidreg != NULL) {
      for (j = 0; j < SIDREGSIZE; ++j) {
	sidreg[j] = 0;
      }
      SIDVOL(sidreg, 0xf);
    }
  }
  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)baseptrs.voice[i];
    if (sidreg != NULL) {
      voicestate.control[i] = 0x10; // Triangle
      voicestate.attackdecay[i] = 0b00000000; // No attack, no decay
      voicestate.sustainrelease[i] = 0b11110000; // Max sustain, no release
    }
  }
}

void init(void) {
  initvessel();
  initsid();
}

void updatevoice(unsigned char ch, unsigned char *voicereg) {
  voicereg[0] = voicestate.lo[ch];
  voicereg[1] = voicestate.hi[ch];
  voicereg[5] = voicestate.attackdecay[ch];
  voicereg[6] = voicestate.sustainrelease[ch];
  voicereg[4] = voicestate.control[ch];
}


void handlenoteoff(unsigned char ch, unsigned char *voicereg) {
  voicestate.control[ch] &= 0b11111110; // Gate off
  updatevoice(ch, voicereg);
}

void handlenoteon(unsigned char ch, unsigned char *voicereg, unsigned char p, unsigned char v) {
  if (v == 0) {
    handlenoteoff(ch, voicereg);
  } else {
    voicestate.lo[ch] = ptosflo[p];
    voicestate.hi[ch] = ptosfhi[p];
    voicestate.control[ch] |= 0x1; // Gate on
    updatevoice(ch, voicereg);
  }
}

void midiloop(void) {
  register unsigned char bc = 0;
  register unsigned char i = 0;
  unsigned char ch = 0;
  unsigned char *voicereg = NULL;

  for (;;) {
    VIN;
    bc = VR;
    if (bc) {
      // Rapidly drain Vessel buffer.
      for (i = 0; i < bc; ++i) {
	buf[i] = VR;
      }
    }
    VOUT;
    for (i = 0; i < bc; i += 3) {
      ch = buf[i] & 0b00001111;
      voicereg = (unsigned char*)baseptrs.voice[ch];
      if (voicereg) {
	switch (buf[i] & 0b11110000) {
	case NOTEON:
	  handlenoteon(ch, voicereg, buf[i+1], buf[i+2]);
	  break;
	case NOTEOFF:
	  handlenoteoff(ch, voicereg);
	  break;
	default:
	  break;
	}
      }
    }
  }
}

void main(void) {
  init();
  midiloop();
}
