#!/usr/bin/python3

a = 440
pal_phi = 985248
ntsc_phi = 1022727
coeff = 256**3 / pal_phi
maxpitch = 127
maxsid = 0

def ntof(n):
    return (a / 32) * (2 ** ((n - 9) / 12))

def sidf(f):
    return int(coeff * f)

lines = []

lines.append('const unsigned ptosf[] = {')
for n in range(0, maxpitch+1):
    f = ntof(n)
    sf = sidf(f)
    if sf > 2**16:
        sf = 0
        if not maxsid:
            maxsid = n - 1
    lines.append('\t0x%2.2x, // %.1fHz (%u)' % (sf, f, n))
lines.append('};')
lines.append('const unsigned char maxpitch = %u;' % maxsid)

with open('freq.h', 'w') as f:
    f.write('\n'.join(lines))
