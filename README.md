# vmp
Vessel MIDI Player
==================

VMP turns any C64 with any number of installed SIDs, into a externally MIDI controlled MIDI synthesizer, using the [Vessel interface](https://github.com/anarkiwi/vessel)
(one C64 voice per MIDI channel).

Because the Vessel interface is high performance (large buffers, CIA parallel, MIDI message filtering), the C64 can keep up
with multiple channels (whereas other interfaces support only single channels with strict rate limiting).

To use VMP, install a Vessel interface, and use MIDI channels 1 to 3 in your DAW for the built in SID's voices 1 to 3.

For additional SIDs or to change the default channel mapping, set the SID base addresses and channel mappings in sid.h, and recompile.
