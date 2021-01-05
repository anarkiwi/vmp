#!/usr/bin/python3

n = 64
d = 100
r = 2

lines = []
lines.append('const unsigned char pbmult[] = {')
for i in range(0, n+1):
    f = int(i / n * d)
    lines.append('\t0x%2.2x, // %u (%u)' % (f, i, f))
lines.append('};')
lines.append('const unsigned char pbmults = sizeof(pbmult);')
lines.append('const unsigned char pbdiv = %u;' % d)
lines.append('const unsigned char pbrange = %u;' % r)

with open('pbmult.h', 'w') as f:
    f.write('\n'.join(lines))


