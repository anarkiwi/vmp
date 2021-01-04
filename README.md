# vmp
Vessel MIDI Player
==================

VMP turns any C64 with any number of installed SIDs, into a externally MIDI controlled MIDI synthesizer, using the [Vessel interface](https://github.com/anarkiwi/vessel)
(one C64 voice per MIDI channel).

Because the Vessel interface is high performance (large buffers, CIA parallel, MIDI message filtering), the C64 can keep up
with multiple channels (whereas other interfaces support only single channels with strict rate limiting).

To use VMP, install a Vessel interface, and use MIDI channels 1 to 3 in your DAW for the built in SID's voices 1 to 3.

For additional SIDs or to change the default channel mapping, set the SID base addresses and channel mappings in sid.h, and recompile.

## Supported MIDI CC messages
|CC|Default|Description
|--|--|--
|123||Note off
|121||Reset SID and voice CCs (if v > 0, reset just voice CCs)
|89|0|Filter routing
|87|0|Filter resonance
|86|0|Filter mode (most significant 4 bits of main control register)
|85|8|Most significant 7 bits of voice control register.
|75|0|Decay
|74|15|Sustain
|73|0|Attack
|72|0|Release
|31|0|Most significant 7 bits of PWM control registers
|30|0|Most significant 7 bits of filter cutoff
|7|15|Volume of all voices on this SID (least significant 4 bits of main control register)