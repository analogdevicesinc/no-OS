/***************************************************************************//**
 *   @file   ADRV9040_RxGainTable.h
 *   @brief  adrv9040 RX gain table.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef ADRV9040_RX_GAIN_TABLE_H
#define ADRV9040_RX_GAIN_TABLE_H

#include <stdint.h>

const char *ADRV9040_RxGainTable_text =
	"Version,2,10,0,4\n\
Checksum,3032671981,0,0,0\n\
Gain Index,FE Control Word,Ext Control,Phase Offset,Digital Gain\n\
0,0,0,0,0,\n\
1,0,0,0,0,\n\
2,0,0,0,0,\n\
3,0,0,0,0,\n\
4,0,0,0,0,\n\
5,0,0,0,0,\n\
6,0,0,0,0,\n\
7,0,0,0,0,\n\
8,0,0,0,0,\n\
9,0,0,0,0,\n\
10,0,0,0,0,\n\
11,0,0,0,0,\n\
12,0,0,0,0,\n\
13,0,0,0,0,\n\
14,0,0,0,0,\n\
15,0,0,0,0,\n\
16,0,0,0,0,\n\
17,0,0,0,0,\n\
18,0,0,0,0,\n\
19,0,0,0,0,\n\
20,0,0,0,0,\n\
21,0,0,0,0,\n\
22,0,0,0,0,\n\
23,0,0,0,0,\n\
24,0,0,0,0,\n\
25,0,0,0,0,\n\
26,0,0,0,0,\n\
27,0,0,0,0,\n\
28,0,0,0,0,\n\
29,0,0,0,0,\n\
30,0,0,0,0,\n\
31,0,0,0,0,\n\
32,0,0,0,0,\n\
33,0,0,0,0,\n\
34,0,0,0,0,\n\
35,0,0,0,0,\n\
36,0,0,0,0,\n\
37,0,0,0,0,\n\
38,0,0,0,0,\n\
39,0,0,0,0,\n\
40,0,0,0,0,\n\
41,0,0,0,0,\n\
42,0,0,0,0,\n\
43,0,0,0,0,\n\
44,0,0,0,0,\n\
45,0,0,0,0,\n\
46,0,0,0,0,\n\
47,0,0,0,0,\n\
48,0,0,0,0,\n\
49,0,0,0,0,\n\
50,0,0,0,0,\n\
51,0,0,0,0,\n\
52,0,0,0,0,\n\
53,0,0,0,0,\n\
54,0,0,0,0,\n\
55,0,0,0,0,\n\
56,0,0,0,0,\n\
57,0,0,0,0,\n\
58,0,0,0,0,\n\
59,0,0,0,0,\n\
60,0,0,0,0,\n\
61,0,0,0,0,\n\
62,0,0,0,0,\n\
63,0,0,0,0,\n\
64,0,0,0,0,\n\
65,0,0,0,0,\n\
66,0,0,0,0,\n\
67,0,0,0,0,\n\
68,0,0,0,0,\n\
69,0,0,0,0,\n\
70,0,0,0,0,\n\
71,0,0,0,0,\n\
72,0,0,0,0,\n\
73,0,0,0,0,\n\
74,0,0,0,0,\n\
75,0,0,0,0,\n\
76,0,0,0,0,\n\
77,0,0,0,0,\n\
78,0,0,0,0,\n\
79,0,0,0,0,\n\
80,0,0,0,0,\n\
81,0,0,0,0,\n\
82,0,0,0,0,\n\
83,0,0,0,0,\n\
84,0,0,0,0,\n\
85,0,0,0,0,\n\
86,0,0,0,0,\n\
87,0,0,0,0,\n\
88,0,0,0,0,\n\
89,0,0,0,0,\n\
90,0,0,0,0,\n\
91,0,0,0,0,\n\
92,0,0,0,0,\n\
93,0,0,0,0,\n\
94,0,0,0,0,\n\
95,0,0,0,0,\n\
96,0,0,0,0,\n\
97,0,0,0,0,\n\
98,0,0,0,0,\n\
99,0,0,0,0,\n\
100,0,0,0,0,\n\
101,0,0,0,0,\n\
102,0,0,0,0,\n\
103,0,0,0,0,\n\
104,0,0,0,0,\n\
105,0,0,0,0,\n\
106,0,0,0,0,\n\
107,0,0,0,0,\n\
108,0,0,0,0,\n\
109,0,0,0,0,\n\
110,0,0,0,0,\n\
111,0,0,0,0,\n\
112,0,0,0,0,\n\
113,0,0,0,0,\n\
114,0,0,0,0,\n\
115,0,0,0,0,\n\
116,0,0,0,0,\n\
117,0,0,0,0,\n\
118,0,0,0,0,\n\
119,0,0,0,0,\n\
120,0,0,0,0,\n\
121,0,0,0,0,\n\
122,0,0,0,0,\n\
123,0,0,0,0,\n\
124,0,0,0,0,\n\
125,0,0,0,0,\n\
126,0,0,0,0,\n\
127,0,0,0,0,\n\
128,0,0,0,0,\n\
129,0,0,0,0,\n\
130,0,0,0,0,\n\
131,0,0,0,0,\n\
132,0,0,0,0,\n\
133,0,0,0,0,\n\
134,0,0,0,0,\n\
135,0,0,0,0,\n\
136,0,0,0,0,\n\
137,0,0,0,0,\n\
138,0,0,0,0,\n\
139,0,0,0,0,\n\
140,0,0,0,0,\n\
141,0,0,0,0,\n\
142,0,0,0,0,\n\
143,0,0,0,0,\n\
144,0,0,0,0,\n\
145,0,0,0,0,\n\
146,0,0,0,0,\n\
147,0,0,0,0,\n\
148,0,0,0,0,\n\
149,0,0,0,0,\n\
150,0,0,0,0,\n\
151,0,0,0,0,\n\
152,0,0,0,0,\n\
153,0,0,0,0,\n\
154,0,0,0,0,\n\
155,0,0,0,0,\n\
156,0,0,0,0,\n\
157,0,0,0,0,\n\
158,0,0,0,0,\n\
159,0,0,0,0,\n\
160,0,0,0,0,\n\
161,0,0,0,0,\n\
162,0,0,0,0,\n\
163,0,0,0,0,\n\
164,0,0,0,0,\n\
165,0,0,0,0,\n\
166,0,0,0,0,\n\
167,0,0,0,0,\n\
168,0,0,0,0,\n\
169,0,0,0,0,\n\
170,0,0,0,0,\n\
171,0,0,0,0,\n\
172,0,0,0,0,\n\
173,0,0,0,0,\n\
174,0,0,0,0,\n\
175,0,0,0,0,\n\
176,0,0,0,0,\n\
177,0,0,0,0,\n\
178,0,0,0,0,\n\
179,0,0,0,0,\n\
180,0,0,0,0,\n\
181,0,0,0,0,\n\
182,0,0,0,0,\n\
183,251,0,0,-20,\n\
184,251,0,0,-10,\n\
185,251,0,0,0,\n\
186,251,0,0,10,\n\
187,250,0,0,-17,\n\
188,250,0,0,-7,\n\
189,250,0,0,3,\n\
190,250,0,0,13,\n\
191,249,0,0,-9,\n\
192,249,0,0,1,\n\
193,249,0,0,11,\n\
194,248,0,0,-4,\n\
195,248,0,0,6,\n\
196,247,0,0,-11,\n\
197,247,0,0,-1,\n\
198,247,0,0,9,\n\
199,246,0,0,0,\n\
200,246,0,0,10,\n\
201,245,0,0,0,\n\
202,244,0,0,-5,\n\
203,244,0,0,5,\n\
204,243,0,0,0,\n\
205,242,0,0,-3,\n\
206,241,0,0,-6,\n\
207,241,0,0,4,\n\
208,240,0,0,3,\n\
209,239,0,0,0,\n\
210,238,0,0,0,\n\
211,237,0,0,0,\n\
212,236,0,0,1,\n\
213,235,0,0,2,\n\
214,234,0,0,4,\n\
215,232,0,0,-1,\n\
216,231,0,0,0,\n\
217,229,0,0,-3,\n\
218,228,0,0,1,\n\
219,226,0,0,-1,\n\
220,225,0,0,3,\n\
221,223,0,0,1,\n\
222,221,0,0,1,\n\
223,219,0,0,1,\n\
224,216,0,0,-2,\n\
225,214,0,0,-1,\n\
226,212,0,0,1,\n\
227,209,0,0,0,\n\
228,206,0,0,-1,\n\
229,203,0,0,-1,\n\
230,200,0,0,0,\n\
231,197,0,0,0,\n\
232,193,0,0,0,\n\
233,189,0,0,-1,\n\
234,185,0,0,-1,\n\
235,181,0,0,0,\n\
236,176,0,0,-1,\n\
237,172,0,0,1,\n\
238,166,0,0,-1,\n\
239,161,0,0,0,\n\
240,155,0,0,0,\n\
241,149,0,0,0,\n\
242,142,0,0,0,\n\
243,135,0,0,0,\n\
244,127,0,0,-1,\n\
245,119,0,0,0,\n\
246,111,0,0,0,\n\
247,101,0,0,0,\n\
248,91,0,0,0,\n\
249,81,0,0,0,\n\
250,70,0,0,0,\n\
251,57,0,0,0,\n\
252,45,0,0,0,\n\
253,31,0,0,0,\n\
254,16,0,0,0,\n\
255,0,0,0,0,\n";

#endif

