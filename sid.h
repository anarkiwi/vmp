// Copyright 2021 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define SIDREGSIZE	24
#define SIDVOICEREGSIZE 7
#define SIDVOL(x, v)	(*(x+24) = v)

#define BASEPTR(x, y)	{ x + ((y - 1) * SIDVOICEREGSIZE) }

struct {
  unsigned char control[MIDICHANNELS];
  unsigned char attackdecay[MIDICHANNELS];
  unsigned char sustainrelease[MIDICHANNELS];
  unsigned char lo[MIDICHANNELS];
  unsigned char hi[MIDICHANNELS];
} voicestate = {{}, {}, {}, {}, {}};

// Configure MIDI channel to SID voice mappings.
#define SID1            ((unsigned char*)0xd400)
const struct {
  const unsigned char *sid[MIDICHANNELS];
  const unsigned char *voice[MIDICHANNELS];
} baseptrs = {{ SID1, SID1, SID1 }, { BASEPTR(SID1, 1), BASEPTR(SID1, 2), BASEPTR(SID1, 3) }};
