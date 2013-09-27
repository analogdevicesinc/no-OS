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
// Transmit HDMI, CrYCb to RGB conversion
// The multiplication coefficients are in 1.4.12 format
// The addition coefficients are in 1.12.12 format
// R = (+408.583/256)*Cr + (+298.082/256)*Y + ( 000.000/256)*Cb + (-222.921);
// G = (-208.120/256)*Cr + (+298.082/256)*Y + (-100.291/256)*Cb + (+135.576);
// B = ( 000.000/256)*Cr + (+298.082/256)*Y + (+516.412/256)*Cb + (-276.836);

module cf_csc_CrYCb2RGB (

  // Cr-Y-Cb inputs

  clk,
  CrYCb_vs,
  CrYCb_hs,
  CrYCb_de,
  CrYCb_data,

  // R-G-B outputs

  RGB_vs,
  RGB_hs,
  RGB_de,
  RGB_data);

  // Cr-Y-Cb inputs

  input           clk;
  input           CrYCb_vs;
  input           CrYCb_hs;
  input           CrYCb_de;
  input   [23:0]  CrYCb_data;

  // R-G-B outputs

  output          RGB_vs;
  output          RGB_hs;
  output          RGB_de;
  output  [23:0]  RGB_data;

  reg             RGB_vs = 'd0;
  reg             RGB_hs = 'd0;
  reg             RGB_de = 'd0;
  reg     [23:0]  RGB_data = 'd0;

  wire            R_vs_s;
  wire            R_hs_s;
  wire            R_de_s;
  wire    [ 7:0]  R_data_s;
  wire            G_vs_s;
  wire            G_hs_s;
  wire            G_de_s;
  wire    [ 7:0]  G_data_s;
  wire            B_vs_s;
  wire            B_hs_s;
  wire            B_de_s;
  wire    [ 7:0]  B_data_s;

  // output registers (the control signals are gated together)

  always @(posedge clk) begin
    RGB_vs <= R_vs_s & G_vs_s & B_vs_s;
    RGB_hs <= R_hs_s & G_hs_s & B_hs_s;
    RGB_de <= R_de_s & G_de_s & B_de_s;
    RGB_data <= {R_data_s, G_data_s, B_data_s};
  end

  // red

  cf_csc_1 i_csc_R (
    .clk (clk),
    .vs (CrYCb_vs),
    .hs (CrYCb_hs),
    .de (CrYCb_de),
    .data (CrYCb_data),
    .C1 (17'h01989),
    .C2 (17'h012a1),
    .C3 (17'h00000),
    .C4 (25'h10deebc),
    .csc_vs (R_vs_s),
    .csc_hs (R_hs_s),
    .csc_de (R_de_s),
    .csc_data_1 (R_data_s));

  // green

  cf_csc_1 i_csc_G (
    .clk (clk),
    .vs (CrYCb_vs),
    .hs (CrYCb_hs),
    .de (CrYCb_de),
    .data (CrYCb_data),
    .C1 (17'h10d01),
    .C2 (17'h012a1),
    .C3 (17'h10644),
    .C4 (25'h0087937),
    .csc_vs (G_vs_s),
    .csc_hs (G_hs_s),
    .csc_de (G_de_s),
    .csc_data_1 (G_data_s));

  // blue

  cf_csc_1 i_csc_B (
    .clk (clk),
    .vs (CrYCb_vs),
    .hs (CrYCb_hs),
    .de (CrYCb_de),
    .data (CrYCb_data),
    .C1 (17'h00000),
    .C2 (17'h012a1),
    .C3 (17'h02046),
    .C4 (25'h1114d60),
    .csc_vs (B_vs_s),
    .csc_hs (B_hs_s),
    .csc_de (B_de_s),
    .csc_data_1 (B_data_s));

endmodule

// ***************************************************************************
// ***************************************************************************
