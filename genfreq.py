#!/usr/bin/python3

a = 440
pal_phi = 985248
ntsc_phi = 1022727
coeff = 256**3 / pal_phi
maxpitch = 127

def ntof(n):
    return (a / 32) * (2 ** ((n - 9) / 12))

def sidf(f):
    return int(coeff * f)

lines = []

lines.append('const unsigned char ptosfhi[] = {')
for n in range(0, maxpitch+1):
    f = ntof(n)
    sf = sidf(f)
    if sf > 2**16:
        sf = 0
    lines.append('\t0x%2.2x, // %.1fHz (%u)' % (sf >> 8, f, n))
lines.append('};')

lines.append('const unsigned char ptosflo[] = {')
for n in range(0, maxpitch+1):
    f = ntof(n)
    sf = sidf(f)
    if sf > 2**16:
        sf = 0
    lines.append('\t0x%2.2x, // %.1fHz (%u)' % (sf & 0xff, f, n))
lines.append('};')

with open('freq.h', 'w') as f:
    f.write('\n'.join(lines))
