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
    sidreg = (unsigned char*)voicebase[i].sid;
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
    sidreg = (unsigned char*)voicebase[i].sid;
    if (sidreg != NULL) {
      for (j = 0; j < SIDREGSIZE; ++j) {
	*(sidreg + j) = 0;
      }
      SIDVOL(sidreg, 0xf);
    }
  }
  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)voicebase[i].voice;
    if (sidreg != NULL) {
      *(sidreg+4) = 0x10; // Triangle
      *(sidreg+5) = 0b00000000; // No attack, no decay
      *(sidreg+6) = 0b11110000; // Max sustain, no release
    }
  }
}

void init(void) {
  initvessel();
  initsid();
}

void handlenoteoff(unsigned char ch, unsigned char p, unsigned char v) {
  unsigned char *voicereg = (unsigned char*)voicebase[ch].voice;
  if (voicereg == NULL) {
    return;
  }
  *(voicereg+4) = 0x10; // Triangle
}

void handlenoteon(unsigned char ch, unsigned char p, unsigned char v) {
  unsigned char *voicereg = (unsigned char*)voicebase[ch].voice;
  if (voicereg == NULL) {
    return;
  }
  if (v == 0) {
    handlenoteoff(ch, p, 0);
  } else {
    *voicereg = ptosflo[p];
    *(voicereg+1) = ptosfhi[p];
    *(voicereg+4) = 0x11; // Gate on
  }
}

void midiloop(void) {
  unsigned char bc = 0;
  unsigned char ch = 0;
  unsigned char msg = 0;
  unsigned char db1 = 0;
  enum parserstateenum parserstate = PNONE;
  unsigned char cmd = 0;

  for (;;) {
    VIN;
    bc = VR;
    while (bc--) {
      switch (parserstate) {
      case PNONE:
	{
	  msg = VR;
	  if (msg & 0b10000000) {
	    cmd = msg & 0b11110000;
	    ch =  msg & 0b00001111;
	    switch (cmd) {
	    case NOTEON:
	      parserstate = PNOTEONDB1;
	      break;
	    case NOTEOFF:
	      parserstate = PNOTEOFFDB1;
	      break;
	    default:
	      break;
	    }
	  }
	}
	break;
      case PNOTEONDB1:
	{
	  db1 = VR7;
	  parserstate = PNOTEONDB2;
	}
	break;
      case PNOTEOFFDB1:
	{
	  db1 = VR7;
	  parserstate = PNOTEOFFDB2;
	}
	break;
      case PNOTEONDB2:
	{
	  handlenoteon(ch, db1, VR7);
	  parserstate = PNONE;
	}
	break;
      case PNOTEOFFDB2:
	{
	  handlenoteoff(ch, db1, VR7);
	  parserstate = PNONE;
	}
	break;
      default:
	parserstate = PNONE;
	break;
      }
    }
    VOUT;
  }
}

void main(void) {
  init();
  midiloop();
}
