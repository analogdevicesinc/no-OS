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
// Transmit HDMI, RGB to CrYCb conversion
// The multiplication coefficients are in 1.4.12 format
// The addition coefficients are in 1.12.12 format
// Cr = (+112.439/256)*R + (-094.154/256)*G + (-018.285/256)*B + 128;
// Y  = (+065.738/256)*R + (+129.057/256)*G + (+025.064/256)*B +  16;
// Cb = (-037.945/256)*R + (-074.494/256)*G + (+112.439/256)*B + 128;

module cf_csc_RGB2CrYCb (

  // R-G-B inputs

  clk,
  RGB_vs,
  RGB_hs,
  RGB_de,
  RGB_data,

  // Cr-Y-Cb outputs

  CrYCb_vs,
  CrYCb_hs,
  CrYCb_de,
  CrYCb_data);

  // R-G-B inputs

  input           clk;
  input           RGB_vs;
  input           RGB_hs;
  input           RGB_de;
  input   [23:0]  RGB_data;

  // Cr-Y-Cb outputs

  output          CrYCb_vs;
  output          CrYCb_hs;
  output          CrYCb_de;
  output  [23:0]  CrYCb_data;

  reg             CrYCb_vs = 'd0;
  reg             CrYCb_hs = 'd0;
  reg             CrYCb_de = 'd0;
  reg     [23:0]  CrYCb_data = 'd0;

  wire            Cr_vs_s;
  wire            Cr_hs_s;
  wire            Cr_de_s;
  wire    [ 7:0]  Cr_data_s;
  wire            Y_vs_s;
  wire            Y_hs_s;
  wire            Y_de_s;
  wire    [ 7:0]  Y_data_s;
  wire            Cb_vs_s;
  wire            Cb_hs_s;
  wire            Cb_de_s;
  wire    [ 7:0]  Cb_data_s;

  // output registers (the control signals are gated together)

  always @(posedge clk) begin
    CrYCb_vs <= Cr_vs_s & Y_vs_s & Cb_vs_s;
    CrYCb_hs <= Cr_hs_s & Y_hs_s & Cb_hs_s;
    CrYCb_de <= Cr_de_s & Y_de_s & Cb_de_s;
    CrYCb_data <= {Cr_data_s, Y_data_s, Cb_data_s};
  end

  // Cr (red-diff)

  cf_csc_1 i_csc_Cr (
    .clk (clk),
    .vs (RGB_vs),
    .hs (RGB_hs),
    .de (RGB_de),
    .data (RGB_data),
    .C1 (17'h00707),
    .C2 (17'h105e2),
    .C3 (17'h10124),
    .C4 (25'h0080000),
    .csc_vs (Cr_vs_s),
    .csc_hs (Cr_hs_s),
    .csc_de (Cr_de_s),
    .csc_data_1 (Cr_data_s));

  // Y (luma)

  cf_csc_1 i_csc_Y (
    .clk (clk),
    .vs (RGB_vs),
    .hs (RGB_hs),
    .de (RGB_de),
    .data (RGB_data),
    .C1 (17'h0041b),
    .C2 (17'h00810),
    .C3 (17'h00191),
    .C4 (25'h0010000),
    .csc_vs (Y_vs_s),
    .csc_hs (Y_hs_s),
    .csc_de (Y_de_s),
    .csc_data_1 (Y_data_s));

  // Cb (blue-diff)

  cf_csc_1 i_csc_Cb (
    .clk (clk),
    .vs (RGB_vs),
    .hs (RGB_hs),
    .de (RGB_de),
    .data (RGB_data),
    .C1 (17'h1025f),
    .C2 (17'h104a7),
    .C3 (17'h00707),
    .C4 (25'h0080000),
    .csc_vs (Cb_vs_s),
    .csc_hs (Cb_hs_s),
    .csc_de (Cb_de_s),
    .csc_data_1 (Cb_data_s));

endmodule

// ***************************************************************************
// ***************************************************************************
