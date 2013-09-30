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

module cf_hdmi_tx_16b (

  // hdmi interface

  hdmi_clk,
  hdmi_vsync,
  hdmi_hsync,
  hdmi_data_e,
  hdmi_data,

  // vdma interface

  vdma_clk,
  vdma_fs,
  vdma_fs_ret,
  vdma_valid,
  vdma_be,
  vdma_data,
  vdma_last,
  vdma_ready,

  // processor interface

  up_rstn,
  up_clk,
  up_sel,
  up_rwn,
  up_addr,
  up_wdata,
  up_rdata,
  up_ack,
  up_status,

  // debug interface (chipscope)

  debug_data,
  debug_trigger);

  // hdmi interface

  input           hdmi_clk;
  output          hdmi_vsync;
  output          hdmi_hsync;
  output          hdmi_data_e;
  output  [15:0]  hdmi_data;

  // vdma interface

  input           vdma_clk;
  output          vdma_fs;
  input           vdma_fs_ret;
  input           vdma_valid;
  input   [ 7:0]  vdma_be;
  input   [63:0]  vdma_data;
  input           vdma_last;
  output          vdma_ready;

  // processor interface

  input           up_rstn;
  input           up_clk;
  input           up_sel;
  input           up_rwn;
  input   [ 4:0]  up_addr;
  input   [31:0]  up_wdata;
  output  [31:0]  up_rdata;
  output          up_ack;
  output  [ 7:0]  up_status;

  // debug interface (chipscope)

  output  [63:0]  debug_data;
  output  [ 7:0]  debug_trigger;

  reg             up_crcb_init = 'd0;
  reg             up_tpg_enable = 'd0;
  reg             up_csc_bypass = 'd0;
  reg             up_enable = 'd0;
  reg     [15:0]  up_hs_width = 'd0;
  reg     [15:0]  up_hs_count = 'd0;
  reg     [15:0]  up_hs_de_min = 'd0;
  reg     [15:0]  up_hs_de_max = 'd0;
  reg     [15:0]  up_vs_width = 'd0;
  reg     [15:0]  up_vs_count = 'd0;
  reg     [15:0]  up_vs_de_min = 'd0;
  reg     [15:0]  up_vs_de_max = 'd0;
  reg             up_hdmi_tpm_oos_hold = 'd0;
  reg             up_vdma_tpm_oos_hold = 'd0;
  reg             up_vdma_be_error_hold = 'd0;
  reg             up_vdma_ovf_hold = 'd0;
  reg             up_vdma_unf_hold = 'd0;
  reg             up_cp_en = 'd0;
  reg     [23:0]  up_cp = 'd0;
  reg     [ 7:0]  up_status = 'd0;
  reg     [31:0]  up_rdata = 'd0;
  reg             up_sel_d = 'd0;
  reg             up_sel_2d = 'd0;
  reg             up_ack = 'd0;
  reg             up_hdmi_tpm_oos_m1 = 'd0;
  reg             up_vdma_tpm_oos_m1 = 'd0;
  reg             up_vdma_be_error_m1 = 'd0;
  reg             up_vdma_ovf_m1 = 'd0;
  reg             up_vdma_unf_m1 = 'd0;
  reg             up_hdmi_tpm_oos = 'd0;
  reg             up_vdma_tpm_oos = 'd0;
  reg             up_vdma_be_error = 'd0;
  reg             up_vdma_ovf = 'd0;
  reg             up_vdma_unf = 'd0;

  wire            up_wr_s;
  wire            up_rd_s;
  wire            up_ack_s;
  wire            vdma_wr_s;
  wire    [ 8:0]  vdma_waddr_s;
  wire    [47:0]  vdma_wdata_s;
  wire            vdma_fs_ret_toggle_s;
  wire    [ 8:0]  vdma_fs_waddr_s;
  wire            vdma_tpm_oos_s;
  wire            vdma_be_error_s;
  wire            vdma_ovf_s;
  wire            vdma_unf_s;
  wire    [63:0]  vdma_debug_data_s;
  wire    [ 7:0]  vdma_debug_trigger_s;
  wire            hdmi_fs_toggle_s;
  wire    [ 8:0]  hdmi_raddr_g_s;
  wire            hdmi_tpm_oos_s;
  wire    [63:0]  hdmi_debug_data_s;
  wire    [ 7:0]  hdmi_debug_trigger_s;

  assign debug_data = vdma_debug_data_s;
  assign debug_trigger = vdma_debug_trigger_s;

  // processor write interface (see regmap.txt for details)

  assign up_wr_s = up_sel & ~up_rwn;
  assign up_rd_s = up_sel & up_rwn;
  assign up_ack_s = up_sel_d & ~up_sel_2d;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_crcb_init <= 'd0;
      up_tpg_enable <= 'd0;
      up_csc_bypass <= 'd0;
      up_enable <= 'd0;
      up_hs_width <= 'd0;
      up_hs_count <= 'd0;
      up_hs_de_min <= 'd0;
      up_hs_de_max <= 'd0;
      up_vs_width <= 'd0;
      up_vs_count <= 'd0;
      up_vs_de_min <= 'd0;
      up_vs_de_max <= 'd0;
      up_hdmi_tpm_oos_hold <= 'd0;
      up_vdma_tpm_oos_hold <= 'd0;
      up_vdma_be_error_hold <= 'd0;
      up_vdma_ovf_hold <= 'd0;
      up_vdma_unf_hold <= 'd0;
      up_cp_en <= 'd0;
      up_cp <= 'd0;
      up_status <= 'd0;
    end else begin
      if ((up_addr == 5'h01) && (up_wr_s == 1'b1)) begin
        up_crcb_init <= up_wdata[3];
        up_tpg_enable <= up_wdata[2];
        up_csc_bypass <= up_wdata[1];
        up_enable <= up_wdata[0];
      end
      if ((up_addr == 5'h02) && (up_wr_s == 1'b1)) begin
        up_hs_width <= up_wdata[31:16];
        up_hs_count <= up_wdata[15:0];
      end
      if ((up_addr == 5'h03) && (up_wr_s == 1'b1)) begin
        up_hs_de_min <= up_wdata[31:16];
        up_hs_de_max <= up_wdata[15:0];
      end
      if ((up_addr == 5'h04) && (up_wr_s == 1'b1)) begin
        up_vs_width <= up_wdata[31:16];
        up_vs_count <= up_wdata[15:0];
      end
      if ((up_addr == 5'h05) && (up_wr_s == 1'b1)) begin
        up_vs_de_min <= up_wdata[31:16];
        up_vs_de_max <= up_wdata[15:0];
      end
      if (up_hdmi_tpm_oos == 1'b1) begin
        up_hdmi_tpm_oos_hold <= 1'b1;
      end else if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        up_hdmi_tpm_oos_hold <= up_hdmi_tpm_oos_hold & (~up_wdata[4]);
      end
      if (up_vdma_tpm_oos == 1'b1) begin
        up_vdma_tpm_oos_hold <= 1'b1;
      end else if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        up_vdma_tpm_oos_hold <= up_vdma_tpm_oos_hold & (~up_wdata[3]);
      end
      if (up_vdma_be_error == 1'b1) begin
        up_vdma_be_error_hold <= 1'b1;
      end else if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        up_vdma_be_error_hold <= up_vdma_be_error_hold & (~up_wdata[2]);
      end
      if (up_vdma_ovf == 1'b1) begin
        up_vdma_ovf_hold <= 1'b1;
      end else if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        up_vdma_ovf_hold <= up_vdma_ovf_hold & (~up_wdata[1]);
      end
      if (up_vdma_unf == 1'b1) begin
        up_vdma_unf_hold <= 1'b1;
      end else if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        up_vdma_unf_hold <= up_vdma_unf_hold & (~up_wdata[0]);
      end
      if ((up_addr == 5'h07) && (up_wr_s == 1'b1)) begin
        up_cp_en <= up_wdata[24];
        up_cp <= up_wdata[23:0];
      end
      up_status <= {up_enable, up_tpg_enable, up_csc_bypass, up_vdma_be_error_hold,
        up_hdmi_tpm_oos_hold, up_vdma_tpm_oos_hold, up_vdma_ovf_hold, up_vdma_unf_hold};
    end
  end

  // processor read interface

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_rdata <= 'd0;
      up_sel_d <= 'd0;
      up_sel_2d <= 'd0;
      up_ack <= 'd0;
    end else begin
      case (up_addr)
        5'h00: up_rdata <= 32'h00010061;
        5'h01: up_rdata <= {28'd0, up_crcb_init, up_tpg_enable, up_csc_bypass, up_enable};
        5'h02: up_rdata <= {up_hs_width, up_hs_count};
        5'h03: up_rdata <= {up_hs_de_min, up_hs_de_max};
        5'h04: up_rdata <= {up_vs_width, up_vs_count};
        5'h05: up_rdata <= {up_vs_de_min, up_vs_de_max};
        5'h06: up_rdata <= {28'd0, up_hdmi_tpm_oos_hold, up_vdma_tpm_oos_hold,
          up_vdma_be_error_hold, up_vdma_ovf_hold, up_vdma_unf_hold};
        5'h07: up_rdata <= {7'd0, up_cp_en, up_cp};
        default: up_rdata <= 0;
      endcase
      up_sel_d <= up_sel;
      up_sel_2d <= up_sel_d;
      up_ack <= up_ack_s;
    end
  end

  // the hdmi status signals transferred to the processor clock domain

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_hdmi_tpm_oos_m1 <= 'd0;
      up_vdma_tpm_oos_m1 <= 'd0;
      up_vdma_be_error_m1 <= 'd0;
      up_vdma_ovf_m1 <= 'd0;
      up_vdma_unf_m1 <= 'd0;
      up_hdmi_tpm_oos <= 'd0;
      up_vdma_tpm_oos <= 'd0;
      up_vdma_be_error <= 'd0;
      up_vdma_ovf <= 'd0;
      up_vdma_unf <= 'd0;
    end else begin
      up_hdmi_tpm_oos_m1 <= hdmi_tpm_oos_s;
      up_vdma_tpm_oos_m1 <= vdma_tpm_oos_s;
      up_vdma_be_error_m1 <= vdma_be_error_s;
      up_vdma_ovf_m1 <= vdma_ovf_s;
      up_vdma_unf_m1 <= vdma_unf_s;
      up_hdmi_tpm_oos <= up_hdmi_tpm_oos_m1;
      up_vdma_tpm_oos <= up_vdma_tpm_oos_m1;
      up_vdma_be_error <= up_vdma_be_error_m1;
      up_vdma_ovf <= up_vdma_ovf_m1;
      up_vdma_unf <= up_vdma_unf_m1;
    end
  end

  // vdma interface

  cf_vdma i_vdma (
    .hdmi_fs_toggle (hdmi_fs_toggle_s),
    .hdmi_raddr_g (hdmi_raddr_g_s),
    .vdma_clk (vdma_clk),
    .vdma_fs (vdma_fs),
    .vdma_fs_ret (vdma_fs_ret),
    .vdma_valid (vdma_valid),
    .vdma_be (vdma_be),
    .vdma_data (vdma_data),
    .vdma_last (vdma_last),
    .vdma_ready (vdma_ready),
    .vdma_wr (vdma_wr_s),
    .vdma_waddr (vdma_waddr_s),
    .vdma_wdata (vdma_wdata_s),
    .vdma_fs_ret_toggle (vdma_fs_ret_toggle_s),
    .vdma_fs_waddr (vdma_fs_waddr_s),
    .vdma_tpm_oos (vdma_tpm_oos_s),
    .vdma_be_error (vdma_be_error_s),
    .vdma_ovf (vdma_ovf_s),
    .vdma_unf (vdma_unf_s),
    .debug_data (vdma_debug_data_s),
    .debug_trigger (vdma_debug_trigger_s));

  // hdmi interface

  cf_hdmi i_hdmi (
    .hdmi_clk (hdmi_clk),
    .hdmi_vsync (hdmi_vsync),
    .hdmi_hsync (hdmi_hsync),
    .hdmi_data_e (hdmi_data_e),
    .hdmi_data (hdmi_data),
    .hdmi_fs_toggle (hdmi_fs_toggle_s),
    .hdmi_raddr_g (hdmi_raddr_g_s),
    .hdmi_tpm_oos (hdmi_tpm_oos_s),
    .vdma_clk (vdma_clk),
    .vdma_wr (vdma_wr_s),
    .vdma_waddr (vdma_waddr_s),
    .vdma_wdata (vdma_wdata_s),
    .vdma_fs_ret_toggle (vdma_fs_ret_toggle_s),
    .vdma_fs_waddr (vdma_fs_waddr_s),
    .up_enable (up_enable),
    .up_crcb_init (up_crcb_init),
    .up_tpg_enable (up_tpg_enable),
    .up_csc_bypass (up_csc_bypass),
    .up_hs_width (up_hs_width),
    .up_hs_count (up_hs_count),
    .up_hs_de_min (up_hs_de_min),
    .up_hs_de_max (up_hs_de_max),
    .up_vs_width (up_vs_width),
    .up_vs_count (up_vs_count),
    .up_vs_de_min (up_vs_de_min),
    .up_vs_de_max (up_vs_de_max),
    .up_cp_en (up_cp_en),
    .up_cp (up_cp),
    .debug_data (hdmi_debug_data_s),
    .debug_trigger (hdmi_debug_trigger_s));

endmodule

// ***************************************************************************
// ***************************************************************************
