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
// Input must be RGB or CrYCb in that order, output is CrY/CbY

module cf_ss_422to444 (

  // 422 inputs

  clk,
  s422_vs,
  s422_hs,
  s422_de,
  s422_data,

  // 444 outputs

  s444_vs,
  s444_hs,
  s444_de,
  s444_data,

  // change to switch Cr/Cb sel (0-> Cb first, 1-> Cr first)

  Cr_Cb_sel_init);

  input           clk;
  input           s422_vs;
  input           s422_hs;
  input           s422_de;
  input   [15:0]  s422_data;

  output          s444_vs;
  output          s444_hs;
  output          s444_de;
  output  [23:0]  s444_data;

  input           Cr_Cb_sel_init;

  reg             Cr_Cb_sel = 'd0;
  reg             s422_vs_d = 'd0;
  reg             s422_hs_d = 'd0;
  reg             s422_de_d = 'd0;
  reg     [23:0]  s422_data_d = 'd0;
  reg             s422_vs_2d = 'd0;
  reg             s422_hs_2d = 'd0;
  reg             s422_de_2d = 'd0;
  reg     [23:0]  s422_data_2d = 'd0;
  reg             s422_vs_3d = 'd0;
  reg             s422_hs_3d = 'd0;
  reg             s422_de_3d = 'd0;
  reg     [23:0]  s422_data_3d = 'd0;
  reg     [ 7:0]  R = 'd0;
  reg     [ 7:0]  B = 'd0;
  reg             s444_vs = 'd0;
  reg             s444_hs = 'd0;
  reg             s444_de = 'd0;
  reg     [23:0]  s444_data = 'd0;

  wire    [ 9:0]  R_s;
  wire    [ 9:0]  B_s;

  // First data on DE assertion is Cb (0x0), then Cr (0x1).
  // The previous data is held when not current

  always @(posedge clk) begin
    if (s422_de == 1'b1) begin
      Cr_Cb_sel <= ~Cr_Cb_sel;
    end else begin
      Cr_Cb_sel <= Cr_Cb_sel_init;
    end
    s422_vs_d <= s422_vs;
    s422_hs_d <= s422_hs;
    s422_de_d <= s422_de;
    if (s422_de == 1'b1) begin
      if (Cr_Cb_sel == 1'b1) begin
        s422_data_d <= {s422_data[15:8], s422_data[7:0], s422_data_d[7:0]};
      end else begin
        s422_data_d <= {s422_data_d[23:16], s422_data[7:0], s422_data[15:8]};
      end
    end
    s422_vs_2d <= s422_vs_d;
    s422_hs_2d <= s422_hs_d;
    s422_de_2d <= s422_de_d;
    if (s422_de_d == 1'b1) begin
      s422_data_2d <= s422_data_d;
    end
    s422_vs_3d <= s422_vs_2d;
    s422_hs_3d <= s422_hs_2d;
    s422_de_3d <= s422_de_2d;
    if (s422_de_2d == 1'b1) begin
      s422_data_3d <= s422_data_2d;
    end
  end

  // Get the average 0.4*S(n-1) + 0.2*S(n) + 0.2*S(n+1)

  assign R_s = {2'd0, s422_data_d[23:16]} + {2'd0, s422_data_3d[23:16]} +
    {1'd0, s422_data_2d[23:16], 1'd0};

  assign B_s = {2'd0, s422_data_d[7:0]} + {2'd0, s422_data_3d[7:0]} +
    {1'd0, s422_data_2d[7:0], 1'd0};

  always @(posedge clk) begin
    R <= R_s[9:2];
    B <= B_s[9:2];
  end

  // 444 outputs

  always @(posedge clk) begin
    s444_vs <= s422_vs_3d;
    s444_hs <= s422_hs_3d;
    s444_de <= s422_de_3d;
    if (s422_de_3d == 1'b0) begin
      s444_data <= 'd0;
    end else begin
      s444_data <= {R, s422_data_3d[15:8], B};
    end
  end

endmodule

// ***************************************************************************
// ***************************************************************************
