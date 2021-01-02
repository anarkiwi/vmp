// Copyright 2021 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define SIDREGSIZE	24
#define SIDVOICEREGSIZE 7
#define SIDVOL(x, v)	(*(x+24) = v)

typedef struct {
  unsigned char *sid;
  unsigned char *voice;
} baseptrs;

#define BASEPTRS(x, y) { x, x + ((y - 1) * SIDVOICEREGSIZE) }
#define NULLBASEPTRS	BASEPTRS((unsigned char *)NULL, 0)

// Configure MIDI channel to SID voice mappings.
#define SID1		((unsigned char*)0xd400)
const baseptrs voicebase[] = {
			      BASEPTRS(SID1, 1), // SID1, voice 1 is on channel 1
			      BASEPTRS(SID1, 2), // SID1, voice 2 is on channel 2
			      BASEPTRS(SID1, 3), // SID1, voice 3 is on channel 3
			      NULLBASEPTRS, // remaining MIDI channels not used.
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
			      NULLBASEPTRS,
};
