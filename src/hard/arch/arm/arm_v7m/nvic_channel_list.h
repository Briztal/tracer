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
//Channel 1 to 15 are system exceptions, implemented directly;

#if (NB_EXCEPTIONS > 16)
channel(16)
#endif
#if (NB_EXCEPTIONS > 17)
channel(17)
#endif
#if (NB_EXCEPTIONS > 18)
channel(18)
#endif
#if (NB_EXCEPTIONS > 19)
channel(19)
#endif
#if (NB_EXCEPTIONS > 20)
channel(20)
#endif
#if (NB_EXCEPTIONS > 21)
channel(21)
#endif
#if (NB_EXCEPTIONS > 22)
channel(22)
#endif
#if (NB_EXCEPTIONS > 23)
channel(23)
#endif
#if (NB_EXCEPTIONS > 24)
channel(24)
#endif
#if (NB_EXCEPTIONS > 25)
channel(25)
#endif
#if (NB_EXCEPTIONS > 26)
channel(26)
#endif
#if (NB_EXCEPTIONS > 27)
channel(27)
#endif
#if (NB_EXCEPTIONS > 28)
channel(28)
#endif
#if (NB_EXCEPTIONS > 29)
channel(29)
#endif
#if (NB_EXCEPTIONS > 30)
channel(30)
#endif
#if (NB_EXCEPTIONS > 31)
channel(31)
#endif
#if (NB_EXCEPTIONS > 32)
channel(32)
#endif
#if (NB_EXCEPTIONS > 33)
channel(33)
#endif
#if (NB_EXCEPTIONS > 34)
channel(34)
#endif
#if (NB_EXCEPTIONS > 35)
channel(35)
#endif
#if (NB_EXCEPTIONS > 36)
channel(36)
#endif
#if (NB_EXCEPTIONS > 37)
channel(37)
#endif
#if (NB_EXCEPTIONS > 38)
channel(38)
#endif
#if (NB_EXCEPTIONS > 39)
channel(39)
#endif
#if (NB_EXCEPTIONS > 40)
channel(40)
#endif
#if (NB_EXCEPTIONS > 41)
channel(41)
#endif
#if (NB_EXCEPTIONS > 42)
channel(42)
#endif
#if (NB_EXCEPTIONS > 43)
channel(43)
#endif
#if (NB_EXCEPTIONS > 44)
channel(44)
#endif
#if (NB_EXCEPTIONS > 45)
channel(45)
#endif
#if (NB_EXCEPTIONS > 46)
channel(46)
#endif
#if (NB_EXCEPTIONS > 47)
channel(47)
#endif
#if (NB_EXCEPTIONS > 48)
channel(48)
#endif
#if (NB_EXCEPTIONS > 49)
channel(49)
#endif
#if (NB_EXCEPTIONS > 50)
channel(50)
#endif
#if (NB_EXCEPTIONS > 51)
channel(51)
#endif
#if (NB_EXCEPTIONS > 52)
channel(52)
#endif
#if (NB_EXCEPTIONS > 53)
channel(53)
#endif
#if (NB_EXCEPTIONS > 54)
channel(54)
#endif
#if (NB_EXCEPTIONS > 55)
channel(55)
#endif
#if (NB_EXCEPTIONS > 56)
channel(56)
#endif
#if (NB_EXCEPTIONS > 57)
channel(57)
#endif
#if (NB_EXCEPTIONS > 58)
channel(58)
#endif
#if (NB_EXCEPTIONS > 59)
channel(59)
#endif
#if (NB_EXCEPTIONS > 60)
channel(60)
#endif
#if (NB_EXCEPTIONS > 61)
channel(61)
#endif
#if (NB_EXCEPTIONS > 62)
channel(62)
#endif
#if (NB_EXCEPTIONS > 63)
channel(63)
#endif
#if (NB_EXCEPTIONS > 64)
channel(64)
#endif
#if (NB_EXCEPTIONS > 65)
channel(65)
#endif
#if (NB_EXCEPTIONS > 66)
channel(66)
#endif
#if (NB_EXCEPTIONS > 67)
channel(67)
#endif
#if (NB_EXCEPTIONS > 68)
channel(68)
#endif
#if (NB_EXCEPTIONS > 69)
channel(69)
#endif
#if (NB_EXCEPTIONS > 70)
channel(70)
#endif
#if (NB_EXCEPTIONS > 71)
channel(71)
#endif
#if (NB_EXCEPTIONS > 72)
channel(72)
#endif
#if (NB_EXCEPTIONS > 73)
channel(73)
#endif
#if (NB_EXCEPTIONS > 74)
channel(74)
#endif
#if (NB_EXCEPTIONS > 75)
channel(75)
#endif
#if (NB_EXCEPTIONS > 76)
channel(76)
#endif
#if (NB_EXCEPTIONS > 77)
channel(77)
#endif
#if (NB_EXCEPTIONS > 78)
channel(78)
#endif
#if (NB_EXCEPTIONS > 79)
channel(79)
#endif
#if (NB_EXCEPTIONS > 80)
channel(80)
#endif
#if (NB_EXCEPTIONS > 81)
channel(81)
#endif
#if (NB_EXCEPTIONS > 82)
channel(82)
#endif
#if (NB_EXCEPTIONS > 83)
channel(83)
#endif
#if (NB_EXCEPTIONS > 84)
channel(84)
#endif
#if (NB_EXCEPTIONS > 85)
channel(85)
#endif
#if (NB_EXCEPTIONS > 86)
channel(86)
#endif
#if (NB_EXCEPTIONS > 87)
channel(87)
#endif
#if (NB_EXCEPTIONS > 88)
channel(88)
#endif
#if (NB_EXCEPTIONS > 89)
channel(89)
#endif
#if (NB_EXCEPTIONS > 90)
channel(90)
#endif
#if (NB_EXCEPTIONS > 91)
channel(91)
#endif
#if (NB_EXCEPTIONS > 92)
channel(92)
#endif
#if (NB_EXCEPTIONS > 93)
channel(93)
#endif
#if (NB_EXCEPTIONS > 94)
channel(94)
#endif
#if (NB_EXCEPTIONS > 95)
channel(95)
#endif
#if (NB_EXCEPTIONS > 96)
channel(96)
#endif
#if (NB_EXCEPTIONS > 97)
channel(97)
#endif
#if (NB_EXCEPTIONS > 98)
channel(98)
#endif
#if (NB_EXCEPTIONS > 99)
channel(99)
#endif
#if (NB_EXCEPTIONS > 100)
channel(100)
#endif
#if (NB_EXCEPTIONS > 101)
channel(101)
#endif
#if (NB_EXCEPTIONS > 102)
channel(102)
#endif
#if (NB_EXCEPTIONS > 103)
channel(103)
#endif
#if (NB_EXCEPTIONS > 104)
channel(104)
#endif
#if (NB_EXCEPTIONS > 105)
channel(105)
#endif
#if (NB_EXCEPTIONS > 106)
channel(106)
#endif
#if (NB_EXCEPTIONS > 107)
channel(107)
#endif
#if (NB_EXCEPTIONS > 108)
channel(108)
#endif
#if (NB_EXCEPTIONS > 109)
channel(109)
#endif
#if (NB_EXCEPTIONS > 110)
channel(110)
#endif
#if (NB_EXCEPTIONS > 111)
channel(111)
#endif
#if (NB_EXCEPTIONS > 112)
channel(112)
#endif
#if (NB_EXCEPTIONS > 113)
channel(113)
#endif
#if (NB_EXCEPTIONS > 114)
channel(114)
#endif
#if (NB_EXCEPTIONS > 115)
channel(115)
#endif
#if (NB_EXCEPTIONS > 116)
channel(116)
#endif
#if (NB_EXCEPTIONS > 117)
channel(117)
#endif
#if (NB_EXCEPTIONS > 118)
channel(118)
#endif
#if (NB_EXCEPTIONS > 119)
channel(119)
#endif
#if (NB_EXCEPTIONS > 120)
channel(120)
#endif
#if (NB_EXCEPTIONS > 121)
channel(121)
#endif
#if (NB_EXCEPTIONS > 122)
channel(122)
#endif
#if (NB_EXCEPTIONS > 123)
channel(123)
#endif
#if (NB_EXCEPTIONS > 124)
channel(124)
#endif
#if (NB_EXCEPTIONS > 125)
channel(125)
#endif
#if (NB_EXCEPTIONS > 126)
channel(126)
#endif
#if (NB_EXCEPTIONS > 127)
channel(127)
#endif
#if (NB_EXCEPTIONS > 128)
channel(128)
#endif
#if (NB_EXCEPTIONS > 129)
channel(129)
#endif
#if (NB_EXCEPTIONS > 130)
channel(130)
#endif
#if (NB_EXCEPTIONS > 131)
channel(131)
#endif
#if (NB_EXCEPTIONS > 132)
channel(132)
#endif
#if (NB_EXCEPTIONS > 133)
channel(133)
#endif
#if (NB_EXCEPTIONS > 134)
channel(134)
#endif
#if (NB_EXCEPTIONS > 135)
channel(135)
#endif
#if (NB_EXCEPTIONS > 136)
channel(136)
#endif
#if (NB_EXCEPTIONS > 137)
channel(137)
#endif
#if (NB_EXCEPTIONS > 138)
channel(138)
#endif
#if (NB_EXCEPTIONS > 139)
channel(139)
#endif
#if (NB_EXCEPTIONS > 140)
channel(140)
#endif
#if (NB_EXCEPTIONS > 141)
channel(141)
#endif
#if (NB_EXCEPTIONS > 142)
channel(142)
#endif
#if (NB_EXCEPTIONS > 143)
channel(143)
#endif
#if (NB_EXCEPTIONS > 144)
channel(144)
#endif
#if (NB_EXCEPTIONS > 145)
channel(145)
#endif
#if (NB_EXCEPTIONS > 146)
channel(146)
#endif
#if (NB_EXCEPTIONS > 147)
channel(147)
#endif
#if (NB_EXCEPTIONS > 148)
channel(148)
#endif
#if (NB_EXCEPTIONS > 149)
channel(149)
#endif
#if (NB_EXCEPTIONS > 150)
channel(150)
#endif
#if (NB_EXCEPTIONS > 151)
channel(151)
#endif
#if (NB_EXCEPTIONS > 152)
channel(152)
#endif
#if (NB_EXCEPTIONS > 153)
channel(153)
#endif
#if (NB_EXCEPTIONS > 154)
channel(154)
#endif
#if (NB_EXCEPTIONS > 155)
channel(155)
#endif
#if (NB_EXCEPTIONS > 156)
channel(156)
#endif
#if (NB_EXCEPTIONS > 157)
channel(157)
#endif
#if (NB_EXCEPTIONS > 158)
channel(158)
#endif
#if (NB_EXCEPTIONS > 159)
channel(159)
#endif
#if (NB_EXCEPTIONS > 160)
channel(160)
#endif
#if (NB_EXCEPTIONS > 161)
channel(161)
#endif
#if (NB_EXCEPTIONS > 162)
channel(162)
#endif
#if (NB_EXCEPTIONS > 163)
channel(163)
#endif
#if (NB_EXCEPTIONS > 164)
channel(164)
#endif
#if (NB_EXCEPTIONS > 165)
channel(165)
#endif
#if (NB_EXCEPTIONS > 166)
channel(166)
#endif
#if (NB_EXCEPTIONS > 167)
channel(167)
#endif
#if (NB_EXCEPTIONS > 168)
channel(168)
#endif
#if (NB_EXCEPTIONS > 169)
channel(169)
#endif
#if (NB_EXCEPTIONS > 170)
channel(170)
#endif
#if (NB_EXCEPTIONS > 171)
channel(171)
#endif
#if (NB_EXCEPTIONS > 172)
channel(172)
#endif
#if (NB_EXCEPTIONS > 173)
channel(173)
#endif
#if (NB_EXCEPTIONS > 174)
channel(174)
#endif
#if (NB_EXCEPTIONS > 175)
channel(175)
#endif
#if (NB_EXCEPTIONS > 176)
channel(176)
#endif
#if (NB_EXCEPTIONS > 177)
channel(177)
#endif
#if (NB_EXCEPTIONS > 178)
channel(178)
#endif
#if (NB_EXCEPTIONS > 179)
channel(179)
#endif
#if (NB_EXCEPTIONS > 180)
channel(180)
#endif
#if (NB_EXCEPTIONS > 181)
channel(181)
#endif
#if (NB_EXCEPTIONS > 182)
channel(182)
#endif
#if (NB_EXCEPTIONS > 183)
channel(183)
#endif
#if (NB_EXCEPTIONS > 184)
channel(184)
#endif
#if (NB_EXCEPTIONS > 185)
channel(185)
#endif
#if (NB_EXCEPTIONS > 186)
channel(186)
#endif
#if (NB_EXCEPTIONS > 187)
channel(187)
#endif
#if (NB_EXCEPTIONS > 188)
channel(188)
#endif
#if (NB_EXCEPTIONS > 189)
channel(189)
#endif
#if (NB_EXCEPTIONS > 190)
channel(190)
#endif
#if (NB_EXCEPTIONS > 191)
channel(191)
#endif
#if (NB_EXCEPTIONS > 192)
channel(192)
#endif
#if (NB_EXCEPTIONS > 193)
channel(193)
#endif
#if (NB_EXCEPTIONS > 194)
channel(194)
#endif
#if (NB_EXCEPTIONS > 195)
channel(195)
#endif
#if (NB_EXCEPTIONS > 196)
channel(196)
#endif
#if (NB_EXCEPTIONS > 197)
channel(197)
#endif
#if (NB_EXCEPTIONS > 198)
channel(198)
#endif
#if (NB_EXCEPTIONS > 199)
channel(199)
#endif
#if (NB_EXCEPTIONS > 200)
channel(200)
#endif
#if (NB_EXCEPTIONS > 201)
channel(201)
#endif
#if (NB_EXCEPTIONS > 202)
channel(202)
#endif
#if (NB_EXCEPTIONS > 203)
channel(203)
#endif
#if (NB_EXCEPTIONS > 204)
channel(204)
#endif
#if (NB_EXCEPTIONS > 205)
channel(205)
#endif
#if (NB_EXCEPTIONS > 206)
channel(206)
#endif
#if (NB_EXCEPTIONS > 207)
channel(207)
#endif
#if (NB_EXCEPTIONS > 208)
channel(208)
#endif
#if (NB_EXCEPTIONS > 209)
channel(209)
#endif
#if (NB_EXCEPTIONS > 210)
channel(210)
#endif
#if (NB_EXCEPTIONS > 211)
channel(211)
#endif
#if (NB_EXCEPTIONS > 212)
channel(212)
#endif
#if (NB_EXCEPTIONS > 213)
channel(213)
#endif
#if (NB_EXCEPTIONS > 214)
channel(214)
#endif
#if (NB_EXCEPTIONS > 215)
channel(215)
#endif
#if (NB_EXCEPTIONS > 216)
channel(216)
#endif
#if (NB_EXCEPTIONS > 217)
channel(217)
#endif
#if (NB_EXCEPTIONS > 218)
channel(218)
#endif
#if (NB_EXCEPTIONS > 219)
channel(219)
#endif
#if (NB_EXCEPTIONS > 220)
channel(220)
#endif
#if (NB_EXCEPTIONS > 221)
channel(221)
#endif
#if (NB_EXCEPTIONS > 222)
channel(222)
#endif
#if (NB_EXCEPTIONS > 223)
channel(223)
#endif
#if (NB_EXCEPTIONS > 224)
channel(224)
#endif
#if (NB_EXCEPTIONS > 225)
channel(225)
#endif
#if (NB_EXCEPTIONS > 226)
channel(226)
#endif
#if (NB_EXCEPTIONS > 227)
channel(227)
#endif
#if (NB_EXCEPTIONS > 228)
channel(228)
#endif
#if (NB_EXCEPTIONS > 229)
channel(229)
#endif
#if (NB_EXCEPTIONS > 230)
channel(230)
#endif
#if (NB_EXCEPTIONS > 231)
channel(231)
#endif
#if (NB_EXCEPTIONS > 232)
channel(232)
#endif
#if (NB_EXCEPTIONS > 233)
channel(233)
#endif
#if (NB_EXCEPTIONS > 234)
channel(234)
#endif
#if (NB_EXCEPTIONS > 235)
channel(235)
#endif
#if (NB_EXCEPTIONS > 236)
channel(236)
#endif
#if (NB_EXCEPTIONS > 237)
channel(237)
#endif
#if (NB_EXCEPTIONS > 238)
channel(238)
#endif
#if (NB_EXCEPTIONS > 239)
channel(239)
#endif
#if (NB_EXCEPTIONS > 240)
channel(240)
#endif
#if (NB_EXCEPTIONS > 241)
channel(241)
#endif
#if (NB_EXCEPTIONS > 242)
channel(242)
#endif
#if (NB_EXCEPTIONS > 243)
channel(243)
#endif
#if (NB_EXCEPTIONS > 244)
channel(244)
#endif
#if (NB_EXCEPTIONS > 245)
channel(245)
#endif
#if (NB_EXCEPTIONS > 246)
channel(246)
#endif
#if (NB_EXCEPTIONS > 247)
channel(247)
#endif
#if (NB_EXCEPTIONS > 248)
channel(248)
#endif
#if (NB_EXCEPTIONS > 249)
channel(249)
#endif
#if (NB_EXCEPTIONS > 250)
channel(250)
#endif
#if (NB_EXCEPTIONS > 251)
channel(251)
#endif
#if (NB_EXCEPTIONS > 252)
channel(252)
#endif
#if (NB_EXCEPTIONS > 253)
channel(253)
#endif
#if (NB_EXCEPTIONS > 254)
channel(254)
#endif
#if (NB_EXCEPTIONS > 255)
channel(255)
#endif
#if (NB_EXCEPTIONS > 256)
channel(256)
#endif
#if (NB_EXCEPTIONS > 257)
channel(257)
#endif
#if (NB_EXCEPTIONS > 258)
channel(258)
#endif
#if (NB_EXCEPTIONS > 259)
channel(259)
#endif
#if (NB_EXCEPTIONS > 260)
channel(260)
#endif
#if (NB_EXCEPTIONS > 261)
channel(261)
#endif
#if (NB_EXCEPTIONS > 262)
channel(262)
#endif
#if (NB_EXCEPTIONS > 263)
channel(263)
#endif
#if (NB_EXCEPTIONS > 264)
channel(264)
#endif
#if (NB_EXCEPTIONS > 265)
channel(265)
#endif
#if (NB_EXCEPTIONS > 266)
channel(266)
#endif
#if (NB_EXCEPTIONS > 267)
channel(267)
#endif
#if (NB_EXCEPTIONS > 268)
channel(268)
#endif
#if (NB_EXCEPTIONS > 269)
channel(269)
#endif
#if (NB_EXCEPTIONS > 270)
channel(270)
#endif
#if (NB_EXCEPTIONS > 271)
channel(271)
#endif
#if (NB_EXCEPTIONS > 272)
channel(272)
#endif
#if (NB_EXCEPTIONS > 273)
channel(273)
#endif
#if (NB_EXCEPTIONS > 274)
channel(274)
#endif
#if (NB_EXCEPTIONS > 275)
channel(275)
#endif
#if (NB_EXCEPTIONS > 276)
channel(276)
#endif
#if (NB_EXCEPTIONS > 277)
channel(277)
#endif
#if (NB_EXCEPTIONS > 278)
channel(278)
#endif
#if (NB_EXCEPTIONS > 279)
channel(279)
#endif
#if (NB_EXCEPTIONS > 280)
channel(280)
#endif
#if (NB_EXCEPTIONS > 281)
channel(281)
#endif
#if (NB_EXCEPTIONS > 282)
channel(282)
#endif
#if (NB_EXCEPTIONS > 283)
channel(283)
#endif
#if (NB_EXCEPTIONS > 284)
channel(284)
#endif
#if (NB_EXCEPTIONS > 285)
channel(285)
#endif
#if (NB_EXCEPTIONS > 286)
channel(286)
#endif
#if (NB_EXCEPTIONS > 287)
channel(287)
#endif
#if (NB_EXCEPTIONS > 288)
channel(288)
#endif
#if (NB_EXCEPTIONS > 289)
channel(289)
#endif
#if (NB_EXCEPTIONS > 290)
channel(290)
#endif
#if (NB_EXCEPTIONS > 291)
channel(291)
#endif
#if (NB_EXCEPTIONS > 292)
channel(292)
#endif
#if (NB_EXCEPTIONS > 293)
channel(293)
#endif
#if (NB_EXCEPTIONS > 294)
channel(294)
#endif
#if (NB_EXCEPTIONS > 295)
channel(295)
#endif
#if (NB_EXCEPTIONS > 296)
channel(296)
#endif
#if (NB_EXCEPTIONS > 297)
channel(297)
#endif
#if (NB_EXCEPTIONS > 298)
channel(298)
#endif
#if (NB_EXCEPTIONS > 299)
channel(299)
#endif
#if (NB_EXCEPTIONS > 300)
channel(300)
#endif
#if (NB_EXCEPTIONS > 301)
channel(301)
#endif
#if (NB_EXCEPTIONS > 302)
channel(302)
#endif
#if (NB_EXCEPTIONS > 303)
channel(303)
#endif
#if (NB_EXCEPTIONS > 304)
channel(304)
#endif
#if (NB_EXCEPTIONS > 305)
channel(305)
#endif
#if (NB_EXCEPTIONS > 306)
channel(306)
#endif
#if (NB_EXCEPTIONS > 307)
channel(307)
#endif
#if (NB_EXCEPTIONS > 308)
channel(308)
#endif
#if (NB_EXCEPTIONS > 309)
channel(309)
#endif
#if (NB_EXCEPTIONS > 310)
channel(310)
#endif
#if (NB_EXCEPTIONS > 311)
channel(311)
#endif
#if (NB_EXCEPTIONS > 312)
channel(312)
#endif
#if (NB_EXCEPTIONS > 313)
channel(313)
#endif
#if (NB_EXCEPTIONS > 314)
channel(314)
#endif
#if (NB_EXCEPTIONS > 315)
channel(315)
#endif
#if (NB_EXCEPTIONS > 316)
channel(316)
#endif
#if (NB_EXCEPTIONS > 317)
channel(317)
#endif
#if (NB_EXCEPTIONS > 318)
channel(318)
#endif
#if (NB_EXCEPTIONS > 319)
channel(319)
#endif
#if (NB_EXCEPTIONS > 320)
channel(320)
#endif
#if (NB_EXCEPTIONS > 321)
channel(321)
#endif
#if (NB_EXCEPTIONS > 322)
channel(322)
#endif
#if (NB_EXCEPTIONS > 323)
channel(323)
#endif
#if (NB_EXCEPTIONS > 324)
channel(324)
#endif
#if (NB_EXCEPTIONS > 325)
channel(325)
#endif
#if (NB_EXCEPTIONS > 326)
channel(326)
#endif
#if (NB_EXCEPTIONS > 327)
channel(327)
#endif
#if (NB_EXCEPTIONS > 328)
channel(328)
#endif
#if (NB_EXCEPTIONS > 329)
channel(329)
#endif
#if (NB_EXCEPTIONS > 330)
channel(330)
#endif
#if (NB_EXCEPTIONS > 331)
channel(331)
#endif
#if (NB_EXCEPTIONS > 332)
channel(332)
#endif
#if (NB_EXCEPTIONS > 333)
channel(333)
#endif
#if (NB_EXCEPTIONS > 334)
channel(334)
#endif
#if (NB_EXCEPTIONS > 335)
channel(335)
#endif
#if (NB_EXCEPTIONS > 336)
channel(336)
#endif
#if (NB_EXCEPTIONS > 337)
channel(337)
#endif
#if (NB_EXCEPTIONS > 338)
channel(338)
#endif
#if (NB_EXCEPTIONS > 339)
channel(339)
#endif
#if (NB_EXCEPTIONS > 340)
channel(340)
#endif
#if (NB_EXCEPTIONS > 341)
channel(341)
#endif
#if (NB_EXCEPTIONS > 342)
channel(342)
#endif
#if (NB_EXCEPTIONS > 343)
channel(343)
#endif
#if (NB_EXCEPTIONS > 344)
channel(344)
#endif
#if (NB_EXCEPTIONS > 345)
channel(345)
#endif
#if (NB_EXCEPTIONS > 346)
channel(346)
#endif
#if (NB_EXCEPTIONS > 347)
channel(347)
#endif
#if (NB_EXCEPTIONS > 348)
channel(348)
#endif
#if (NB_EXCEPTIONS > 349)
channel(349)
#endif
#if (NB_EXCEPTIONS > 350)
channel(350)
#endif
#if (NB_EXCEPTIONS > 351)
channel(351)
#endif
#if (NB_EXCEPTIONS > 352)
channel(352)
#endif
#if (NB_EXCEPTIONS > 353)
channel(353)
#endif
#if (NB_EXCEPTIONS > 354)
channel(354)
#endif
#if (NB_EXCEPTIONS > 355)
channel(355)
#endif
#if (NB_EXCEPTIONS > 356)
channel(356)
#endif
#if (NB_EXCEPTIONS > 357)
channel(357)
#endif
#if (NB_EXCEPTIONS > 358)
channel(358)
#endif
#if (NB_EXCEPTIONS > 359)
channel(359)
#endif
#if (NB_EXCEPTIONS > 360)
channel(360)
#endif
#if (NB_EXCEPTIONS > 361)
channel(361)
#endif
#if (NB_EXCEPTIONS > 362)
channel(362)
#endif
#if (NB_EXCEPTIONS > 363)
channel(363)
#endif
#if (NB_EXCEPTIONS > 364)
channel(364)
#endif
#if (NB_EXCEPTIONS > 365)
channel(365)
#endif
#if (NB_EXCEPTIONS > 366)
channel(366)
#endif
#if (NB_EXCEPTIONS > 367)
channel(367)
#endif
#if (NB_EXCEPTIONS > 368)
channel(368)
#endif
#if (NB_EXCEPTIONS > 369)
channel(369)
#endif
#if (NB_EXCEPTIONS > 370)
channel(370)
#endif
#if (NB_EXCEPTIONS > 371)
channel(371)
#endif
#if (NB_EXCEPTIONS > 372)
channel(372)
#endif
#if (NB_EXCEPTIONS > 373)
channel(373)
#endif
#if (NB_EXCEPTIONS > 374)
channel(374)
#endif
#if (NB_EXCEPTIONS > 375)
channel(375)
#endif
#if (NB_EXCEPTIONS > 376)
channel(376)
#endif
#if (NB_EXCEPTIONS > 377)
channel(377)
#endif
#if (NB_EXCEPTIONS > 378)
channel(378)
#endif
#if (NB_EXCEPTIONS > 379)
channel(379)
#endif
#if (NB_EXCEPTIONS > 380)
channel(380)
#endif
#if (NB_EXCEPTIONS > 381)
channel(381)
#endif
#if (NB_EXCEPTIONS > 382)
channel(382)
#endif
#if (NB_EXCEPTIONS > 383)
channel(383)
#endif
#if (NB_EXCEPTIONS > 384)
channel(384)
#endif
#if (NB_EXCEPTIONS > 385)
channel(385)
#endif
#if (NB_EXCEPTIONS > 386)
channel(386)
#endif
#if (NB_EXCEPTIONS > 387)
channel(387)
#endif
#if (NB_EXCEPTIONS > 388)
channel(388)
#endif
#if (NB_EXCEPTIONS > 389)
channel(389)
#endif
#if (NB_EXCEPTIONS > 390)
channel(390)
#endif
#if (NB_EXCEPTIONS > 391)
channel(391)
#endif
#if (NB_EXCEPTIONS > 392)
channel(392)
#endif
#if (NB_EXCEPTIONS > 393)
channel(393)
#endif
#if (NB_EXCEPTIONS > 394)
channel(394)
#endif
#if (NB_EXCEPTIONS > 395)
channel(395)
#endif
#if (NB_EXCEPTIONS > 396)
channel(396)
#endif
#if (NB_EXCEPTIONS > 397)
channel(397)
#endif
#if (NB_EXCEPTIONS > 398)
channel(398)
#endif
#if (NB_EXCEPTIONS > 399)
channel(399)
#endif
#if (NB_EXCEPTIONS > 400)
channel(400)
#endif
#if (NB_EXCEPTIONS > 401)
channel(401)
#endif
#if (NB_EXCEPTIONS > 402)
channel(402)
#endif
#if (NB_EXCEPTIONS > 403)
channel(403)
#endif
#if (NB_EXCEPTIONS > 404)
channel(404)
#endif
#if (NB_EXCEPTIONS > 405)
channel(405)
#endif
#if (NB_EXCEPTIONS > 406)
channel(406)
#endif
#if (NB_EXCEPTIONS > 407)
channel(407)
#endif
#if (NB_EXCEPTIONS > 408)
channel(408)
#endif
#if (NB_EXCEPTIONS > 409)
channel(409)
#endif
#if (NB_EXCEPTIONS > 410)
channel(410)
#endif
#if (NB_EXCEPTIONS > 411)
channel(411)
#endif
#if (NB_EXCEPTIONS > 412)
channel(412)
#endif
#if (NB_EXCEPTIONS > 413)
channel(413)
#endif
#if (NB_EXCEPTIONS > 414)
channel(414)
#endif
#if (NB_EXCEPTIONS > 415)
channel(415)
#endif
#if (NB_EXCEPTIONS > 416)
channel(416)
#endif
#if (NB_EXCEPTIONS > 417)
channel(417)
#endif
#if (NB_EXCEPTIONS > 418)
channel(418)
#endif
#if (NB_EXCEPTIONS > 419)
channel(419)
#endif
#if (NB_EXCEPTIONS > 420)
channel(420)
#endif
#if (NB_EXCEPTIONS > 421)
channel(421)
#endif
#if (NB_EXCEPTIONS > 422)
channel(422)
#endif
#if (NB_EXCEPTIONS > 423)
channel(423)
#endif
#if (NB_EXCEPTIONS > 424)
channel(424)
#endif
#if (NB_EXCEPTIONS > 425)
channel(425)
#endif
#if (NB_EXCEPTIONS > 426)
channel(426)
#endif
#if (NB_EXCEPTIONS > 427)
channel(427)
#endif
#if (NB_EXCEPTIONS > 428)
channel(428)
#endif
#if (NB_EXCEPTIONS > 429)
channel(429)
#endif
#if (NB_EXCEPTIONS > 430)
channel(430)
#endif
#if (NB_EXCEPTIONS > 431)
channel(431)
#endif
#if (NB_EXCEPTIONS > 432)
channel(432)
#endif
#if (NB_EXCEPTIONS > 433)
channel(433)
#endif
#if (NB_EXCEPTIONS > 434)
channel(434)
#endif
#if (NB_EXCEPTIONS > 435)
channel(435)
#endif
#if (NB_EXCEPTIONS > 436)
channel(436)
#endif
#if (NB_EXCEPTIONS > 437)
channel(437)
#endif
#if (NB_EXCEPTIONS > 438)
channel(438)
#endif
#if (NB_EXCEPTIONS > 439)
channel(439)
#endif
#if (NB_EXCEPTIONS > 440)
channel(440)
#endif
#if (NB_EXCEPTIONS > 441)
channel(441)
#endif
#if (NB_EXCEPTIONS > 442)
channel(442)
#endif
#if (NB_EXCEPTIONS > 443)
channel(443)
#endif
#if (NB_EXCEPTIONS > 444)
channel(444)
#endif
#if (NB_EXCEPTIONS > 445)
channel(445)
#endif
#if (NB_EXCEPTIONS > 446)
channel(446)
#endif
#if (NB_EXCEPTIONS > 447)
channel(447)
#endif
#if (NB_EXCEPTIONS > 448)
channel(448)
#endif
#if (NB_EXCEPTIONS > 449)
channel(449)
#endif
#if (NB_EXCEPTIONS > 450)
channel(450)
#endif
#if (NB_EXCEPTIONS > 451)
channel(451)
#endif
#if (NB_EXCEPTIONS > 452)
channel(452)
#endif
#if (NB_EXCEPTIONS > 453)
channel(453)
#endif
#if (NB_EXCEPTIONS > 454)
channel(454)
#endif
#if (NB_EXCEPTIONS > 455)
channel(455)
#endif
#if (NB_EXCEPTIONS > 456)
channel(456)
#endif
#if (NB_EXCEPTIONS > 457)
channel(457)
#endif
#if (NB_EXCEPTIONS > 458)
channel(458)
#endif
#if (NB_EXCEPTIONS > 459)
channel(459)
#endif
#if (NB_EXCEPTIONS > 460)
channel(460)
#endif
#if (NB_EXCEPTIONS > 461)
channel(461)
#endif
#if (NB_EXCEPTIONS > 462)
channel(462)
#endif
#if (NB_EXCEPTIONS > 463)
channel(463)
#endif
#if (NB_EXCEPTIONS > 464)
channel(464)
#endif
#if (NB_EXCEPTIONS > 465)
channel(465)
#endif
#if (NB_EXCEPTIONS > 466)
channel(466)
#endif
#if (NB_EXCEPTIONS > 467)
channel(467)
#endif
#if (NB_EXCEPTIONS > 468)
channel(468)
#endif
#if (NB_EXCEPTIONS > 469)
channel(469)
#endif
#if (NB_EXCEPTIONS > 470)
channel(470)
#endif
#if (NB_EXCEPTIONS > 471)
channel(471)
#endif
#if (NB_EXCEPTIONS > 472)
channel(472)
#endif
#if (NB_EXCEPTIONS > 473)
channel(473)
#endif
#if (NB_EXCEPTIONS > 474)
channel(474)
#endif
#if (NB_EXCEPTIONS > 475)
channel(475)
#endif
#if (NB_EXCEPTIONS > 476)
channel(476)
#endif
#if (NB_EXCEPTIONS > 477)
channel(477)
#endif
#if (NB_EXCEPTIONS > 478)
channel(478)
#endif
#if (NB_EXCEPTIONS > 479)
channel(479)
#endif
#if (NB_EXCEPTIONS > 480)
channel(480)
#endif
#if (NB_EXCEPTIONS > 481)
channel(481)
#endif
#if (NB_EXCEPTIONS > 482)
channel(482)
#endif
#if (NB_EXCEPTIONS > 483)
channel(483)
#endif
#if (NB_EXCEPTIONS > 484)
channel(484)
#endif
#if (NB_EXCEPTIONS > 485)
channel(485)
#endif
#if (NB_EXCEPTIONS > 486)
channel(486)
#endif
#if (NB_EXCEPTIONS > 487)
channel(487)
#endif
#if (NB_EXCEPTIONS > 488)
channel(488)
#endif
#if (NB_EXCEPTIONS > 489)
channel(489)
#endif
#if (NB_EXCEPTIONS > 490)
channel(490)
#endif
#if (NB_EXCEPTIONS > 491)
channel(491)
#endif
#if (NB_EXCEPTIONS > 492)
channel(492)
#endif
#if (NB_EXCEPTIONS > 493)
channel(493)
#endif
#if (NB_EXCEPTIONS > 494)
channel(494)
#endif
#if (NB_EXCEPTIONS > 495)
channel(495)
#endif
#if (NB_EXCEPTIONS > 496)
channel(496)
#endif
#if (NB_EXCEPTIONS > 497)
channel(497)
#endif
#if (NB_EXCEPTIONS > 498)
channel(498)
#endif
#if (NB_EXCEPTIONS > 499)
channel(499)
#endif
#if (NB_EXCEPTIONS > 500)
channel(500)
#endif
#if (NB_EXCEPTIONS > 501)
channel(501)
#endif
#if (NB_EXCEPTIONS > 502)
channel(502)
#endif
#if (NB_EXCEPTIONS > 503)
channel(503)
#endif
#if (NB_EXCEPTIONS > 504)
channel(504)
#endif
#if (NB_EXCEPTIONS > 505)
channel(505)
#endif
#if (NB_EXCEPTIONS > 506)
channel(506)
#endif
#if (NB_EXCEPTIONS > 507)
channel(507)
#endif
#if (NB_EXCEPTIONS > 508)
channel(508)
#endif
#if (NB_EXCEPTIONS > 509)
channel(509)
#endif
#if (NB_EXCEPTIONS > 510)
channel(510)
#endif

#endif
