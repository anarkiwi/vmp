// Copyright 2021 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define SIDREGSIZE	25
#define SIDVOICEREGSIZE 7
#define SIDVOL(x, v)	x[24] = v

#define BASEPTR(x, y)	{ x + ((y - 1) * SIDVOICEREGSIZE) }

struct {
  unsigned freq[MIDICHANNELS];
  unsigned char control[MIDICHANNELS];
  unsigned char attackdecay[MIDICHANNELS];
  unsigned char sustainrelease[MIDICHANNELS];
  unsigned char pwmhi[MIDICHANNELS];
  unsigned char pwmlo[MIDICHANNELS];
  unsigned char playing[MIDICHANNELS];
  unsigned char pb[MIDICHANNELS];
  unsigned pb_highdiff[MIDICHANNELS];
  unsigned pb_lowdiff[MIDICHANNELS];
} voicestate = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}};

struct {
  unsigned filtermodvol[MIDICHANNELS];
  unsigned char filterhi[MIDICHANNELS];
  unsigned char filterlo[MIDICHANNELS];
  unsigned char filterresroute[MIDICHANNELS];
} sidstate = {{}, {}, {}, {}};

// Configure MIDI channel to SID voice mappings.
#define SID1            ((unsigned char*)0xd400)

#ifdef D420
#define SID2            ((unsigned char*)0xd420)
#endif

#ifdef DE00
#define SID2            ((unsigned char*)0xde00)
#endif

const struct {
  const unsigned char *sid[MIDICHANNELS];
  const unsigned char *voice[MIDICHANNELS];
} baseptrs = {
#ifdef SID2
  {SID1, SID1, SID1, SID2, SID2, SID2},
  {BASEPTR(SID1, 1), BASEPTR(SID1, 2), BASEPTR(SID1, 3), BASEPTR(SID2, 1), BASEPTR(SID2, 2), BASEPTR(SID2, 3)}
#else
  {SID1, SID1, SID1},
  {BASEPTR(SID1, 1), BASEPTR(SID1, 2), BASEPTR(SID1, 3)}
#endif
};
