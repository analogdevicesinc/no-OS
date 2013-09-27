// ***************************************************************************
// ***************************************************************************
// Copyright 2011(c) Analog Devices, Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//     - Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     - Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the
//       distribution.
//     - Neither the name of Analog Devices, Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//     - The use of this software may or may not infringe the patent rights
//       of one or more patent holders.  This license does not release you
//       from the requirement that you obtain separate licenses from these
//       patent holders to use this software.
//     - Use of the software either in source or binary form, must be run
//       on or directly connected to an Analog Devices Inc. component.
//    
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.
//
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
// RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

module cf_csc_1 (

  // csc inputs
  // csc <= c1*d[23:16] + c2*d[15:8] + c3*d[7:0] + c4;

  clk,
  vs,
  hs,
  de,
  data,

  C1,
  C2,
  C3,
  C4,

  // these are the delay matched versions of the inputs

  csc_vs,
  csc_hs,
  csc_de,
  csc_data_1);

  // csc inputs
  // csc <= c1*d[23:16] + c2*d[15:8] + c3*d[7:0] + c4;

  input           clk;
  input           vs;
  input           hs;
  input           de;
  input   [23:0]  data;

  input   [16:0]  C1;
  input   [16:0]  C2;
  input   [16:0]  C3;
  input   [24:0]  C4;

  // these are the delay matched versions of the inputs

  output          csc_vs;
  output          csc_hs;
  output          csc_de;
  output  [ 7:0]  csc_data_1;

  wire    [24:0]  data_1_m_s;
  wire    [ 2:0]  ddata_1_m_s;
  wire    [24:0]  data_2_m_s;
  wire    [ 2:0]  ddata_2_m_s;
  wire    [24:0]  data_3_m_s;
  wire    [ 2:0]  ddata_3_m_s;
  wire    [ 2:0]  ddata_m_s;

  assign ddata_m_s = ddata_1_m_s & ddata_2_m_s & ddata_3_m_s;

  // c1*R

  cf_csc_1_mul #(.DELAY_DATA_WIDTH(3)) i_mul_c1 (
    .clk (clk),
    .data_a (C1),
    .data_b (data[23:16]),
    .data_p (data_1_m_s),
    .ddata_in ({vs, hs, de}),
    .ddata_out (ddata_1_m_s));

  // c2*G

  cf_csc_1_mul #(.DELAY_DATA_WIDTH(3)) i_mul_c2 (
    .clk (clk),
    .data_a (C2),
    .data_b (data[15:8]),
    .data_p (data_2_m_s),
    .ddata_in ({vs, hs, de}),
    .ddata_out (ddata_2_m_s));

  // c3*B

  cf_csc_1_mul #(.DELAY_DATA_WIDTH(3)) i_mul_c3 (
    .clk (clk),
    .data_a (C3),
    .data_b (data[7:0]),
    .data_p (data_3_m_s),
    .ddata_in ({vs, hs, de}),
    .ddata_out (ddata_3_m_s));

  // last stage addition

  cf_csc_1_add #(.DELAY_DATA_WIDTH(3)) i_add_c4 (
    .clk (clk),
    .data_1 (data_1_m_s),
    .data_2 (data_2_m_s),
    .data_3 (data_3_m_s),
    .data_4 (C4),
    .data_p (csc_data_1),
    .ddata_in (ddata_m_s),
    .ddata_out ({csc_vs, csc_hs, csc_de}));

endmodule

// ***************************************************************************
// ***************************************************************************
