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

`timescale 1ns/100ps

module up_adc_common (

  // clock reset

  mmcm_rst,

  // adc interface

  adc_clk,
  adc_rst,
  adc_r1_mode,
  adc_ddr_edgesel,
  adc_pin_mode,
  adc_status,
  adc_status_pn_err,
  adc_status_pn_oos,
  adc_status_or,
  adc_clk_ratio,

  // delay interface

  delay_clk,
  delay_rst,
  delay_sel,
  delay_rwn,
  delay_addr,
  delay_wdata,
  delay_rdata,
  delay_ack_t,
  delay_locked,

  // drp interface

  drp_clk,
  drp_rst,
  drp_sel,
  drp_rwn,
  drp_addr,
  drp_wdata,
  drp_rdata,
  drp_ack_t,

  // dma interface

  dma_clk,
  dma_rst,
  dma_start,
  dma_stream,
  dma_count,
  dma_ovf,
  dma_unf,
  dma_status,
  dma_bw,

  // user channel control

  up_usr_chanmax,
  adc_usr_chanmax,

  // bus interface

  up_rstn,
  up_clk,
  up_sel,
  up_wr,
  up_addr,
  up_wdata,
  up_rdata,
  up_ack);

  // parameters

  parameter   PCORE_VERSION = 32'h00040062;
  parameter   PCORE_ID = 0;

  // clock reset

  output          mmcm_rst;

  // adc interface

  input           adc_clk;
  output          adc_rst;
  output          adc_r1_mode;
  output          adc_ddr_edgesel;
  output          adc_pin_mode;
  input           adc_status;
  input           adc_status_pn_err;
  input           adc_status_pn_oos;
  input           adc_status_or;
  input   [31:0]  adc_clk_ratio;

  // delay interface

  input           delay_clk;
  output          delay_rst;
  output          delay_sel;
  output          delay_rwn;
  output  [ 7:0]  delay_addr;
  output  [ 4:0]  delay_wdata;
  input   [ 4:0]  delay_rdata;
  input           delay_ack_t;
  input           delay_locked;

  // drp interface

  input           drp_clk;
  output          drp_rst;
  output          drp_sel;
  output          drp_rwn;
  output  [11:0]  drp_addr;
  output  [15:0]  drp_wdata;
  input   [15:0]  drp_rdata;
  input           drp_ack_t;

  // dma interface

  input           dma_clk;
  output          dma_rst;
  output          dma_start;
  output          dma_stream;
  output  [31:0]  dma_count;
  input           dma_ovf;
  input           dma_unf;
  input           dma_status;
  input   [31:0]  dma_bw;

  // user channel control

  output  [ 7:0]  up_usr_chanmax;
  input   [ 7:0]  adc_usr_chanmax;

  // bus interface

  input           up_rstn;
  input           up_clk;
  input           up_sel;
  input           up_wr;
  input   [13:0]  up_addr;
  input   [31:0]  up_wdata;
  output  [31:0]  up_rdata;
  output          up_ack;

  // internal registers

  reg     [31:0]  up_scratch = 'd0;
  reg             up_mmcm_resetn = 'd0;
  reg             up_resetn = 'd0;
  reg             up_adc_r1_mode = 'd0;
  reg             up_adc_ddr_edgesel = 'd0;
  reg             up_adc_pin_mode = 'd0;
  reg             up_delay_sel = 'd0;
  reg             up_delay_rwn = 'd0;
  reg     [ 7:0]  up_delay_addr = 'd0;
  reg     [ 4:0]  up_delay_wdata = 'd0;
  reg             up_drp_sel = 'd0;
  reg             up_drp_rwn = 'd0;
  reg     [11:0]  up_drp_addr = 'd0;
  reg     [15:0]  up_drp_wdata = 'd0;
  reg             up_dma_stream = 'd0;
  reg             up_dma_start = 'd0;
  reg     [31:0]  up_dma_count = 'd0;
  reg     [ 7:0]  up_usr_chanmax = 'd0;
  reg             up_ack = 'd0;
  reg     [31:0]  up_rdata = 'd0;
  reg             adc_r1_mode_m1 = 'd0;
  reg             adc_r1_mode = 'd0;
  reg             adc_ddr_edgesel_m1 = 'd0;
  reg             adc_ddr_edgesel = 'd0;
  reg             adc_pin_mode_m1 = 'd0;
  reg             adc_pin_mode = 'd0;
  reg     [ 3:0]  up_adc_status_m1 = 'd0;
  reg     [ 3:0]  up_adc_status = 'd0;
  reg             up_count_toggle_m1 = 'd0;
  reg             up_count_toggle_m2 = 'd0;
  reg             up_count_toggle_m3 = 'd0;
  reg             up_count_toggle = 'd0;
  reg     [15:0]  up_count = 'd0;
  reg     [31:0]  up_adc_clk_count = 'd0;
  reg             adc_clk_count_toggle_m1 = 'd0;
  reg             adc_clk_count_toggle_m2 = 'd0;
  reg             adc_clk_count_toggle_m3 = 'd0;
  reg             adc_clk_count_toggle = 'd0;
  reg     [31:0]  adc_clk_count_hold = 'd0;
  reg     [32:0]  adc_clk_count = 'd0;
  reg             delay_sel_m1 = 'd0;
  reg             delay_sel_m2 = 'd0;
  reg             delay_sel_m3 = 'd0;
  reg             delay_sel = 'd0;
  reg             delay_rwn = 'd0;
  reg     [ 7:0]  delay_addr = 'd0;
  reg     [ 4:0]  delay_wdata = 'd0;
  reg             up_delay_locked_m1 = 'd0;
  reg             up_delay_locked = 'd0;
  reg             up_delay_ack_t_m1 = 'd0;
  reg             up_delay_ack_t_m2 = 'd0;
  reg             up_delay_ack_t_m3 = 'd0;
  reg             up_delay_sel_d = 'd0;
  reg             up_delay_status = 'd0;
  reg     [ 4:0]  up_delay_rdata = 'd0;
  reg             drp_sel_m1 = 'd0;
  reg             drp_sel_m2 = 'd0;
  reg             drp_sel_m3 = 'd0;
  reg             drp_sel = 'd0;
  reg             drp_rwn = 'd0;
  reg     [11:0]  drp_addr = 'd0;
  reg     [15:0]  drp_wdata = 'd0;
  reg             up_drp_ack_t_m1 = 'd0;
  reg             up_drp_ack_t_m2 = 'd0;
  reg             up_drp_ack_t_m3 = 'd0;
  reg             up_drp_sel_d = 'd0;
  reg             up_drp_status = 'd0;
  reg     [15:0]  up_drp_rdata = 'd0;
  reg             dma_start_m1 = 'd0;
  reg             dma_start_m2 = 'd0;
  reg             dma_start_m3 = 'd0;
  reg             dma_start = 'd0;
  reg             dma_stream = 'd0;
  reg     [31:0]  dma_count = 'd0;
  reg     [ 5:0]  dma_xfer_cnt = 'd0;
  reg             dma_xfer_toggle = 'd0;
  reg             dma_xfer_ovf = 'd0;
  reg             dma_xfer_unf = 'd0;
  reg             dma_acc_ovf = 'd0;
  reg             dma_acc_unf = 'd0;
  reg             up_dma_xfer_toggle_m1 = 'd0;
  reg             up_dma_xfer_toggle_m2 = 'd0;
  reg             up_dma_xfer_toggle_m3 = 'd0;
  reg             up_dma_xfer_ovf = 'd0;
  reg             up_dma_xfer_unf = 'd0;
  reg             up_dma_ovf = 'd0;
  reg             up_dma_unf = 'd0;
  reg             up_dma_status_m1 = 'd0;
  reg             up_dma_status = 'd0;

  // internal signals

  wire            up_sel_s;
  wire            up_wr_s;
  wire            up_preset_s;
  wire            up_mmcm_preset_s;
  wire    [ 3:0]  adc_status_s;
  wire            up_count_toggle_s;
  wire            adc_clk_count_toggle_s;
  wire            up_delay_ack_t_s;
  wire            up_delay_sel_s;
  wire            up_drp_ack_t_s;
  wire            up_drp_sel_s;
  wire            up_dma_xfer_toggle_s;

  // decode block select

  assign up_sel_s = (up_addr[13:8] == 6'h00) ? up_sel : 1'b0;
  assign up_wr_s = up_sel_s & up_wr;
  assign up_preset_s = ~up_resetn;
  assign up_mmcm_preset_s = ~up_mmcm_resetn;

  // processor write interface

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_scratch <= 'd0;
      up_mmcm_resetn <= 'd0;
      up_resetn <= 'd0;
      up_adc_r1_mode <= 'd0;
      up_adc_ddr_edgesel <= 'd0;
      up_adc_pin_mode <= 'd0;
      up_delay_sel <= 'd0;
      up_delay_rwn <= 'd0;
      up_delay_addr <= 'd0;
      up_delay_wdata <= 'd0;
      up_drp_sel <= 'd0;
      up_drp_rwn <= 'd0;
      up_drp_addr <= 'd0;
      up_drp_wdata <= 'd0;
      up_dma_stream <= 'd0;
      up_dma_start <= 'd0;
      up_dma_count <= 'd0;
      up_usr_chanmax <= 'd0;
    end else begin
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h02)) begin
        up_scratch <= up_wdata;
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h10)) begin
        up_mmcm_resetn <= up_wdata[1];
        up_resetn <= up_wdata[0];
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h11)) begin
        up_adc_r1_mode <= up_wdata[2];
        up_adc_ddr_edgesel <= up_wdata[1];
        up_adc_pin_mode <= up_wdata[0];
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h18)) begin
        up_delay_sel <= up_wdata[17];
        up_delay_rwn <= up_wdata[16];
        up_delay_addr <= up_wdata[15:8];
        up_delay_wdata <= up_wdata[4:0];
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h1c)) begin
        up_drp_sel <= up_wdata[29];
        up_drp_rwn <= up_wdata[28];
        up_drp_addr <= up_wdata[27:16];
        up_drp_wdata <= up_wdata[15:0];
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h20)) begin
        up_dma_stream <= up_wdata[1];
        up_dma_start <= up_wdata[0];
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h21)) begin
        up_dma_count <= up_wdata;
      end
      if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h28)) begin
        up_usr_chanmax <= up_wdata[7:0];
      end
    end
  end

  // processor read interface

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_ack <= 'd0;
      up_rdata <= 'd0;
    end else begin
      up_ack <= up_sel_s;
      if (up_sel_s == 1'b1) begin
        case (up_addr[7:0])
          8'h00: up_rdata <= PCORE_VERSION;
          8'h01: up_rdata <= PCORE_ID;
          8'h02: up_rdata <= up_scratch;
          8'h10: up_rdata <= {30'd0, up_mmcm_resetn, up_resetn};
          8'h11: up_rdata <= {29'd0, up_adc_r1_mode, up_adc_ddr_edgesel, up_adc_pin_mode};
          8'h15: up_rdata <= up_adc_clk_count;
          8'h16: up_rdata <= adc_clk_ratio;
          8'h17: up_rdata <= {28'd0, up_adc_status};
          8'h18: up_rdata <= {14'd0, up_delay_sel, up_delay_rwn, up_delay_addr, 3'd0, up_delay_wdata};
          8'h19: up_rdata <= {22'd0, up_delay_locked, up_delay_status, 3'd0, up_delay_rdata};
          8'h1c: up_rdata <= {2'd0, up_drp_sel, up_drp_rwn, up_drp_addr, up_drp_wdata};
          8'h1d: up_rdata <= {15'd0, up_drp_status, up_drp_rdata};
          8'h20: up_rdata <= {30'd0, up_dma_stream, up_dma_start};
          8'h21: up_rdata <= up_dma_count;
          8'h22: up_rdata <= {29'd0, up_dma_ovf, up_dma_unf, up_dma_status};
          8'h23: up_rdata <= dma_bw;
          8'h28: up_rdata <= {24'd0, adc_usr_chanmax};
          default: up_rdata <= 0;
        endcase
      end else begin
        up_rdata <= 32'd0;
      end
    end
  end

  // MMCM CONTROL

  FDPE #(.INIT(1'b1)) i_mmcm_rst_reg (
    .CE (1'b1),
    .D (1'b0),
    .PRE (up_mmcm_preset_s),
    .C (drp_clk),
    .Q (mmcm_rst));

  // ADC CONTROL

  FDPE #(.INIT(1'b1)) i_adc_rst_reg (
    .CE (1'b1),
    .D (1'b0),
    .PRE (up_preset_s),
    .C (adc_clk),
    .Q (adc_rst));

  // adc control transfer

  always @(posedge adc_clk) begin
    if (adc_rst == 1'b1) begin
      adc_r1_mode_m1 <= 'd0;
      adc_r1_mode <= 'd0;
      adc_ddr_edgesel_m1 <= 'd0;
      adc_ddr_edgesel <= 'd0;
      adc_pin_mode_m1 <= 'd0;
      adc_pin_mode <= 'd0;
    end else begin
      adc_r1_mode_m1 <= up_adc_r1_mode;
      adc_r1_mode <= adc_r1_mode_m1;
      adc_ddr_edgesel_m1 <= up_adc_ddr_edgesel;
      adc_ddr_edgesel <= adc_ddr_edgesel_m1;
      adc_pin_mode_m1 <= up_adc_pin_mode;
      adc_pin_mode <= adc_pin_mode_m1;
    end
  end

  // adc status transfer

  assign adc_status_s[3] = adc_status_pn_err;
  assign adc_status_s[2] = adc_status_pn_oos;
  assign adc_status_s[1] = adc_status_or;
  assign adc_status_s[0] = adc_status;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_adc_status_m1 <= 'd0;
      up_adc_status <= 'd0;
    end else begin
      up_adc_status_m1 <= adc_status_s;
      up_adc_status <= up_adc_status_m1;
    end
  end

  // processor base reference

  assign up_count_toggle_s = up_count_toggle_m3 ^ up_count_toggle_m2;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_count_toggle_m1 <= 'd0;
      up_count_toggle_m2 <= 'd0;
      up_count_toggle_m3 <= 'd0;
      up_count_toggle <= 'd0;
      up_count <= 'd0;
      up_adc_clk_count <= 'd0;
    end else begin
      up_count_toggle_m1 <= adc_clk_count_toggle;
      up_count_toggle_m2 <= up_count_toggle_m1;
      up_count_toggle_m3 <= up_count_toggle_m2;
      if (up_count == 16'd0) begin
        up_count_toggle <= ~up_count_toggle;
      end
      up_count <= up_count + 1'b1;
      if (up_count_toggle_s == 1'b1) begin
        up_adc_clk_count <= adc_clk_count_hold;
      end
    end
  end

  // measuring clock

  assign adc_clk_count_toggle_s = adc_clk_count_toggle_m3 ^ adc_clk_count_toggle_m2;

  always @(posedge adc_clk) begin
    if (adc_rst == 1'b1) begin
      adc_clk_count_toggle_m1 <= 'd0;
      adc_clk_count_toggle_m2 <= 'd0;
      adc_clk_count_toggle_m3 <= 'd0;
    end else begin
      adc_clk_count_toggle_m1 <= up_count_toggle;
      adc_clk_count_toggle_m2 <= adc_clk_count_toggle_m1;
      adc_clk_count_toggle_m3 <= adc_clk_count_toggle_m2;
    end
    if (adc_clk_count_toggle_s == 1'b1) begin
      adc_clk_count_toggle <= ~adc_clk_count_toggle;
      adc_clk_count_hold <= adc_clk_count[31:0];
    end
    if (adc_clk_count_toggle_s == 1'b1) begin
      adc_clk_count <= 33'd1;
    end else if (adc_clk_count[32] == 1'b0) begin
      adc_clk_count <= adc_clk_count + 1'b1;
    end else begin
      adc_clk_count <= {33{1'b1}};
    end
  end

  // DELAY CONTROL

  FDPE #(.INIT(1'b1)) i_delay_rst_reg (
    .CE (1'b1),
    .D (1'b0),
    .PRE (up_preset_s),
    .C (delay_clk),
    .Q (delay_rst));

  // delay control transfer

  always @(posedge delay_clk) begin
    if (delay_rst == 1'b1) begin
      delay_sel_m1 <= 'd0;
      delay_sel_m2 <= 'd0;
      delay_sel_m3 <= 'd0;
    end else begin
      delay_sel_m1 <= up_delay_sel;
      delay_sel_m2 <= delay_sel_m1;
      delay_sel_m3 <= delay_sel_m2;
    end
    delay_sel <= delay_sel_m2 & ~delay_sel_m3;
    if ((delay_sel_m2 == 1'b1) && (delay_sel_m3 == 1'b0)) begin
      delay_rwn <= up_delay_rwn;
      delay_addr <= up_delay_addr;
      delay_wdata <= up_delay_wdata;
    end
  end

  // delay status transfer

  assign up_delay_ack_t_s = up_delay_ack_t_m3 ^ up_delay_ack_t_m2;
  assign up_delay_sel_s = up_delay_sel & ~up_delay_sel_d;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_delay_locked_m1 <= 'd0;
      up_delay_locked <= 'd0;
      up_delay_ack_t_m1 <= 'd0;
      up_delay_ack_t_m2 <= 'd0;
      up_delay_ack_t_m3 <= 'd0;
      up_delay_sel_d <= 'd0;
      up_delay_status <= 'd0;
      up_delay_rdata <= 'd0;
    end else begin
      up_delay_locked_m1 <= delay_locked;
      up_delay_locked <= up_delay_locked_m1;
      up_delay_ack_t_m1 <= delay_ack_t;
      up_delay_ack_t_m2 <= up_delay_ack_t_m1;
      up_delay_ack_t_m3 <= up_delay_ack_t_m2;
      up_delay_sel_d <= up_delay_sel;
      if (up_delay_ack_t_s == 1'b1) begin
        up_delay_status <= 1'b0;
      end else if (up_delay_sel_s == 1'b1) begin
        up_delay_status <= 1'b1;
      end
      if (up_delay_ack_t_s == 1'b1) begin
        up_delay_rdata <= delay_rdata;
      end
    end
  end

  // DRP CONTROL

  FDPE #(.INIT(1'b1)) i_drp_rst_reg (
    .CE (1'b1),
    .D (1'b0),
    .PRE (up_preset_s),
    .C (drp_clk),
    .Q (drp_rst));

  // drp control transfer

  always @(posedge drp_clk) begin
    if (drp_rst == 1'b1) begin
      drp_sel_m1 <= 'd0;
      drp_sel_m2 <= 'd0;
      drp_sel_m3 <= 'd0;
    end else begin
      drp_sel_m1 <= up_drp_sel;
      drp_sel_m2 <= drp_sel_m1;
      drp_sel_m3 <= drp_sel_m2;
    end
    drp_sel <= drp_sel_m2 & ~drp_sel_m3;
    if ((drp_sel_m2 == 1'b1) && (drp_sel_m3 == 1'b0)) begin
      drp_rwn <= up_drp_rwn;
      drp_addr <= up_drp_addr;
      drp_wdata <= up_drp_wdata;
    end
  end

  // drp status transfer

  assign up_drp_ack_t_s = up_drp_ack_t_m3 ^ up_drp_ack_t_m2;
  assign up_drp_sel_s = up_drp_sel & ~up_drp_sel_d;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_drp_ack_t_m1 <= 'd0;
      up_drp_ack_t_m2 <= 'd0;
      up_drp_ack_t_m3 <= 'd0;
      up_drp_sel_d <= 'd0;
      up_drp_status <= 'd0;
      up_drp_rdata <= 'd0;
    end else begin
      up_drp_ack_t_m1 <= drp_ack_t;
      up_drp_ack_t_m2 <= up_drp_ack_t_m1;
      up_drp_ack_t_m3 <= up_drp_ack_t_m2;
      up_drp_sel_d <= up_drp_sel;
      if (up_drp_ack_t_s == 1'b1) begin
        up_drp_status <= 1'b0;
      end else if (up_drp_sel_s == 1'b1) begin
        up_drp_status <= 1'b1;
      end
      if (up_drp_ack_t_s == 1'b1) begin
        up_drp_rdata <= drp_rdata;
      end
    end
  end

  // DMA CONTROL

  FDPE #(.INIT(1'b1)) i_dma_rst_reg (
    .CE (1'b1),
    .D (1'b0),
    .PRE (up_preset_s),
    .C (dma_clk),
    .Q (dma_rst));

  // dma control transfer

  always @(posedge dma_clk) begin
    if (dma_rst == 1'b1) begin
      dma_start_m1 <= 'd0;
      dma_start_m2 <= 'd0;
      dma_start_m3 <= 'd0;
    end else begin
      dma_start_m1 <= up_dma_start;
      dma_start_m2 <= dma_start_m1;
      dma_start_m3 <= dma_start_m2;
    end
    dma_start <= dma_start_m2 & ~dma_start_m3;
    if ((dma_start_m2 == 1'b1) && (dma_start_m3 == 1'b0)) begin
      dma_stream <= up_dma_stream;
      dma_count <= up_dma_count;
    end
  end

  // dma status transfer

  always @(posedge dma_clk) begin
    dma_xfer_cnt <= dma_xfer_cnt + 1'b1;
    if (dma_xfer_cnt == 6'd0) begin
      dma_xfer_toggle <= ~dma_xfer_toggle;
      dma_xfer_ovf <= dma_acc_ovf;
      dma_xfer_unf <= dma_acc_unf;
    end
    if (dma_xfer_cnt == 6'd0) begin
      dma_acc_ovf <= dma_ovf;
      dma_acc_unf <= dma_unf;
    end else begin
      dma_acc_ovf <= dma_acc_ovf | dma_ovf;
      dma_acc_unf <= dma_acc_unf | dma_unf;
    end
  end

  assign up_dma_xfer_toggle_s = up_dma_xfer_toggle_m2 ^ up_dma_xfer_toggle_m3;

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_dma_xfer_toggle_m1 <= 'd0;
      up_dma_xfer_toggle_m2 <= 'd0;
      up_dma_xfer_toggle_m3 <= 'd0;
      up_dma_xfer_ovf <= 'd0;
      up_dma_xfer_unf <= 'd0;
      up_dma_ovf <= 'd0;
      up_dma_unf <= 'd0;
    end else begin
      up_dma_xfer_toggle_m1 <= dma_xfer_toggle;
      up_dma_xfer_toggle_m2 <= up_dma_xfer_toggle_m1;
      up_dma_xfer_toggle_m3 <= up_dma_xfer_toggle_m2;
      if (up_dma_xfer_toggle_s == 1'b1) begin
        up_dma_xfer_ovf <= dma_xfer_ovf;
        up_dma_xfer_unf <= dma_xfer_unf;
      end
      if (up_dma_xfer_ovf == 1'b1) begin
        up_dma_ovf <= 1'b1;
      end else if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h22)) begin
        up_dma_ovf <= up_dma_ovf & ~up_wdata[2];
      end
      if (up_dma_xfer_unf == 1'b1) begin
        up_dma_unf <= 1'b1;
      end else if ((up_wr_s == 1'b1) && (up_addr[7:0] == 8'h22)) begin
        up_dma_unf <= up_dma_unf & ~up_wdata[1];
      end
    end
  end

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_dma_status_m1 <= 'd0;
      up_dma_status <= 'd0;
    end else begin
      up_dma_status_m1 <= dma_status;
      up_dma_status <= up_dma_status_m1;
    end
  end

endmodule

// ***************************************************************************
// ***************************************************************************
