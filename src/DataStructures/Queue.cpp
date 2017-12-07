/*
  Queue.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/



//LOL


/*
 * hhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyysssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssossssssssooosssssssssssssssssssssssoossssssssssssssssssssssssssssssssssssyyyyyyyyyyyyy
hhhhhhhyyyyyyyyyyyyyyyysssssssssssssssssssssssssooooooooooooooooooooooooooooooooooooooooooooooooo+++++++++ooooo++++++++++++++++ooooooo++++ooo+++++o+++++++++ooooooooooooooooooooooooooooooosssssssssssss
dddddddhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyssssssssssssssssssssssssssssssssooooooooooooooooooooooooooooooooooooooooooooooooooooooo+oooooooooooooooooooooooooooooooooossssssssssssssssssssssyyyyyyyy
mmmmmddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyysssoo++++++++++++++++++oooooo++++++oo++++ooosssssyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhhhhhhhh
mmmmmmmmmmmddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyysso++///:///++++++++++ooossssooooooo++++/++++ooosssyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhddddddddddddddd
mmmmmmmmmdddddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyysoo+//:----::///+++++ooosyyyyyyyyyyssoo+////////+++ossyyhhhhhhhhhhhhhhhhhhhddddddddddddddddddddddd
mmmddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyssooo+/::--::/++ooosssyyhhdddddhhhhhhyyysoo+++//////++ossyyhhhhhhhhhhhhhhhhhddddddddddddddddddddddd
mmddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyysssoo++//:://+ossyyyhhhddmmmmmddddddddhhyyssoo++++++++oossyyyhhhhhhhhhhhhhhhhdddddddddddddddddddddd
mdddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyysssoo++++oosyyyhhhdddmmmmmmdddddmdddddhhhyyyssssssssssyyyyyhhhhhhhhhhhhhhhhhddddddddddddddddddddd
dddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyhhyyyyyyyyyyyyhhhhddddmmmmmNNNNmmmmmmmmdddddhhhhhyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhhhhhhddddddddddddddddd
ddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyhhddddddmmdddddddddddddmmmmNNNNNNNNmmmmdddddddhhhhhhhhhhhhhhhyyyhhhhhhhhhhhhhhhhhhhhhhdddddddddddddddd
dddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyhhdmmNNNNNNNNNmmmddhhhhhhddmmmmmmmmmmddddhhhhhhhhhhhhddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhddddddddddddddd
dddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyhhdmNNMMMMMMNNmmdhhyyssssyyyhhhhdhhhhhhhyyyyysssyyyhhhdddmmmmddddddddhhhhhhhhhhhhhhhhhhhhhhhdddddddddddd
dddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyhdmmNMMMMMMNNmddhyyssoooooooossssyyyysssssssooooooosyyyhddmmmmmmmmddddhhhhhhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyhddmNNMMMMNNmddyysooooo++++++++++ooooooooooo+++++++++oosyhhmmNNNmmmmdddhhyhhhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyhhddmNNNMMMNmmhhyssoo+++++++///////+++++++++/////////////+osshdmNNNNNmmddhhyhhhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyhhdmmNNNMNNmdhyssooo++++++/////////////////:::::::::::::://++syhmmNNNNmmdhyyhhhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyhddmmNNNNNNmdysoo++++++++////////////////:::::::::::::::::///+osyhdmmNmmdhyyyhhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhhdmmNNNNNNmdhyoo+++++++++////////////////::::::::://////:::///++syyddmmmmhhyyyhhhhhhhhhhhhhhhhhhhhhddddddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhddmmNNNNNmmhysooo++oooo+++++++///////////////++++++++++///////++osshhdmmmdhyyyhhhhhhhhhhhhhhhhhhhhhddddddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhddmNNNNmmddhysoooossssssssssooo++++//++++oossssssoooooo+++++++++ooshhdmmmdhhhyyhhhhhhhhhhhhhhhhhhhhddddddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhdmmmNmmmdhhyyssssssssssssyyyyyysoo+++++oossyyysssoo+++++++ooo+++ooshhdmmmddhhyyhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyhdmmmmmmmdhyyyyyyysssssssssyyhhyyss++//++ssssssoooooo+++++osooo+++oshhdmmmddhhyyhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhddmmmmmdhhyyyyyyssssyyyyyyyyyyyyss+////+ossssyyyyyyyyysssossoo+++ooyhdmmmddhhyyhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhddmmmmmdhhyyyyyyyyyyyyhhyyyyssssso+/////+oosyyyyyyyyyyyssssssoo++ooyhdmmmmddhyyhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddmmmmmdhyyssyyyyyyyyyyssssssssoo++/////++osssssssssooooossssooooooyhdmmmmmdhhyhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddmmmmmdhyysyyyyyyssssssoooooooo++///////+++oooooooooooooooooooo+ooyhhddmmddhhhhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdmmmmddhyyssssssooooooo++++oo+++////////////+++++++++++++++++++++oyyhddddddhhhhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddmmmdhyyysssoooo++++++++++oo++///::://////////////////////////++osyhhdddhhhhhhhhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhhhhddmddhyysooo++++/////+++ooooo+++//////////+++++///:::::://////++osyhhddhhyyyyyyhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhhhdddddhyysoo+++/////++oosssyyysso+++++ooooooosooo++///::://///+++osyyhhhyyssssyyhhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddddddhysooo+++++++oossyyhhhhhyyysssyyhhhysssssssso++//////++++oosyyhhyysssyyyyyhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhhhddddhhyssoooooooossssyyhhdmmddddhhhhhhhhysoooosyyysso++++++++ooosssyyysssyyyyyyhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyhhddhhyyysssooossssssssyyhddmmmmmdddhhysso+++++ssyhyysoo+++++oooosssssssssyyyyyyhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyhhhhhhhyyyssssssssysssossyyhhddddddhyss++++///++ooshhhyssoo+++ooooossyysoo+oossyyhhhhhhhhhhhhhhhhhhhhdddddd
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyhhhhhhhyyyssssssyyysssssssyyhhhhhhhyssooooooo++++oshhhyysoo++++oooossssso++++osyyhhhhhhhhhhhhhhhhhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyhhhhhhyyysssssssssyyyyyyyyyyyyyyyssooooossssssssssyhhyysoo++++ooooossso+///+osyhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyhhhhyyysssssoosssyyhhdhhyysssoooo+++oooosyhddhhhyyyysso+++++ooooooo++///++oyyhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssyyhhhhhyysssoooooossyhddddhyyyyyyyyysssssosyhddddhyssooo++++oooooooo++///+oosyhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssyyhhhhhysssooo++++osyhddddddmmmmmmmmmddhyyhdddhhysooooo+++oooooooo+////++ssyhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyysssyyhhhyyssooo++++++syhdmmNNMMMMMMMNNNNNmmmmmdyso+++++++ooooosssoo+////++syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssssyyhhyyssooo++////oshmmNMMMMMMMMMMMMMMMMMNmdo++++++++oooooosssoo+////++syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo++////oshdmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//:/++syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo++////oshdmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooosssoo+//:/++syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo++////oshdmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/+++++ooooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyysooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/+++++ooooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyysooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syyyyyyhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooosssoo+//:///+++///oshhhhhhhhhhhhhhdddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso++::-------..-+syhhhhhhhhhhhhhdddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso+/-.`````   `-+syhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooosss++:.`        `.+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+////+++oooooosss+/:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/::://++ooooosss++:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNdh/:.....--/++ooosss++:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMNmhs-.`    ``-:/+oosss++:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMNNNMMMMNmyo/.`      `..-++osss++:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMNmdddNNMMmd+:.`         `./+osss+/:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMNdsssdNNNdy:.``````     ``:/+ssso/:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh+++hmmmhs-.```...`    ``-:+oss+/:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+hmmdyo-``.----.`    `-:/osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+hdmhs+.``--:::-``   `-:/osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+hddyo/.``-::/:-``   `-:/osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+dddyo/.``-://::``   `.-/osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+dddyo/.``:://::``   `.-:osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+dddy+/.``:://::``   `.-:osso+:.`        `-+shhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+dddyo/.``-://::``   `.-:osso+:.`        `-+syhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+hddyo/.``-://::``   `.-/osso+:.`        `-+syhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh//+yhdyo+..`-::/:-``   `.-/osso+:.`        `-+syhhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh/::+osso+.``--:::-`    `-:/osso+:.`        `-+syhhhhhhhhhdddddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh/-..:/soo.``..---.`   ``-:/osso+:.`        `-+syhhhhhhhhddddddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMmh/-.`-:sss-` ``````    ``:/+ssso+:.`        `-+syhhhhhhhhddddddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMNmyo+/+ohhy/-``         `./+ossso+:.`        `.+shhhdhhhhdddddddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMNmdhhhNmds/:``       `.-/+osss++:.`        `./oyyhhhhhhhhhhdddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMNNNNMNNhso.`      `.-:+oosss++:.`         `-:/+osssyyhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMNNdy:-.````.-:/+ooosss++:.`          ``.--:://++ossyhhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmh+/:---::/++oooosss+/:.`             ```````.-:/oshddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/////+++ooooosss+/:.`                     `.-/syddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/+++ooooooooosss++:.`                     ``./oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++oooooooossso+/..`                    `../oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/+++++ooooooossso+/:-....```              `../oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/+++++ooooooosssoo+/::::::::--.```        `../oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+osso+/:-..```    ``./oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//:/++syhyyyso+//:--.````./oyddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso++//://+syhhhhhhhyyso+/:---:+syddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso++//://+syhhhhhhhhhhyyyssooosyhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso++//://+syhhhhhhhhhhhhhhhhyyhhhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooossso++//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//:/++syhhhhhhhhhhhhhhhhhhhhhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+////+osyhhhhhhhhhhhhhhhhhhhhhddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++///osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+////++syhhhhhhhhhhhhhhhhhhhhdddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyyyyyssooo+++/++oshdmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++/++oshdmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/+++++ooooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyysssssyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+//++++ooooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyysssssyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+//+++++oooooossso++//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssossyhyyyssooo+++//+osydmNMMMMMMMMMMMMMMMMMNmdo+/++++++oooooosssoo+//://+syhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyysssyyyyyyssooo+++///+oyhdmNNNNNNNNNNNNNNNNNmdyo+/++++++oooooosssoo+////+osyhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyysssyyyyyyyssooo++++/+osyhddmmmddddddddddddhyss+///++++oooooosssssso++++ooyyhhhhhhhhhhhhhhhhhhhddddddddddd
ddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyysssyyyyhyyysssoo++++++oossyysssooooooooooo++///////++ooooossssssyyyyssssyyhhhhhhhhhhhhhhhhhhhhhdddddddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssyyyyyyyyyyssoo++++++oosssooo++++////////////////+oooosssssssyyyhhhyyyyhhhhhhhhhhhhhhhhhhhhhhhddddddddd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyhyyyyyyssoo+++++oooooooo++++////////////////+oossssyyysyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddddddddd
ddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyhhhhhhhhyyysso+++++oooooo+++/////////////////++ossssyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhddddddddd
ddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyhhhhhhyyyssooo++++++++++/////////////////++ossssyyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdddddddddd
ddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyhhhhhhhhhyyssoo+++++++++////////////////+oosssssyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdd
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhyyssooooooo++++///+++++++++oosssssssyysyyhhhhhhhhhhhhhddddddddhhhhyyyyyyyyyyyyyyyyyyy
ddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhyyyssssssooooooooooooooosssssssssssyyyyhdddddddddmmmmmmmNmmmmmddhhhhhhhyyyhhhhyyyyy
dddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyysssssyyyssssssssyyyhmmmmmmmmmNNNNNMMMMMMNNNNmmddddhhhhhhhhdddhhh
dddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssssssssyyhdmNNNmmmddmmmmNNNNMMNNNNNmmmddddddddmmmmmmmdd
ddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyssssssssyyyhmNNNNmddhhhdddmmmNNNNmmmmmmdddmmmmmNNNNNNNNm
ddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyhhhhhyyyhhhhhhhhhhhhhhhhhhhhhhhhdddddddddhhhyyyyyyyyysssosydmNNNmdhhyyhhddmmmmmmmmmmmmmmmmNNNNMMMMMMNNN
dddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdddddddhhhhhyyyyyyyysoo++osdmNNNmdhyyyyhdmmNNNNNmmmmmmmmNNNMMMMMMMMMMMM
dddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyhhhhhhhhhhhhhhhhhhhyyyyyyso//:/+odmNNNmdhhyhhdmmNNMMNNNNmmmNNNNMMMMMMMMMMMMMM
ddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdhhhhhhhhhhhhyyyyyyhhhhhhhhhhhhhhhhhhyyysso+/---/+sdmNNNNddhhhdmmNMMMMMNNNNNNNNMMMMMMMMMMMMMMMM
mddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhdddddmmmmmmmmmmddhyyyyhhhhhhyyyhhhhhhhhhhhhhhhhyyysso++:--`../osmNMMNNmmmddmmNNNNNNNNNNNNNMMMMMMMMMMMMMMMMMM
mmddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyhhhhhhhhhdddddmmmmNNNNNNMMMMMNmdyysssyyyyyyyhhhhhhhhhhhhhhhyyysso++/:-.````.+oymNMMNNNNNmmmmNNmmmNNNNNNMMMMMMMMMMMMMMMMMMMM
mmddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhyyyyyyyyyyhhhdddmmNNNNNNNMMMMMMMMMMMMNmdysoooosssyyyyhhhhhhhdddhhysso++/::-.``````.-+shmNNNNNNNNNNNNmmmmmmmNNNNNMMMMMMMMMMMMMMMMMMM
mmmdddddddddddddddddddddddddddddddddhhhhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyyyyyyhhhddmmNNNMMMMMMMMMMMMMMMMMMMMNmdyso++oooossyyyhhddddmddhys+/:--..```   ```.-oyhmNNNNNMMMMMMNNNNmmNNNNNMMMMMMMMMMMMMMMMMMMMM
mmmmddddddddddddddddddddddddddddddddddhhhhhhhhhhhhhhyyyyyyyyyyyyyyyyyyhhhhhdddmmNNNMMMMMMMMMMMMMMMMMMMMMMMNmdyso++++++ooosyhddmmmmmddys+:-.```     ````.:oyhmNMMMMMMMMMMMMMNNNNNNMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmdddddddddddddddddddddddddddddddddddddddhhhhyyyyyyyyyhhhhhhddddddmmmmNNNNNMMMMMMMMMMMMMMMMMMMMNNNNNmdyso++++////+oshmmmmmmdddhyso+:-.``   `````.:sydNNMMMMMMMMMMMMMNNNNMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmddddddddddddddddddddddddddddddddddddddddhhhhddddddddmmmmmmmNNNNNNNNMMMMMMMMMMMMMMMMMMMMMNNmmNNNmmhyo+////:://osdmmmmmdddhhhhhyso+-.```  ```.:sydNNMMMMMMMMMMMMMNNNMMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmdddddddddddddddddddddddddddddddddddmmmmmmmmmmmmmmmNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMNNNmmmmNNNmdyso///::::/+sydmNmmmdddhhhdddmhy+/-``   ``-:shmNNMMMMMMMMMMMMMNMMMMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmmmddddddddddddddddddddddddddddddmmmNNNNNNNNNNNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMNNmmddmmNNNmdyso+//::::/+shdmNNNmmdddhhhdmmNmmhso-.`````-:yhmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmmmmdddddddddddddddddddddddddddddmNNNMMNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNmmmdddmmNNmmhyo///::::::/+ydmmNNNmmdddhddmmNmmmmhy+/:.``.-/ydmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmmmmmmmddddddddddddddddddddddddmmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNmmddddddmNNNdho+/:::::::::/oyddmmmNmmmdddddmmmddhdhhyso--..:/hdNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdddddddddmmmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNmmmdddmmNNNmhs//::://::://+oyhhddmmmmmmmdddddhsoooosyss+/::/+hmNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNmmmNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNNNNNNNNMMNdhssssssssssyyhhddmmmmNNNNNNmmmmddysssyydddhyysyymNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNNMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

 */