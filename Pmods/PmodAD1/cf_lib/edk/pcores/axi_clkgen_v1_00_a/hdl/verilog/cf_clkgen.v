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
// Software programmable clock generator (still needs a reference input!)

module cf_clkgen (

  // reference clock input (default 200MHz)

  ref_clk,

  // output clock M/N*f_in for any M/N supported by MMCM.

  clk,

  // processor interface

  up_rstn,
  up_clk,
  up_sel,
  up_rwn,
  up_addr,
  up_wdata,
  up_rdata,
  up_ack);

  // MMCM state machine parameters

  parameter MMCM_TYPE   = 'd0;
  parameter MMCM_IDLE   = 'h0;
  parameter MMCM_READ   = 'h1;
  parameter MMCM_RDRDY  = 'h2;
  parameter MMCM_WRITE  = 'h3;
  parameter MMCM_WRRDY  = 'h4;

  // reference clock input (default 200MHz)

  input           ref_clk;

  // output clock M/N*f_in for any M/N supported by MMCM.

  output          clk;

  // processor interface

  input           up_rstn;
  input           up_clk;
  input           up_sel;
  input           up_rwn;
  input   [ 4:0]  up_addr;
  input   [31:0]  up_wdata;
  output  [31:0]  up_rdata;
  output          up_ack;

  reg             mmcm_swrst;
  reg             mmcm_start;
  reg             mmcm_start_d;
  reg     [15:0]  mmcm_clk_out_1;
  reg     [15:0]  mmcm_clk_out_2;
  reg     [15:0]  mmcm_clk_div;
  reg     [15:0]  mmcm_clk_fb_1;
  reg     [15:0]  mmcm_clk_fb_2;
  reg     [15:0]  mmcm_lock_1;
  reg     [15:0]  mmcm_lock_2;
  reg     [15:0]  mmcm_lock_3;
  reg     [15:0]  mmcm_filter_1;
  reg     [15:0]  mmcm_filter_2;
  reg     [31:0]  up_rdata;
  reg             up_sel_d;
  reg             up_sel_2d;
  reg             up_ack;
  reg             mmcm_rst;
  reg     [ 6:0]  mmcm_addr;
  reg             mmcm_sel;
  reg     [15:0]  mmcm_rdata;
  reg     [15:0]  mmcm_wdata;
  reg             mmcm_wr;
  reg     [ 3:0]  mmcm_count;
  reg     [ 2:0]  mmcm_state;

  wire            up_wr_s;
  wire            up_ack_s;
  wire            mmcm_start_s;
  wire    [15:0]  mmcm_wdata_s;
  reg     [38:0]  mmcm_data_s;
  wire    [15:0]  mmcm_rdata_s;
  wire            mmcm_ready_s;
  wire            mmcm_locked_s;
  wire            mmcm_clk_s;
  wire            mmcm_fb_clk_s;
  wire            buf_fb_clk_s;

  // Processor write interface, most of these registers are defined by Xilinx for MMCM.
  // See regmap.txt for details, the processor write is directly transferred to DRP.

  assign up_wr_s = up_sel & ~up_rwn;
  assign up_ack_s = up_sel_d & ~up_sel_2d;
  assign mmcm_start_s = mmcm_start & ~mmcm_start_d;
  assign mmcm_wdata_s = ~mmcm_data_s[31:16] & mmcm_data_s[15:0];

  always @(mmcm_count or mmcm_clk_out_1 or mmcm_clk_out_2 or mmcm_clk_div or
    mmcm_clk_fb_1 or mmcm_clk_fb_2 or mmcm_lock_1 or mmcm_lock_2 or mmcm_lock_3 or
    mmcm_filter_1 or mmcm_filter_2) begin
    case (mmcm_count)
      4'b0101: mmcm_data_s = {7'h28, 16'h0000, 16'hffff};
      4'b0110: mmcm_data_s = {7'h08, 16'h1000, mmcm_clk_out_1};
      4'b0111: mmcm_data_s = {7'h09, 16'hfc00, mmcm_clk_out_2};
      4'b1000: mmcm_data_s = {7'h16, 16'hc000, mmcm_clk_div};
      4'b1001: mmcm_data_s = {7'h14, 16'h1000, mmcm_clk_fb_1};
      4'b1010: mmcm_data_s = {7'h15, 16'hfc00, mmcm_clk_fb_2};
      4'b1011: mmcm_data_s = {7'h18, 16'hfc00, mmcm_lock_1};
      4'b1100: mmcm_data_s = {7'h19, 16'h8000, mmcm_lock_2};
      4'b1101: mmcm_data_s = {7'h1a, 16'h8000, mmcm_lock_3};
      4'b1110: mmcm_data_s = {7'h4e, 16'h66ff, mmcm_filter_1};
      4'b1111: mmcm_data_s = {7'h4f, 16'h666f, mmcm_filter_2};
      default: mmcm_data_s = 'd0;
    endcase
  end

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      mmcm_swrst <= 'd0;
      mmcm_start <= 'd0;
      mmcm_start_d <= 'd0;
      mmcm_clk_out_1 <= 'd0;
      mmcm_clk_out_2 <= 'd0;
      mmcm_clk_div <= 'd0;
      mmcm_clk_fb_1 <= 'd0;
      mmcm_clk_fb_2 <= 'd0;
      mmcm_lock_1 <= 'd0;
      mmcm_lock_2 <= 'd0;
      mmcm_lock_3 <= 'd0;
      mmcm_filter_1 <= 'd0;
      mmcm_filter_2 <= 'd0;
    end else begin
      if ((up_addr == 5'h01) && (up_wr_s == 1'b1)) begin
        mmcm_swrst <= up_wdata[1];
        mmcm_start <= up_wdata[0];
      end
      mmcm_start_d <= mmcm_start;
      if ((up_addr == 5'h02) && (up_wr_s == 1'b1)) begin
        mmcm_clk_out_1 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h03) && (up_wr_s == 1'b1)) begin
        mmcm_clk_out_2 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h04) && (up_wr_s == 1'b1)) begin
        mmcm_clk_div <= up_wdata[15:0];
      end
      if ((up_addr == 5'h05) && (up_wr_s == 1'b1)) begin
        mmcm_clk_fb_1 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h06) && (up_wr_s == 1'b1)) begin
        mmcm_clk_fb_2 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h07) && (up_wr_s == 1'b1)) begin
        mmcm_lock_1 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h08) && (up_wr_s == 1'b1)) begin
        mmcm_lock_2 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h09) && (up_wr_s == 1'b1)) begin
        mmcm_lock_3 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h0a) && (up_wr_s == 1'b1)) begin
        mmcm_filter_1 <= up_wdata[15:0];
      end
      if ((up_addr == 5'h0b) && (up_wr_s == 1'b1)) begin
        mmcm_filter_2 <= up_wdata[15:0];
      end
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
        5'h01: up_rdata <= {30'd0, mmcm_swrst, mmcm_start};
        5'h02: up_rdata <= {16'd0, mmcm_clk_out_1};
        5'h03: up_rdata <= {16'd0, mmcm_clk_out_2};
        5'h04: up_rdata <= {16'd0, mmcm_clk_div};
        5'h05: up_rdata <= {16'd0, mmcm_clk_fb_1};
        5'h06: up_rdata <= {16'd0, mmcm_clk_fb_2};
        5'h07: up_rdata <= {16'd0, mmcm_lock_1};
        5'h08: up_rdata <= {16'd0, mmcm_lock_2};
        5'h09: up_rdata <= {16'd0, mmcm_lock_3};
        5'h0a: up_rdata <= {16'd0, mmcm_filter_1};
        5'h0b: up_rdata <= {16'd0, mmcm_filter_2};
        5'h1f: up_rdata <= {30'd0, mmcm_rst, mmcm_locked_s};
        default: up_rdata <= 0;
      endcase
      up_sel_d <= up_sel;
      up_sel_2d <= up_sel_d;
      up_ack <= up_ack_s;
    end
  end

  // DRP state machine and control signals, write/read and then wait for ready
  // The DRP is written always in order with a continous read-modify-writes.

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      mmcm_rst <= 'd1;
      mmcm_addr <= 'd0;
      mmcm_sel <= 'd0;
      mmcm_rdata <= 'd0;
      mmcm_wdata <= 'd0;
      mmcm_wr <= 'd0;
      mmcm_count <= 'd0;
      mmcm_state <= 'd0;
    end else begin
      if (mmcm_state == MMCM_IDLE) begin
        mmcm_rst <= mmcm_swrst;
      end else begin
        mmcm_rst <= 1'b1;
      end
      if (mmcm_state == MMCM_READ) begin
        mmcm_addr <= mmcm_data_s[38:32];
      end else if (mmcm_state == MMCM_IDLE) begin
        mmcm_addr <= 'd0;
      end
      if ((mmcm_state == MMCM_READ) || (mmcm_state == MMCM_WRITE)) begin
        mmcm_sel <= 1'b1;
      end else begin
        mmcm_sel <= 1'b0;
      end
      if ((mmcm_state == MMCM_RDRDY) && (mmcm_ready_s == 1'b1)) begin
        mmcm_rdata <= mmcm_rdata_s;
      end
      if (mmcm_state == MMCM_WRITE) begin
        mmcm_wdata <= (mmcm_data_s[31:16] & mmcm_rdata) | mmcm_wdata_s;
      end else begin
        mmcm_wdata <= 'd0;
      end
      if (mmcm_state == MMCM_WRITE) begin
        mmcm_wr <= 1'b1;
      end else begin
        mmcm_wr <= 1'b0;
      end
      if (mmcm_state == MMCM_IDLE) begin
        mmcm_count <= 4'h5;
      end else if (mmcm_state == MMCM_WRITE) begin
        mmcm_count <= mmcm_count + 1'b1;
      end
      case (mmcm_state)
        MMCM_IDLE: begin
          if ((mmcm_locked_s == 1'b1) && (mmcm_start_s == 1'b1)) begin
            mmcm_state <= MMCM_READ;  // if read back data first
          end else begin
            mmcm_state <= MMCM_IDLE;
          end
        end
        MMCM_READ: begin
          mmcm_state <= MMCM_RDRDY;
        end
        MMCM_RDRDY: begin
          if (mmcm_ready_s == 1'b1) begin
            mmcm_state <= MMCM_WRITE; // modify data and write back
          end else begin
            mmcm_state <= MMCM_RDRDY;
          end
        end
        MMCM_WRITE: begin
          mmcm_state <= MMCM_WRRDY;
        end
        MMCM_WRRDY: begin
          if (mmcm_ready_s == 1'b0) begin
            mmcm_state <= MMCM_WRRDY;
          end else if (mmcm_count == 0) begin
            mmcm_state <= MMCM_IDLE;
          end else begin
            mmcm_state <= MMCM_READ;
          end
        end
        default: begin
          mmcm_state <= MMCM_IDLE;
        end
      endcase
    end
  end

  // MMCM instantiations

  generate
  if (MMCM_TYPE == 1) begin
  MMCM_ADV #(
    .BANDWIDTH ("OPTIMIZED"),
    .CLKOUT4_CASCADE ("FALSE"),
    .CLOCK_HOLD ("FALSE"),
    .COMPENSATION ("ZHOLD"),
    .STARTUP_WAIT ("FALSE"),
    .DIVCLK_DIVIDE (11),
    .CLKFBOUT_MULT_F (49.000),
    .CLKFBOUT_PHASE (0.000),
    .CLKFBOUT_USE_FINE_PS ("FALSE"),
    .CLKOUT0_DIVIDE_F (6.000),
    .CLKOUT0_PHASE (0.000),
    .CLKOUT0_DUTY_CYCLE (0.500),
    .CLKOUT0_USE_FINE_PS ("FALSE"),
    .CLKIN1_PERIOD (5.000),
    .REF_JITTER1 (0.010))
  i_mmcm (
    .CLKFBOUT (mmcm_fb_clk_s),
    .CLKFBIN (buf_fb_clk_s),
    .CLKFBOUTB (),
    .CLKOUT0 (mmcm_clk_s),
    .CLKOUT0B (),
    .CLKOUT1 (),
    .CLKOUT1B (),
    .CLKOUT2 (),
    .CLKOUT2B (),
    .CLKOUT3 (),
    .CLKOUT3B (),
    .CLKOUT4 (),
    .CLKOUT5 (),
    .CLKOUT6 (),
    .CLKIN1 (ref_clk),
    .CLKIN2 (1'b0),
    .CLKINSEL (1'b1),
    .DCLK (up_clk),
    .DADDR (mmcm_addr),
    .DEN (mmcm_sel),
    .DI (mmcm_wdata),
    .DO (mmcm_rdata_s),
    .DRDY (mmcm_ready_s),
    .DWE (mmcm_wr),
    .PSCLK (1'b0),
    .PSEN (1'b0),
    .PSINCDEC (1'b0),
    .PSDONE (),
    .LOCKED (mmcm_locked_s),
    .CLKINSTOPPED (),
    .CLKFBSTOPPED (),
    .PWRDWN (1'b0),
    .RST (mmcm_rst));
  end else begin
  MMCME2_ADV #(
    .BANDWIDTH ("OPTIMIZED"),
    .CLKOUT4_CASCADE ("FALSE"),
    .COMPENSATION ("ZHOLD"),
    .STARTUP_WAIT ("FALSE"),
    .DIVCLK_DIVIDE (11),
    .CLKFBOUT_MULT_F (49.000),
    .CLKFBOUT_PHASE (0.000),
    .CLKFBOUT_USE_FINE_PS ("FALSE"),
    .CLKOUT0_DIVIDE_F (6.000),
    .CLKOUT0_PHASE (0.000),
    .CLKOUT0_DUTY_CYCLE (0.500),
    .CLKOUT0_USE_FINE_PS ("FALSE"),
    .CLKIN1_PERIOD (5.000),
    .REF_JITTER1 (0.010))
  i_mmcm (
    .CLKFBOUT (mmcm_fb_clk_s),
    .CLKFBIN (buf_fb_clk_s),
    .CLKFBOUTB (),
    .CLKOUT0 (mmcm_clk_s),
    .CLKOUT0B (),
    .CLKOUT1 (),
    .CLKOUT1B (),
    .CLKOUT2 (),
    .CLKOUT2B (),
    .CLKOUT3 (),
    .CLKOUT3B (),
    .CLKOUT4 (),
    .CLKOUT5 (),
    .CLKOUT6 (),
    .CLKIN1 (ref_clk),
    .CLKIN2 (1'b0),
    .CLKINSEL (1'b1),
    .DCLK (up_clk),
    .DADDR (mmcm_addr),
    .DEN (mmcm_sel),
    .DI (mmcm_wdata),
    .DO (mmcm_rdata_s),
    .DRDY (mmcm_ready_s),
    .DWE (mmcm_wr),
    .PSCLK (1'b0),
    .PSEN (1'b0),
    .PSINCDEC (1'b0),
    .PSDONE (),
    .LOCKED (mmcm_locked_s),
    .CLKINSTOPPED (),
    .CLKFBSTOPPED (),
    .PWRDWN (1'b0),
    .RST (mmcm_rst));
  end
  endgenerate

  BUFG i_fb_buf (
    .I (mmcm_fb_clk_s),
    .O (buf_fb_clk_s));

  BUFG i_buf (
    .I (mmcm_clk_s),
    .O (clk));

endmodule

// ***************************************************************************
// ***************************************************************************
