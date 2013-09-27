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
// Transmit HDMI, video dma data in, hdmi separate syncs data out.

module cf_hdmi (

  // hdmi interface

  hdmi_clk,
  hdmi_vsync,
  hdmi_hsync,
  hdmi_data_e,
  hdmi_data,
  hdmi_fs_toggle,
  hdmi_raddr_g,
  hdmi_tpm_oos,

  // vdma interface

  vdma_clk,
  vdma_wr,
  vdma_waddr,
  vdma_wdata,
  vdma_fs_ret_toggle,
  vdma_fs_waddr,

  // processor interface

  up_enable,
  up_crcb_init,
  up_tpg_enable,
  up_csc_bypass,
  up_hs_width,
  up_hs_count,
  up_hs_de_min,
  up_hs_de_max,
  up_vs_width,
  up_vs_count,
  up_vs_de_min,
  up_vs_de_max,
  up_cp_en,
  up_cp,

  // debug interface (chipscope)

  debug_data,
  debug_trigger);

  // hdmi interface

  input           hdmi_clk;
  output          hdmi_vsync;
  output          hdmi_hsync;
  output          hdmi_data_e;
  output  [15:0]  hdmi_data;
  output          hdmi_fs_toggle;
  output  [ 8:0]  hdmi_raddr_g;
  output          hdmi_tpm_oos;

  // vdma interface

  input           vdma_clk;
  input           vdma_wr;
  input   [ 8:0]  vdma_waddr;
  input   [47:0]  vdma_wdata;
  input           vdma_fs_ret_toggle;
  input   [ 8:0]  vdma_fs_waddr;

  // processor interface

  input           up_enable;
  input           up_crcb_init;
  input           up_tpg_enable;
  input           up_csc_bypass;
  input   [15:0]  up_hs_width;
  input   [15:0]  up_hs_count;
  input   [15:0]  up_hs_de_min;
  input   [15:0]  up_hs_de_max;
  input   [15:0]  up_vs_width;
  input   [15:0]  up_vs_count;
  input   [15:0]  up_vs_de_min;
  input   [15:0]  up_vs_de_max;
  input           up_cp_en;
  input   [23:0]  up_cp;

  // debug interface (chipscope)

  output  [63:0]  debug_data;
  output  [ 7:0]  debug_trigger;

  reg             hdmi_up_enable_m1 = 'd0;
  reg             hdmi_up_enable_m2 = 'd0;
  reg             hdmi_up_enable_m3 = 'd0;
  reg             hdmi_up_enable = 'd0;
  reg             hdmi_up_crcb_init = 'd0;
  reg             hdmi_up_tpg_enable = 'd0;
  reg             hdmi_up_csc_bypass = 'd0;
  reg     [15:0]  hdmi_up_hs_width = 'd0;
  reg     [15:0]  hdmi_up_hs_count = 'd0;
  reg     [15:0]  hdmi_up_hs_de_min = 'd0;
  reg     [15:0]  hdmi_up_hs_de_max = 'd0;
  reg     [15:0]  hdmi_up_vs_width = 'd0;
  reg     [15:0]  hdmi_up_vs_count = 'd0;
  reg     [15:0]  hdmi_up_vs_de_min = 'd0;
  reg     [15:0]  hdmi_up_vs_de_max = 'd0;
  reg             hdmi_up_cp_en_m1 = 'd0;
  reg             hdmi_up_cp_en_m2 = 'd0;
  reg             hdmi_up_cp_en_m3 = 'd0;
  reg             hdmi_up_cp_en = 'd0;
  reg     [23:0]  hdmi_up_cp = 'd0;
  reg     [15:0]  hdmi_hs_count = 'd0;
  reg     [15:0]  hdmi_vs_count = 'd0;
  reg             hdmi_fs_ret_toggle_m1 = 'd0;
  reg             hdmi_fs_ret_toggle_m2 = 'd0;
  reg             hdmi_fs_ret_toggle_m3 = 'd0;
  reg             hdmi_fs_ret = 'd0;
  reg     [ 8:0]  hdmi_fs_waddr = 'd0;
  reg             hdmi_fs_toggle = 'd0;
  reg     [ 8:0]  hdmi_raddr_g = 'd0;
  reg             hdmi_vs = 'd0;
  reg             hdmi_hs = 'd0;
  reg             hdmi_de = 'd0;
  reg     [ 9:0]  hdmi_raddr = 'd0;
  reg             hdmi_vs_d = 'd0;
  reg             hdmi_hs_d = 'd0;
  reg             hdmi_de_d = 'd0;
  reg             hdmi_data_sel_d = 'd0;
  reg             hdmi_vs_2d = 'd0;
  reg             hdmi_hs_2d = 'd0;
  reg             hdmi_de_2d = 'd0;
  reg             hdmi_data_sel_2d = 'd0;
  reg     [47:0]  hdmi_data_2d = 'd0;
  reg     [23:0]  hdmi_tpm_data = 'd0;
  reg     [ 4:0]  hdmi_tpm_mismatch_count = 'd0;
  reg             hdmi_tpm_oos = 'd0;
  reg             hdmi_vs_444 = 'd0;
  reg             hdmi_hs_444 = 'd0;
  reg             hdmi_de_444 = 'd0;
  reg     [23:0]  hdmi_data_444 = 'd0;
  reg             hdmi_vs_422 = 'd0;
  reg             hdmi_hs_422 = 'd0;
  reg             hdmi_de_422 = 'd0;
  reg     [15:0]  hdmi_data_422 = 'd0;
  reg             hdmi_vsync = 'd0;
  reg             hdmi_hsync = 'd0;
  reg             hdmi_data_e = 'd0;
  reg     [15:0]  hdmi_data = 'd0;

  wire    [15:0]  hdmi_up_hs_count_s;
  wire    [15:0]  hdmi_up_vs_count_s;
  wire            hdmi_fs_s;
  wire            hdmi_hs_s;
  wire            hdmi_vs_s;
  wire            hdmi_hs_de_s;
  wire            hdmi_vs_de_s;
  wire            hdmi_fs_ret_s;
  wire    [47:0]  hdmi_rdata_s;
  wire    [23:0]  hdmi_data_2d_s;
  wire            hdmi_tpm_mismatch_s;
  wire    [23:0]  hdmi_tpg_data_s;
  wire            csc_vs_s;
  wire            csc_hs_s;
  wire            csc_de_s;
  wire    [23:0]  csc_data_s;
  wire            hdmi_vs_422_s;
  wire            hdmi_hs_422_s;
  wire            hdmi_de_422_s;
  wire    [15:0]  hdmi_data_422_s;

  // binary to grey conversion

  function [8:0] b2g;
    input [8:0] b;
    reg   [8:0] g;
    begin
      g[8] = b[8];
      g[7] = b[8] ^ b[7];
      g[6] = b[7] ^ b[6];
      g[5] = b[6] ^ b[5];
      g[4] = b[5] ^ b[4];
      g[3] = b[4] ^ b[3];
      g[2] = b[3] ^ b[2];
      g[1] = b[2] ^ b[1];
      g[0] = b[1] ^ b[0];
      b2g = g;
    end
  endfunction

  // debug ports

  assign debug_data[63:61] = 'd0;
  assign debug_data[60:60] = hdmi_fs_s;
  assign debug_data[59:59] = hdmi_fs_ret_s;
  assign debug_data[58:58] = hdmi_hs_s;
  assign debug_data[57:57] = hdmi_hs_de_s;
  assign debug_data[56:56] = hdmi_vs_s;
  assign debug_data[55:55] = hdmi_vs_de_s;
  assign debug_data[54:54] = hdmi_tpm_mismatch_s;
  assign debug_data[53:53] = hdmi_vs;
  assign debug_data[52:52] = hdmi_hs;
  assign debug_data[51:51] = hdmi_de;
  assign debug_data[50:50] = hdmi_vsync;
  assign debug_data[49:49] = hdmi_hsync;
  assign debug_data[48:48] = hdmi_data_e;
  assign debug_data[47:32] = hdmi_hs_count;
  assign debug_data[31:16] = hdmi_vs_count;
  assign debug_data[15: 0] = hdmi_data;

  assign debug_trigger[7] = hdmi_fs_s;
  assign debug_trigger[6] = hdmi_fs_ret_s;
  assign debug_trigger[5] = hdmi_hs_s;
  assign debug_trigger[4] = hdmi_hs_de_s;
  assign debug_trigger[3] = hdmi_vs_s;
  assign debug_trigger[2] = hdmi_vs_de_s;
  assign debug_trigger[1] = hdmi_tpm_mismatch_s;
  assign debug_trigger[0] = hdmi_data_e;

  // get useful values from the programmed counters, these registers control the
  // video frame size, the timing signals (sync/enable)

  assign hdmi_up_hs_count_s = hdmi_up_hs_count - 1'b1;
  assign hdmi_up_vs_count_s = hdmi_up_vs_count - 1'b1;

  always @(posedge hdmi_clk) begin
    hdmi_up_enable_m1 <= up_enable;
    hdmi_up_enable_m2 <= hdmi_up_enable_m1;
    hdmi_up_enable_m3 <= hdmi_up_enable_m2;
    hdmi_up_enable <= hdmi_up_enable_m3;
    if ((hdmi_up_enable_m2 == 1'b1) && (hdmi_up_enable_m3 == 1'b0)) begin
      hdmi_up_crcb_init <= up_crcb_init;
      hdmi_up_tpg_enable <= up_tpg_enable;
      hdmi_up_csc_bypass <= up_csc_bypass;
      hdmi_up_hs_width <= up_hs_width;
      hdmi_up_hs_count <= up_hs_count;
      hdmi_up_hs_de_min <= up_hs_de_min;
      hdmi_up_hs_de_max <= up_hs_de_max;
      hdmi_up_vs_width <= up_vs_width;
      hdmi_up_vs_count <= up_vs_count;
      hdmi_up_vs_de_min <= up_vs_de_min;
      hdmi_up_vs_de_max <= up_vs_de_max;
    end
    hdmi_up_cp_en_m1 <= up_cp_en;
    hdmi_up_cp_en_m2 <= hdmi_up_cp_en_m1;
    hdmi_up_cp_en_m3 <= hdmi_up_cp_en_m2;
    hdmi_up_cp_en <= hdmi_up_cp_en_m3;
    if ((hdmi_up_cp_en_m2 == 1'b1) && (hdmi_up_cp_en_m3 == 1'b0)) begin
      hdmi_up_cp <= up_cp;
    end
  end

  // hdmi start of frame, this triggers vdma to reading the frame buffers

  assign hdmi_fs_s = ((hdmi_hs_count == 1) && (hdmi_vs_count == hdmi_up_vs_width)) ?
    hdmi_up_enable : 1'b0;

  assign hdmi_hs_s = (hdmi_hs_count < hdmi_up_hs_width) ? hdmi_up_enable : 1'b0;
  assign hdmi_vs_s = (hdmi_vs_count < hdmi_up_vs_width) ? hdmi_up_enable : 1'b0;

  assign hdmi_hs_de_s = ((hdmi_hs_count < hdmi_up_hs_de_min) ||
    (hdmi_hs_count >= hdmi_up_hs_de_max)) ? 1'b0 : hdmi_up_enable;
  assign hdmi_vs_de_s = ((hdmi_vs_count < hdmi_up_vs_de_min) ||
    (hdmi_vs_count >= hdmi_up_vs_de_max)) ? 1'b0 : hdmi_up_enable;

  always @(posedge hdmi_clk) begin
    if (hdmi_hs_count >= hdmi_up_hs_count_s) begin
      hdmi_hs_count <= 0;
    end else begin
      hdmi_hs_count <= hdmi_hs_count + 1'b1;
    end
    if (hdmi_hs_count >= hdmi_up_hs_count_s) begin
      if (hdmi_vs_count >= hdmi_up_vs_count_s) begin
        hdmi_vs_count <= 0;
      end else begin
        hdmi_vs_count <= hdmi_vs_count + 1'b1;
      end
    end
  end

  // returned frame sync from the vdma side is used to reset the start address.

  assign hdmi_fs_ret_s = hdmi_fs_ret_toggle_m2 ^ hdmi_fs_ret_toggle_m3;

  always @(posedge hdmi_clk) begin
    hdmi_fs_ret_toggle_m1 <= vdma_fs_ret_toggle;
    hdmi_fs_ret_toggle_m2 <= hdmi_fs_ret_toggle_m1;
    hdmi_fs_ret_toggle_m3 <= hdmi_fs_ret_toggle_m2;
    hdmi_fs_ret <= hdmi_fs_ret_s;
    if (hdmi_fs_ret_s == 1'b1) begin
      hdmi_fs_waddr <= vdma_fs_waddr;
    end
    if (hdmi_fs_s == 1'b1) begin
      hdmi_fs_toggle <= ~hdmi_fs_toggle;
    end
    hdmi_raddr_g <= b2g(hdmi_raddr[9:1]);
  end

  // control and data pipe line

  always @(posedge hdmi_clk) begin
    hdmi_vs <= hdmi_vs_s;
    hdmi_hs <= hdmi_hs_s;
    hdmi_de <= hdmi_hs_de_s & hdmi_vs_de_s;
    if (hdmi_fs_ret == 1'b1) begin
      hdmi_raddr <= {hdmi_fs_waddr, 1'b0};
    end else if (hdmi_de == 1'b1) begin
      hdmi_raddr <= hdmi_raddr + 1;
    end
    hdmi_vs_d <= hdmi_vs;
    hdmi_hs_d <= hdmi_hs;
    hdmi_de_d <= hdmi_de;
    hdmi_data_sel_d <= hdmi_raddr[0];
    hdmi_vs_2d <= hdmi_vs_d;
    hdmi_hs_2d <= hdmi_hs_d;
    hdmi_de_2d <= hdmi_de_d;
    hdmi_data_sel_2d <= hdmi_data_sel_d;
    hdmi_data_2d <= hdmi_rdata_s;
  end

  // hdmi data count (may be used to monitor or insert)

  assign hdmi_data_2d_s = (hdmi_data_sel_2d == 1'b1) ? hdmi_data_2d[47:24] : hdmi_data_2d[23:0];
  assign hdmi_tpm_mismatch_s = (hdmi_data_2d_s == hdmi_tpm_data) ? 1'b0 : hdmi_de_2d;
  assign hdmi_tpg_data_s = hdmi_tpm_data;

  always @(posedge hdmi_clk) begin
    if (hdmi_fs_ret == 1'b1) begin
      hdmi_tpm_data <= 'd0;
    end else if (hdmi_de_2d == 1'b1) begin
      hdmi_tpm_data <= hdmi_tpm_data + 1'b1;
    end
    if (hdmi_tpm_mismatch_s == 1'b1) begin
      hdmi_tpm_mismatch_count <= 5'h10;
    end else if (hdmi_tpm_mismatch_count[4] == 1'b1) begin
      hdmi_tpm_mismatch_count <= hdmi_tpm_mismatch_count + 1'b1;
    end
    hdmi_tpm_oos <= hdmi_tpm_mismatch_count[4];
  end

  // select test data or dma data before csc/422

  always @(posedge hdmi_clk) begin
    hdmi_vs_444 <= hdmi_vs_2d;
    hdmi_hs_444 <= hdmi_hs_2d;
    hdmi_de_444 <= hdmi_de_2d;
    if (hdmi_up_cp_en == 1'b1) begin
      hdmi_data_444 <= hdmi_up_cp;
    end else if (hdmi_up_tpg_enable == 1'b1) begin
      hdmi_data_444 <= hdmi_tpg_data_s;
    end else begin
      hdmi_data_444 <= hdmi_data_2d_s;
    end
    if (hdmi_up_csc_bypass == 1'b1) begin
      hdmi_vs_422 <= hdmi_vs_444;
      hdmi_hs_422 <= hdmi_hs_444;
      hdmi_de_422 <= hdmi_de_444;
      hdmi_data_422 <= hdmi_data_444[15:0];
    end else begin
      hdmi_vs_422 <= hdmi_vs_422_s;
      hdmi_hs_422 <= hdmi_hs_422_s;
      hdmi_de_422 <= hdmi_de_422_s;
      hdmi_data_422 <= hdmi_data_422_s;
    end
    hdmi_vsync <= hdmi_vs_422;
    hdmi_hsync <= hdmi_hs_422;
    hdmi_data_e <= hdmi_de_422;
    hdmi_data <= hdmi_data_422;
  end

  // data memory

  cf_mem #(.DW(48), .AW(9)) i_mem (
    .clka (vdma_clk),
    .wea (vdma_wr),
    .addra (vdma_waddr),
    .dina (vdma_wdata),
    .clkb (hdmi_clk),
    .addrb (hdmi_raddr[9:1]),
    .doutb (hdmi_rdata_s));

  // color space coversion, RGB to CrYCb

  cf_csc_RGB2CrYCb i_csc (
    .clk (hdmi_clk),
    .RGB_vs (hdmi_vs_444),
    .RGB_hs (hdmi_hs_444),
    .RGB_de (hdmi_de_444),
    .RGB_data (hdmi_data_444),
    .CrYCb_vs (csc_vs_s),
    .CrYCb_hs (csc_hs_s),
    .CrYCb_de (csc_de_s),
    .CrYCb_data (csc_data_s));

  // sub sampling, 444 to 422

  cf_ss_444to422 i_ss (
    .clk (hdmi_clk),
    .s444_vs (csc_vs_s),
    .s444_hs (csc_hs_s),
    .s444_de (csc_de_s),
    .s444_data (csc_data_s),
    .s422_vs (hdmi_vs_422_s),
    .s422_hs (hdmi_hs_422_s),
    .s422_de (hdmi_de_422_s),
    .s422_data (hdmi_data_422_s),
    .Cr_Cb_sel_init (hdmi_up_crcb_init));

endmodule

// ***************************************************************************
// ***************************************************************************
