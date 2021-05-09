// Copyright 2021 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <stdio.h>
#include "vessel.h"
#include "freq.h"
#include "midi.h"
#include "pbmult.h"
#include "sid.h"

#define SCREENMEM       ((unsigned char*)0x0400)


void initvessel(void) {
  unsigned char *sidreg = NULL;
  unsigned chmask = 0;
  unsigned char i = 0;

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

void applyfreq(unsigned char ch, unsigned char *voicereg) {
  voicereg[0] = voicestate.freq[ch] & 0xff;
  voicereg[1] = voicestate.freq[ch] >> 8;
}

void updatevoice(unsigned char ch, unsigned char *voicereg) {
  applyfreq(ch, voicereg);
  voicereg[2] = voicestate.pwmlo[ch];
  voicereg[3] = voicestate.pwmhi[ch];
  voicereg[5] = voicestate.attackdecay[ch];
  voicereg[6] = voicestate.sustainrelease[ch];
  voicereg[4] = voicestate.control[ch];
}

void defaultvoice(unsigned char ch, unsigned char *voicereg) {
  voicestate.freq[ch] = 0;
  voicestate.pwmlo[ch] = 0;
  voicestate.pwmhi[ch] = 0;
  voicestate.attackdecay[ch] = 0b00000000; // No attack, no decay
  voicestate.sustainrelease[ch] = 0b11110000; // Max sustain, no release
  voicestate.control[ch] = 0x10; // Triangle
  voicestate.pb[ch] = 64; // No pitchbend
  updatevoice(ch, voicereg);
}

void defaultsid(unsigned char ch) {
  unsigned char *sidreg = (unsigned char*)baseptrs.sid[ch];
  sidstate.filterhi[ch] = 0;
  sidstate.filterlo[ch] = 0;
  sidstate.filterresroute[ch] = 0;
  sidstate.filtermodvol[ch] = 0b000011111;
  sidreg[21] = sidstate.filterlo[ch];
  sidreg[22] = sidstate.filterhi[ch];
  sidreg[23] = sidstate.filterresroute[ch];
  sidreg[24] = sidstate.filtermodvol[ch];
}

void initsid(void) {
  unsigned char *sidreg = NULL;
  unsigned char i = 0;
  unsigned char j = 0;

  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)baseptrs.sid[i];
    if (sidreg != NULL) {
      for (j = 0; j < SIDREGSIZE; ++j) {
	sidreg[j] = 0;
      }
      defaultsid(i);
    }
  }
  for (i = 0; i < MIDICHANNELS; ++i) {
    sidreg = (unsigned char*)baseptrs.voice[i];
    if (sidreg != NULL) {
      defaultvoice(i, sidreg);
    }
  }
}

void init(void) {
  initvessel();
  initsid();
}

void gateoff(unsigned char ch, unsigned char *voicereg) {
  voicereg[4] = voicestate.control[ch] & 0b11111110;
}

void handlenoteoff(unsigned char ch, unsigned char *voicereg) {
  gateoff(ch, voicereg);
}

void calcpb(unsigned char ch, unsigned char pb) {
  // TODO: optimize
  unsigned diff = 0;
  if (pb > pbmults) {
    diff = voicestate.pb_highdiff[ch] * pbmult[pb - pbmults];
    voicestate.freq[ch] += diff;
  } else if (pb < pbmults) {
    diff = voicestate.pb_lowdiff[ch] * pbmult[pbmults - pb];
    voicestate.freq[ch] -= diff;
  }
}

void applypb(unsigned char ch) {
  unsigned char p = voicestate.playing[ch];
  unsigned char pb = voicestate.pb[ch];
  voicestate.freq[ch] = ptosf[p];
  if (pb != pbmults) {
    calcpb(ch, pb);
  }
}

void handlenoteon(unsigned char ch, unsigned char *voicereg, unsigned char p, unsigned char v) {
  unsigned char hp = p + pbrange;
  unsigned char lp = 0;

  handlenoteoff(ch, voicereg);
  if (v > 0 && p <= maxpitch) {
    voicestate.playing[ch] = p;
    hp = p + pbrange;
    if (p > maxpitch) {
      hp = maxpitch;
    }
    voicestate.pb_highdiff[ch] = (ptosf[hp] - ptosf[p]) / pbdiv;
    if (p > pbrange) {
      lp = p - pbrange;
    }
    voicestate.pb_lowdiff[ch] = (ptosf[p] - ptosf[lp]) / pbdiv;
    applypb(ch);
    voicestate.control[ch] |= 0x1; // Gate on
    updatevoice(ch, voicereg);
  }
}

#define CCHINIB(x, y) { if (y > 0x0f) { y = 0x0f; } x &= 0x0f; x |= (y << 4); }
#define CCLONIB(x, y) { if (y > 0x0f) { y = 0x0f; } x &= 0xf0; x |= (y & 0x0f); }

void handlecc(unsigned char ch, unsigned char *voicereg, unsigned char cc, unsigned char v) {
  unsigned char *sidreg = (unsigned char*) baseptrs.sid[ch];
  // https://www.midi.org/specifications-old/item/table-3-control-change-messages-data-bytes-2
  switch (cc) {
  case 123:
    gateoff(ch, voicereg);
    break;
  case 121:
    {
      defaultvoice(ch, voicereg);
      if (v) {
	defaultsid(ch);
      }
    }
    break;
  case 89: // filter route
    {
      CCLONIB(sidstate.filterresroute[ch], v);
      sidreg[23] = sidstate.filterresroute[ch];
    }
    break;
  case 87: // filter res
    {
      CCHINIB(sidstate.filterresroute[ch], v);
      sidreg[23] = sidstate.filterresroute[ch];
    }
    break;
  case 86: // filter mode
    {
      CCHINIB(sidstate.filtermodvol[ch], v);
      sidreg[24] = sidstate.filtermodvol[ch];
    }
    break;
  case 85: // control
    {
      voicestate.control[ch] &= 0x1;
      voicestate.control[ch] |= (v << 1);
    }
    break;
  case 75: // decay
    CCLONIB(voicestate.attackdecay[ch], v);
    break;
  case 74: // sustain
    CCHINIB(voicestate.sustainrelease[ch], v);
    break;
  case 73: // attack
    CCHINIB(voicestate.attackdecay[ch], v);
    break;
  case 72: // release
    CCLONIB(voicestate.sustainrelease[ch], v);
    break;
  case 31: // pwm
    {
      voicestate.pwmhi[ch] = (v >> 4);
      voicestate.pwmlo[ch] = ((v & 0b00000111) << 5);
      voicereg[3] = voicestate.pwmhi[ch];
      voicereg[2] = voicestate.pwmlo[ch];
    }
    break;
  case 30: // filter cutoff
    {
      sidreg[22] = (v << 1);
    }
    break;
  case 7: // volume
    {
      CCLONIB(sidstate.filtermodvol[ch], v);
      sidreg[24] = sidstate.filtermodvol[ch];
    }
    break;
  default:
    break;
  }
}

void handlepb(unsigned char ch, unsigned char *voicereg, unsigned char pb) {
  if (pb != voicestate.pb[ch]) {
    voicestate.pb[ch] = pb;
    applypb(ch);
    applyfreq(ch, voicereg);
  }
}

void midiloop(void) {
  register unsigned char bc = 0;
  register unsigned char b = 0;
  unsigned char ch = 0;
  unsigned char *voicereg = NULL;
  register unsigned char i = 0;

  for (;;) {
    VIN;
    bc = VR;
    if (bc) {
      // Rapidly drain Vessel buffer.
      for (i = 0; i < bc; ++i) {
        b = VR;
        buf[i] = b;
        *(SCREENMEM + i) = b;
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
	case CC:
	  handlecc(ch, voicereg, buf[i+1], buf[i+2]);
	  break;
	case PITCHBEND:
	  handlepb(ch, voicereg, buf[i+2]);
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
