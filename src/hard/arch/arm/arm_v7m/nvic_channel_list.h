/*
  nvic_channel_list.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

#ifdef channel
//Channel 0 is the initial stack pointer;
//Channel 1 is the program entry point, not reconfigurable;
#if (NB_INTERRUPTS > 2)
channel(2)
#endif
#if (NB_INTERRUPTS > 3)
channel(3)
#endif
#if (NB_INTERRUPTS > 4)
channel(4)
#endif
#if (NB_INTERRUPTS > 5)
channel(5)
#endif
#if (NB_INTERRUPTS > 6)
channel(6)
#endif
#if (NB_INTERRUPTS > 7)
channel(7)
#endif
#if (NB_INTERRUPTS > 8)
channel(8)
#endif
#if (NB_INTERRUPTS > 9)
channel(9)
#endif
#if (NB_INTERRUPTS > 10)
channel(10)
#endif
#if (NB_INTERRUPTS > 11)
channel(11)
#endif
#if (NB_INTERRUPTS > 12)
channel(12)
#endif
#if (NB_INTERRUPTS > 13)
channel(13)
#endif
#if (NB_INTERRUPTS > 14)
channel(14)
#endif
#if (NB_INTERRUPTS > 15)
channel(15)
#endif
#if (NB_INTERRUPTS > 16)
channel(16)
#endif
#if (NB_INTERRUPTS > 17)
channel(17)
#endif
#if (NB_INTERRUPTS > 18)
channel(18)
#endif
#if (NB_INTERRUPTS > 19)
channel(19)
#endif
#if (NB_INTERRUPTS > 20)
channel(20)
#endif
#if (NB_INTERRUPTS > 21)
channel(21)
#endif
#if (NB_INTERRUPTS > 22)
channel(22)
#endif
#if (NB_INTERRUPTS > 23)
channel(23)
#endif
#if (NB_INTERRUPTS > 24)
channel(24)
#endif
#if (NB_INTERRUPTS > 25)
channel(25)
#endif
#if (NB_INTERRUPTS > 26)
channel(26)
#endif
#if (NB_INTERRUPTS > 27)
channel(27)
#endif
#if (NB_INTERRUPTS > 28)
channel(28)
#endif
#if (NB_INTERRUPTS > 29)
channel(29)
#endif
#if (NB_INTERRUPTS > 30)
channel(30)
#endif
#if (NB_INTERRUPTS > 31)
channel(31)
#endif
#if (NB_INTERRUPTS > 32)
channel(32)
#endif
#if (NB_INTERRUPTS > 33)
channel(33)
#endif
#if (NB_INTERRUPTS > 34)
channel(34)
#endif
#if (NB_INTERRUPTS > 35)
channel(35)
#endif
#if (NB_INTERRUPTS > 36)
channel(36)
#endif
#if (NB_INTERRUPTS > 37)
channel(37)
#endif
#if (NB_INTERRUPTS > 38)
channel(38)
#endif
#if (NB_INTERRUPTS > 39)
channel(39)
#endif
#if (NB_INTERRUPTS > 40)
channel(40)
#endif
#if (NB_INTERRUPTS > 41)
channel(41)
#endif
#if (NB_INTERRUPTS > 42)
channel(42)
#endif
#if (NB_INTERRUPTS > 43)
channel(43)
#endif
#if (NB_INTERRUPTS > 44)
channel(44)
#endif
#if (NB_INTERRUPTS > 45)
channel(45)
#endif
#if (NB_INTERRUPTS > 46)
channel(46)
#endif
#if (NB_INTERRUPTS > 47)
channel(47)
#endif
#if (NB_INTERRUPTS > 48)
channel(48)
#endif
#if (NB_INTERRUPTS > 49)
channel(49)
#endif
#if (NB_INTERRUPTS > 50)
channel(50)
#endif
#if (NB_INTERRUPTS > 51)
channel(51)
#endif
#if (NB_INTERRUPTS > 52)
channel(52)
#endif
#if (NB_INTERRUPTS > 53)
channel(53)
#endif
#if (NB_INTERRUPTS > 54)
channel(54)
#endif
#if (NB_INTERRUPTS > 55)
channel(55)
#endif
#if (NB_INTERRUPTS > 56)
channel(56)
#endif
#if (NB_INTERRUPTS > 57)
channel(57)
#endif
#if (NB_INTERRUPTS > 58)
channel(58)
#endif
#if (NB_INTERRUPTS > 59)
channel(59)
#endif
#if (NB_INTERRUPTS > 60)
channel(60)
#endif
#if (NB_INTERRUPTS > 61)
channel(61)
#endif
#if (NB_INTERRUPTS > 62)
channel(62)
#endif
#if (NB_INTERRUPTS > 63)
channel(63)
#endif
#if (NB_INTERRUPTS > 64)
channel(64)
#endif
#if (NB_INTERRUPTS > 65)
channel(65)
#endif
#if (NB_INTERRUPTS > 66)
channel(66)
#endif
#if (NB_INTERRUPTS > 67)
channel(67)
#endif
#if (NB_INTERRUPTS > 68)
channel(68)
#endif
#if (NB_INTERRUPTS > 69)
channel(69)
#endif
#if (NB_INTERRUPTS > 70)
channel(70)
#endif
#if (NB_INTERRUPTS > 71)
channel(71)
#endif
#if (NB_INTERRUPTS > 72)
channel(72)
#endif
#if (NB_INTERRUPTS > 73)
channel(73)
#endif
#if (NB_INTERRUPTS > 74)
channel(74)
#endif
#if (NB_INTERRUPTS > 75)
channel(75)
#endif
#if (NB_INTERRUPTS > 76)
channel(76)
#endif
#if (NB_INTERRUPTS > 77)
channel(77)
#endif
#if (NB_INTERRUPTS > 78)
channel(78)
#endif
#if (NB_INTERRUPTS > 79)
channel(79)
#endif
#if (NB_INTERRUPTS > 80)
channel(80)
#endif
#if (NB_INTERRUPTS > 81)
channel(81)
#endif
#if (NB_INTERRUPTS > 82)
channel(82)
#endif
#if (NB_INTERRUPTS > 83)
channel(83)
#endif
#if (NB_INTERRUPTS > 84)
channel(84)
#endif
#if (NB_INTERRUPTS > 85)
channel(85)
#endif
#if (NB_INTERRUPTS > 86)
channel(86)
#endif
#if (NB_INTERRUPTS > 87)
channel(87)
#endif
#if (NB_INTERRUPTS > 88)
channel(88)
#endif
#if (NB_INTERRUPTS > 89)
channel(89)
#endif
#if (NB_INTERRUPTS > 90)
channel(90)
#endif
#if (NB_INTERRUPTS > 91)
channel(91)
#endif
#if (NB_INTERRUPTS > 92)
channel(92)
#endif
#if (NB_INTERRUPTS > 93)
channel(93)
#endif
#if (NB_INTERRUPTS > 94)
channel(94)
#endif
#if (NB_INTERRUPTS > 95)
channel(95)
#endif
#if (NB_INTERRUPTS > 96)
channel(96)
#endif
#if (NB_INTERRUPTS > 97)
channel(97)
#endif
#if (NB_INTERRUPTS > 98)
channel(98)
#endif
#if (NB_INTERRUPTS > 99)
channel(99)
#endif
#if (NB_INTERRUPTS > 100)
channel(100)
#endif
#if (NB_INTERRUPTS > 101)
channel(101)
#endif
#if (NB_INTERRUPTS > 102)
channel(102)
#endif
#if (NB_INTERRUPTS > 103)
channel(103)
#endif
#if (NB_INTERRUPTS > 104)
channel(104)
#endif
#if (NB_INTERRUPTS > 105)
channel(105)
#endif
#if (NB_INTERRUPTS > 106)
channel(106)
#endif
#if (NB_INTERRUPTS > 107)
channel(107)
#endif
#if (NB_INTERRUPTS > 108)
channel(108)
#endif
#if (NB_INTERRUPTS > 109)
channel(109)
#endif
#if (NB_INTERRUPTS > 110)
channel(110)
#endif
#if (NB_INTERRUPTS > 111)
channel(111)
#endif
#if (NB_INTERRUPTS > 112)
channel(112)
#endif
#if (NB_INTERRUPTS > 113)
channel(113)
#endif
#if (NB_INTERRUPTS > 114)
channel(114)
#endif
#if (NB_INTERRUPTS > 115)
channel(115)
#endif
#if (NB_INTERRUPTS > 116)
channel(116)
#endif
#if (NB_INTERRUPTS > 117)
channel(117)
#endif
#if (NB_INTERRUPTS > 118)
channel(118)
#endif
#if (NB_INTERRUPTS > 119)
channel(119)
#endif
#if (NB_INTERRUPTS > 120)
channel(120)
#endif
#if (NB_INTERRUPTS > 121)
channel(121)
#endif
#if (NB_INTERRUPTS > 122)
channel(122)
#endif
#if (NB_INTERRUPTS > 123)
channel(123)
#endif
#if (NB_INTERRUPTS > 124)
channel(124)
#endif
#if (NB_INTERRUPTS > 125)
channel(125)
#endif
#if (NB_INTERRUPTS > 126)
channel(126)
#endif
#if (NB_INTERRUPTS > 127)
channel(127)
#endif
#if (NB_INTERRUPTS > 128)
channel(128)
#endif
#if (NB_INTERRUPTS > 129)
channel(129)
#endif
#if (NB_INTERRUPTS > 130)
channel(130)
#endif
#if (NB_INTERRUPTS > 131)
channel(131)
#endif
#if (NB_INTERRUPTS > 132)
channel(132)
#endif
#if (NB_INTERRUPTS > 133)
channel(133)
#endif
#if (NB_INTERRUPTS > 134)
channel(134)
#endif
#if (NB_INTERRUPTS > 135)
channel(135)
#endif
#if (NB_INTERRUPTS > 136)
channel(136)
#endif
#if (NB_INTERRUPTS > 137)
channel(137)
#endif
#if (NB_INTERRUPTS > 138)
channel(138)
#endif
#if (NB_INTERRUPTS > 139)
channel(139)
#endif
#if (NB_INTERRUPTS > 140)
channel(140)
#endif
#if (NB_INTERRUPTS > 141)
channel(141)
#endif
#if (NB_INTERRUPTS > 142)
channel(142)
#endif
#if (NB_INTERRUPTS > 143)
channel(143)
#endif
#if (NB_INTERRUPTS > 144)
channel(144)
#endif
#if (NB_INTERRUPTS > 145)
channel(145)
#endif
#if (NB_INTERRUPTS > 146)
channel(146)
#endif
#if (NB_INTERRUPTS > 147)
channel(147)
#endif
#if (NB_INTERRUPTS > 148)
channel(148)
#endif
#if (NB_INTERRUPTS > 149)
channel(149)
#endif
#if (NB_INTERRUPTS > 150)
channel(150)
#endif
#if (NB_INTERRUPTS > 151)
channel(151)
#endif
#if (NB_INTERRUPTS > 152)
channel(152)
#endif
#if (NB_INTERRUPTS > 153)
channel(153)
#endif
#if (NB_INTERRUPTS > 154)
channel(154)
#endif
#if (NB_INTERRUPTS > 155)
channel(155)
#endif
#if (NB_INTERRUPTS > 156)
channel(156)
#endif
#if (NB_INTERRUPTS > 157)
channel(157)
#endif
#if (NB_INTERRUPTS > 158)
channel(158)
#endif
#if (NB_INTERRUPTS > 159)
channel(159)
#endif
#if (NB_INTERRUPTS > 160)
channel(160)
#endif
#if (NB_INTERRUPTS > 161)
channel(161)
#endif
#if (NB_INTERRUPTS > 162)
channel(162)
#endif
#if (NB_INTERRUPTS > 163)
channel(163)
#endif
#if (NB_INTERRUPTS > 164)
channel(164)
#endif
#if (NB_INTERRUPTS > 165)
channel(165)
#endif
#if (NB_INTERRUPTS > 166)
channel(166)
#endif
#if (NB_INTERRUPTS > 167)
channel(167)
#endif
#if (NB_INTERRUPTS > 168)
channel(168)
#endif
#if (NB_INTERRUPTS > 169)
channel(169)
#endif
#if (NB_INTERRUPTS > 170)
channel(170)
#endif
#if (NB_INTERRUPTS > 171)
channel(171)
#endif
#if (NB_INTERRUPTS > 172)
channel(172)
#endif
#if (NB_INTERRUPTS > 173)
channel(173)
#endif
#if (NB_INTERRUPTS > 174)
channel(174)
#endif
#if (NB_INTERRUPTS > 175)
channel(175)
#endif
#if (NB_INTERRUPTS > 176)
channel(176)
#endif
#if (NB_INTERRUPTS > 177)
channel(177)
#endif
#if (NB_INTERRUPTS > 178)
channel(178)
#endif
#if (NB_INTERRUPTS > 179)
channel(179)
#endif
#if (NB_INTERRUPTS > 180)
channel(180)
#endif
#if (NB_INTERRUPTS > 181)
channel(181)
#endif
#if (NB_INTERRUPTS > 182)
channel(182)
#endif
#if (NB_INTERRUPTS > 183)
channel(183)
#endif
#if (NB_INTERRUPTS > 184)
channel(184)
#endif
#if (NB_INTERRUPTS > 185)
channel(185)
#endif
#if (NB_INTERRUPTS > 186)
channel(186)
#endif
#if (NB_INTERRUPTS > 187)
channel(187)
#endif
#if (NB_INTERRUPTS > 188)
channel(188)
#endif
#if (NB_INTERRUPTS > 189)
channel(189)
#endif
#if (NB_INTERRUPTS > 190)
channel(190)
#endif
#if (NB_INTERRUPTS > 191)
channel(191)
#endif
#if (NB_INTERRUPTS > 192)
channel(192)
#endif
#if (NB_INTERRUPTS > 193)
channel(193)
#endif
#if (NB_INTERRUPTS > 194)
channel(194)
#endif
#if (NB_INTERRUPTS > 195)
channel(195)
#endif
#if (NB_INTERRUPTS > 196)
channel(196)
#endif
#if (NB_INTERRUPTS > 197)
channel(197)
#endif
#if (NB_INTERRUPTS > 198)
channel(198)
#endif
#if (NB_INTERRUPTS > 199)
channel(199)
#endif
#if (NB_INTERRUPTS > 200)
channel(200)
#endif
#if (NB_INTERRUPTS > 201)
channel(201)
#endif
#if (NB_INTERRUPTS > 202)
channel(202)
#endif
#if (NB_INTERRUPTS > 203)
channel(203)
#endif
#if (NB_INTERRUPTS > 204)
channel(204)
#endif
#if (NB_INTERRUPTS > 205)
channel(205)
#endif
#if (NB_INTERRUPTS > 206)
channel(206)
#endif
#if (NB_INTERRUPTS > 207)
channel(207)
#endif
#if (NB_INTERRUPTS > 208)
channel(208)
#endif
#if (NB_INTERRUPTS > 209)
channel(209)
#endif
#if (NB_INTERRUPTS > 210)
channel(210)
#endif
#if (NB_INTERRUPTS > 211)
channel(211)
#endif
#if (NB_INTERRUPTS > 212)
channel(212)
#endif
#if (NB_INTERRUPTS > 213)
channel(213)
#endif
#if (NB_INTERRUPTS > 214)
channel(214)
#endif
#if (NB_INTERRUPTS > 215)
channel(215)
#endif
#if (NB_INTERRUPTS > 216)
channel(216)
#endif
#if (NB_INTERRUPTS > 217)
channel(217)
#endif
#if (NB_INTERRUPTS > 218)
channel(218)
#endif
#if (NB_INTERRUPTS > 219)
channel(219)
#endif
#if (NB_INTERRUPTS > 220)
channel(220)
#endif
#if (NB_INTERRUPTS > 221)
channel(221)
#endif
#if (NB_INTERRUPTS > 222)
channel(222)
#endif
#if (NB_INTERRUPTS > 223)
channel(223)
#endif
#if (NB_INTERRUPTS > 224)
channel(224)
#endif
#if (NB_INTERRUPTS > 225)
channel(225)
#endif
#if (NB_INTERRUPTS > 226)
channel(226)
#endif
#if (NB_INTERRUPTS > 227)
channel(227)
#endif
#if (NB_INTERRUPTS > 228)
channel(228)
#endif
#if (NB_INTERRUPTS > 229)
channel(229)
#endif
#if (NB_INTERRUPTS > 230)
channel(230)
#endif
#if (NB_INTERRUPTS > 231)
channel(231)
#endif
#if (NB_INTERRUPTS > 232)
channel(232)
#endif
#if (NB_INTERRUPTS > 233)
channel(233)
#endif
#if (NB_INTERRUPTS > 234)
channel(234)
#endif
#if (NB_INTERRUPTS > 235)
channel(235)
#endif
#if (NB_INTERRUPTS > 236)
channel(236)
#endif
#if (NB_INTERRUPTS > 237)
channel(237)
#endif
#if (NB_INTERRUPTS > 238)
channel(238)
#endif
#if (NB_INTERRUPTS > 239)
channel(239)
#endif
#if (NB_INTERRUPTS > 240)
channel(240)
#endif
#if (NB_INTERRUPTS > 241)
channel(241)
#endif
#if (NB_INTERRUPTS > 242)
channel(242)
#endif
#if (NB_INTERRUPTS > 243)
channel(243)
#endif
#if (NB_INTERRUPTS > 244)
channel(244)
#endif
#if (NB_INTERRUPTS > 245)
channel(245)
#endif
#if (NB_INTERRUPTS > 246)
channel(246)
#endif
#if (NB_INTERRUPTS > 247)
channel(247)
#endif
#if (NB_INTERRUPTS > 248)
channel(248)
#endif
#if (NB_INTERRUPTS > 249)
channel(249)
#endif
#if (NB_INTERRUPTS > 250)
channel(250)
#endif
#if (NB_INTERRUPTS > 251)
channel(251)
#endif
#if (NB_INTERRUPTS > 252)
channel(252)
#endif
#if (NB_INTERRUPTS > 253)
channel(253)
#endif
#if (NB_INTERRUPTS > 254)
channel(254)
#endif
#if (NB_INTERRUPTS > 255)
channel(255)
#endif
#if (NB_INTERRUPTS > 256)
channel(256)
#endif
#if (NB_INTERRUPTS > 257)
channel(257)
#endif
#if (NB_INTERRUPTS > 258)
channel(258)
#endif
#if (NB_INTERRUPTS > 259)
channel(259)
#endif
#if (NB_INTERRUPTS > 260)
channel(260)
#endif
#if (NB_INTERRUPTS > 261)
channel(261)
#endif
#if (NB_INTERRUPTS > 262)
channel(262)
#endif
#if (NB_INTERRUPTS > 263)
channel(263)
#endif
#if (NB_INTERRUPTS > 264)
channel(264)
#endif
#if (NB_INTERRUPTS > 265)
channel(265)
#endif
#if (NB_INTERRUPTS > 266)
channel(266)
#endif
#if (NB_INTERRUPTS > 267)
channel(267)
#endif
#if (NB_INTERRUPTS > 268)
channel(268)
#endif
#if (NB_INTERRUPTS > 269)
channel(269)
#endif
#if (NB_INTERRUPTS > 270)
channel(270)
#endif
#if (NB_INTERRUPTS > 271)
channel(271)
#endif
#if (NB_INTERRUPTS > 272)
channel(272)
#endif
#if (NB_INTERRUPTS > 273)
channel(273)
#endif
#if (NB_INTERRUPTS > 274)
channel(274)
#endif
#if (NB_INTERRUPTS > 275)
channel(275)
#endif
#if (NB_INTERRUPTS > 276)
channel(276)
#endif
#if (NB_INTERRUPTS > 277)
channel(277)
#endif
#if (NB_INTERRUPTS > 278)
channel(278)
#endif
#if (NB_INTERRUPTS > 279)
channel(279)
#endif
#if (NB_INTERRUPTS > 280)
channel(280)
#endif
#if (NB_INTERRUPTS > 281)
channel(281)
#endif
#if (NB_INTERRUPTS > 282)
channel(282)
#endif
#if (NB_INTERRUPTS > 283)
channel(283)
#endif
#if (NB_INTERRUPTS > 284)
channel(284)
#endif
#if (NB_INTERRUPTS > 285)
channel(285)
#endif
#if (NB_INTERRUPTS > 286)
channel(286)
#endif
#if (NB_INTERRUPTS > 287)
channel(287)
#endif
#if (NB_INTERRUPTS > 288)
channel(288)
#endif
#if (NB_INTERRUPTS > 289)
channel(289)
#endif
#if (NB_INTERRUPTS > 290)
channel(290)
#endif
#if (NB_INTERRUPTS > 291)
channel(291)
#endif
#if (NB_INTERRUPTS > 292)
channel(292)
#endif
#if (NB_INTERRUPTS > 293)
channel(293)
#endif
#if (NB_INTERRUPTS > 294)
channel(294)
#endif
#if (NB_INTERRUPTS > 295)
channel(295)
#endif
#if (NB_INTERRUPTS > 296)
channel(296)
#endif
#if (NB_INTERRUPTS > 297)
channel(297)
#endif
#if (NB_INTERRUPTS > 298)
channel(298)
#endif
#if (NB_INTERRUPTS > 299)
channel(299)
#endif
#if (NB_INTERRUPTS > 300)
channel(300)
#endif
#if (NB_INTERRUPTS > 301)
channel(301)
#endif
#if (NB_INTERRUPTS > 302)
channel(302)
#endif
#if (NB_INTERRUPTS > 303)
channel(303)
#endif
#if (NB_INTERRUPTS > 304)
channel(304)
#endif
#if (NB_INTERRUPTS > 305)
channel(305)
#endif
#if (NB_INTERRUPTS > 306)
channel(306)
#endif
#if (NB_INTERRUPTS > 307)
channel(307)
#endif
#if (NB_INTERRUPTS > 308)
channel(308)
#endif
#if (NB_INTERRUPTS > 309)
channel(309)
#endif
#if (NB_INTERRUPTS > 310)
channel(310)
#endif
#if (NB_INTERRUPTS > 311)
channel(311)
#endif
#if (NB_INTERRUPTS > 312)
channel(312)
#endif
#if (NB_INTERRUPTS > 313)
channel(313)
#endif
#if (NB_INTERRUPTS > 314)
channel(314)
#endif
#if (NB_INTERRUPTS > 315)
channel(315)
#endif
#if (NB_INTERRUPTS > 316)
channel(316)
#endif
#if (NB_INTERRUPTS > 317)
channel(317)
#endif
#if (NB_INTERRUPTS > 318)
channel(318)
#endif
#if (NB_INTERRUPTS > 319)
channel(319)
#endif
#if (NB_INTERRUPTS > 320)
channel(320)
#endif
#if (NB_INTERRUPTS > 321)
channel(321)
#endif
#if (NB_INTERRUPTS > 322)
channel(322)
#endif
#if (NB_INTERRUPTS > 323)
channel(323)
#endif
#if (NB_INTERRUPTS > 324)
channel(324)
#endif
#if (NB_INTERRUPTS > 325)
channel(325)
#endif
#if (NB_INTERRUPTS > 326)
channel(326)
#endif
#if (NB_INTERRUPTS > 327)
channel(327)
#endif
#if (NB_INTERRUPTS > 328)
channel(328)
#endif
#if (NB_INTERRUPTS > 329)
channel(329)
#endif
#if (NB_INTERRUPTS > 330)
channel(330)
#endif
#if (NB_INTERRUPTS > 331)
channel(331)
#endif
#if (NB_INTERRUPTS > 332)
channel(332)
#endif
#if (NB_INTERRUPTS > 333)
channel(333)
#endif
#if (NB_INTERRUPTS > 334)
channel(334)
#endif
#if (NB_INTERRUPTS > 335)
channel(335)
#endif
#if (NB_INTERRUPTS > 336)
channel(336)
#endif
#if (NB_INTERRUPTS > 337)
channel(337)
#endif
#if (NB_INTERRUPTS > 338)
channel(338)
#endif
#if (NB_INTERRUPTS > 339)
channel(339)
#endif
#if (NB_INTERRUPTS > 340)
channel(340)
#endif
#if (NB_INTERRUPTS > 341)
channel(341)
#endif
#if (NB_INTERRUPTS > 342)
channel(342)
#endif
#if (NB_INTERRUPTS > 343)
channel(343)
#endif
#if (NB_INTERRUPTS > 344)
channel(344)
#endif
#if (NB_INTERRUPTS > 345)
channel(345)
#endif
#if (NB_INTERRUPTS > 346)
channel(346)
#endif
#if (NB_INTERRUPTS > 347)
channel(347)
#endif
#if (NB_INTERRUPTS > 348)
channel(348)
#endif
#if (NB_INTERRUPTS > 349)
channel(349)
#endif
#if (NB_INTERRUPTS > 350)
channel(350)
#endif
#if (NB_INTERRUPTS > 351)
channel(351)
#endif
#if (NB_INTERRUPTS > 352)
channel(352)
#endif
#if (NB_INTERRUPTS > 353)
channel(353)
#endif
#if (NB_INTERRUPTS > 354)
channel(354)
#endif
#if (NB_INTERRUPTS > 355)
channel(355)
#endif
#if (NB_INTERRUPTS > 356)
channel(356)
#endif
#if (NB_INTERRUPTS > 357)
channel(357)
#endif
#if (NB_INTERRUPTS > 358)
channel(358)
#endif
#if (NB_INTERRUPTS > 359)
channel(359)
#endif
#if (NB_INTERRUPTS > 360)
channel(360)
#endif
#if (NB_INTERRUPTS > 361)
channel(361)
#endif
#if (NB_INTERRUPTS > 362)
channel(362)
#endif
#if (NB_INTERRUPTS > 363)
channel(363)
#endif
#if (NB_INTERRUPTS > 364)
channel(364)
#endif
#if (NB_INTERRUPTS > 365)
channel(365)
#endif
#if (NB_INTERRUPTS > 366)
channel(366)
#endif
#if (NB_INTERRUPTS > 367)
channel(367)
#endif
#if (NB_INTERRUPTS > 368)
channel(368)
#endif
#if (NB_INTERRUPTS > 369)
channel(369)
#endif
#if (NB_INTERRUPTS > 370)
channel(370)
#endif
#if (NB_INTERRUPTS > 371)
channel(371)
#endif
#if (NB_INTERRUPTS > 372)
channel(372)
#endif
#if (NB_INTERRUPTS > 373)
channel(373)
#endif
#if (NB_INTERRUPTS > 374)
channel(374)
#endif
#if (NB_INTERRUPTS > 375)
channel(375)
#endif
#if (NB_INTERRUPTS > 376)
channel(376)
#endif
#if (NB_INTERRUPTS > 377)
channel(377)
#endif
#if (NB_INTERRUPTS > 378)
channel(378)
#endif
#if (NB_INTERRUPTS > 379)
channel(379)
#endif
#if (NB_INTERRUPTS > 380)
channel(380)
#endif
#if (NB_INTERRUPTS > 381)
channel(381)
#endif
#if (NB_INTERRUPTS > 382)
channel(382)
#endif
#if (NB_INTERRUPTS > 383)
channel(383)
#endif
#if (NB_INTERRUPTS > 384)
channel(384)
#endif
#if (NB_INTERRUPTS > 385)
channel(385)
#endif
#if (NB_INTERRUPTS > 386)
channel(386)
#endif
#if (NB_INTERRUPTS > 387)
channel(387)
#endif
#if (NB_INTERRUPTS > 388)
channel(388)
#endif
#if (NB_INTERRUPTS > 389)
channel(389)
#endif
#if (NB_INTERRUPTS > 390)
channel(390)
#endif
#if (NB_INTERRUPTS > 391)
channel(391)
#endif
#if (NB_INTERRUPTS > 392)
channel(392)
#endif
#if (NB_INTERRUPTS > 393)
channel(393)
#endif
#if (NB_INTERRUPTS > 394)
channel(394)
#endif
#if (NB_INTERRUPTS > 395)
channel(395)
#endif
#if (NB_INTERRUPTS > 396)
channel(396)
#endif
#if (NB_INTERRUPTS > 397)
channel(397)
#endif
#if (NB_INTERRUPTS > 398)
channel(398)
#endif
#if (NB_INTERRUPTS > 399)
channel(399)
#endif
#if (NB_INTERRUPTS > 400)
channel(400)
#endif
#if (NB_INTERRUPTS > 401)
channel(401)
#endif
#if (NB_INTERRUPTS > 402)
channel(402)
#endif
#if (NB_INTERRUPTS > 403)
channel(403)
#endif
#if (NB_INTERRUPTS > 404)
channel(404)
#endif
#if (NB_INTERRUPTS > 405)
channel(405)
#endif
#if (NB_INTERRUPTS > 406)
channel(406)
#endif
#if (NB_INTERRUPTS > 407)
channel(407)
#endif
#if (NB_INTERRUPTS > 408)
channel(408)
#endif
#if (NB_INTERRUPTS > 409)
channel(409)
#endif
#if (NB_INTERRUPTS > 410)
channel(410)
#endif
#if (NB_INTERRUPTS > 411)
channel(411)
#endif
#if (NB_INTERRUPTS > 412)
channel(412)
#endif
#if (NB_INTERRUPTS > 413)
channel(413)
#endif
#if (NB_INTERRUPTS > 414)
channel(414)
#endif
#if (NB_INTERRUPTS > 415)
channel(415)
#endif
#if (NB_INTERRUPTS > 416)
channel(416)
#endif
#if (NB_INTERRUPTS > 417)
channel(417)
#endif
#if (NB_INTERRUPTS > 418)
channel(418)
#endif
#if (NB_INTERRUPTS > 419)
channel(419)
#endif
#if (NB_INTERRUPTS > 420)
channel(420)
#endif
#if (NB_INTERRUPTS > 421)
channel(421)
#endif
#if (NB_INTERRUPTS > 422)
channel(422)
#endif
#if (NB_INTERRUPTS > 423)
channel(423)
#endif
#if (NB_INTERRUPTS > 424)
channel(424)
#endif
#if (NB_INTERRUPTS > 425)
channel(425)
#endif
#if (NB_INTERRUPTS > 426)
channel(426)
#endif
#if (NB_INTERRUPTS > 427)
channel(427)
#endif
#if (NB_INTERRUPTS > 428)
channel(428)
#endif
#if (NB_INTERRUPTS > 429)
channel(429)
#endif
#if (NB_INTERRUPTS > 430)
channel(430)
#endif
#if (NB_INTERRUPTS > 431)
channel(431)
#endif
#if (NB_INTERRUPTS > 432)
channel(432)
#endif
#if (NB_INTERRUPTS > 433)
channel(433)
#endif
#if (NB_INTERRUPTS > 434)
channel(434)
#endif
#if (NB_INTERRUPTS > 435)
channel(435)
#endif
#if (NB_INTERRUPTS > 436)
channel(436)
#endif
#if (NB_INTERRUPTS > 437)
channel(437)
#endif
#if (NB_INTERRUPTS > 438)
channel(438)
#endif
#if (NB_INTERRUPTS > 439)
channel(439)
#endif
#if (NB_INTERRUPTS > 440)
channel(440)
#endif
#if (NB_INTERRUPTS > 441)
channel(441)
#endif
#if (NB_INTERRUPTS > 442)
channel(442)
#endif
#if (NB_INTERRUPTS > 443)
channel(443)
#endif
#if (NB_INTERRUPTS > 444)
channel(444)
#endif
#if (NB_INTERRUPTS > 445)
channel(445)
#endif
#if (NB_INTERRUPTS > 446)
channel(446)
#endif
#if (NB_INTERRUPTS > 447)
channel(447)
#endif
#if (NB_INTERRUPTS > 448)
channel(448)
#endif
#if (NB_INTERRUPTS > 449)
channel(449)
#endif
#if (NB_INTERRUPTS > 450)
channel(450)
#endif
#if (NB_INTERRUPTS > 451)
channel(451)
#endif
#if (NB_INTERRUPTS > 452)
channel(452)
#endif
#if (NB_INTERRUPTS > 453)
channel(453)
#endif
#if (NB_INTERRUPTS > 454)
channel(454)
#endif
#if (NB_INTERRUPTS > 455)
channel(455)
#endif
#if (NB_INTERRUPTS > 456)
channel(456)
#endif
#if (NB_INTERRUPTS > 457)
channel(457)
#endif
#if (NB_INTERRUPTS > 458)
channel(458)
#endif
#if (NB_INTERRUPTS > 459)
channel(459)
#endif
#if (NB_INTERRUPTS > 460)
channel(460)
#endif
#if (NB_INTERRUPTS > 461)
channel(461)
#endif
#if (NB_INTERRUPTS > 462)
channel(462)
#endif
#if (NB_INTERRUPTS > 463)
channel(463)
#endif
#if (NB_INTERRUPTS > 464)
channel(464)
#endif
#if (NB_INTERRUPTS > 465)
channel(465)
#endif
#if (NB_INTERRUPTS > 466)
channel(466)
#endif
#if (NB_INTERRUPTS > 467)
channel(467)
#endif
#if (NB_INTERRUPTS > 468)
channel(468)
#endif
#if (NB_INTERRUPTS > 469)
channel(469)
#endif
#if (NB_INTERRUPTS > 470)
channel(470)
#endif
#if (NB_INTERRUPTS > 471)
channel(471)
#endif
#if (NB_INTERRUPTS > 472)
channel(472)
#endif
#if (NB_INTERRUPTS > 473)
channel(473)
#endif
#if (NB_INTERRUPTS > 474)
channel(474)
#endif
#if (NB_INTERRUPTS > 475)
channel(475)
#endif
#if (NB_INTERRUPTS > 476)
channel(476)
#endif
#if (NB_INTERRUPTS > 477)
channel(477)
#endif
#if (NB_INTERRUPTS > 478)
channel(478)
#endif
#if (NB_INTERRUPTS > 479)
channel(479)
#endif
#if (NB_INTERRUPTS > 480)
channel(480)
#endif
#if (NB_INTERRUPTS > 481)
channel(481)
#endif
#if (NB_INTERRUPTS > 482)
channel(482)
#endif
#if (NB_INTERRUPTS > 483)
channel(483)
#endif
#if (NB_INTERRUPTS > 484)
channel(484)
#endif
#if (NB_INTERRUPTS > 485)
channel(485)
#endif
#if (NB_INTERRUPTS > 486)
channel(486)
#endif
#if (NB_INTERRUPTS > 487)
channel(487)
#endif
#if (NB_INTERRUPTS > 488)
channel(488)
#endif
#if (NB_INTERRUPTS > 489)
channel(489)
#endif
#if (NB_INTERRUPTS > 490)
channel(490)
#endif
#if (NB_INTERRUPTS > 491)
channel(491)
#endif
#if (NB_INTERRUPTS > 492)
channel(492)
#endif
#if (NB_INTERRUPTS > 493)
channel(493)
#endif
#if (NB_INTERRUPTS > 494)
channel(494)
#endif
#if (NB_INTERRUPTS > 495)
channel(495)
#endif
#if (NB_INTERRUPTS > 496)
channel(496)
#endif
#if (NB_INTERRUPTS > 497)
channel(497)
#endif
#if (NB_INTERRUPTS > 498)
channel(498)
#endif
#if (NB_INTERRUPTS > 499)
channel(499)
#endif
#if (NB_INTERRUPTS > 500)
channel(500)
#endif
#if (NB_INTERRUPTS > 501)
channel(501)
#endif
#if (NB_INTERRUPTS > 502)
channel(502)
#endif
#if (NB_INTERRUPTS > 503)
channel(503)
#endif
#if (NB_INTERRUPTS > 504)
channel(504)
#endif
#if (NB_INTERRUPTS > 505)
channel(505)
#endif
#if (NB_INTERRUPTS > 506)
channel(506)
#endif
#if (NB_INTERRUPTS > 507)
channel(507)
#endif
#if (NB_INTERRUPTS > 508)
channel(508)
#endif
#if (NB_INTERRUPTS > 509)
channel(509)
#endif
#if (NB_INTERRUPTS > 510)
channel(510)
#endif

#endif
