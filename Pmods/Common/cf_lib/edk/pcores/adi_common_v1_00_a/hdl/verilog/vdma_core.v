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
// dac vdma read

module vdma_core (

  // vdma interface

  vdma_clk,
  vdma_rst,
  vdma_fs,
  vdma_valid,
  vdma_data,
  vdma_ready,
  vdma_ovf,
  vdma_unf,

  // dac interface 

  dac_clk,
  dac_rst,
  dac_rd,
  dac_valid,
  dac_data,

  // processor interface

  vdma_frmcnt);

  // parameters

  parameter       DATA_WIDTH = 64;
  localparam      DW = DATA_WIDTH - 1;
  localparam      BUF_THRESHOLD_LO = 6'd3;
  localparam      BUF_THRESHOLD_HI = 6'd60;
  localparam      RDY_THRESHOLD_LO = 6'd40;
  localparam      RDY_THRESHOLD_HI = 6'd50;

  // vdma interface

  input           vdma_clk;
  input           vdma_rst;
  output          vdma_fs;
  input           vdma_valid;
  input   [DW:0]  vdma_data;
  output          vdma_ready;
  output          vdma_ovf;
  output          vdma_unf;

  // dac interface

  input           dac_clk;
  input           dac_rst;
  input           dac_rd;
  output          dac_valid;
  output  [DW:0]  dac_data;

  // processor interface

  input   [31:0]  vdma_frmcnt;

  // internal registers

  reg             dac_start_m1 = 'd0;
  reg             dac_start = 'd0;
  reg             dac_resync_m1 = 'd0;
  reg             dac_resync = 'd0;
  reg     [ 5:0]  dac_raddr = 'd0;
  reg     [ 5:0]  dac_raddr_g = 'd0;
  reg             dac_rd_d = 'd0;
  reg             dac_rd_2d = 'd0;
  reg             dac_valid = 'd0;
  reg     [DW:0]  dac_data = 'd0;
  reg     [31:0]  vdma_clkcnt = 'd0;
  reg             vdma_fs = 'd0;
  reg     [ 5:0]  vdma_raddr_g_m1 = 'd0;
  reg     [ 5:0]  vdma_raddr_g_m2 = 'd0;
  reg     [ 5:0]  vdma_raddr = 'd0;
  reg     [ 5:0]  vdma_addr_diff = 'd0;
  reg             vdma_ready = 'd0;
  reg             vdma_almost_full = 'd0;
  reg             vdma_almost_empty = 'd0;
  reg             vdma_ovf = 'd0;
  reg             vdma_unf = 'd0;
  reg             vdma_resync = 'd0;
  reg             vdma_start = 'd0;
  reg             vdma_wr = 'd0;
  reg     [ 5:0]  vdma_waddr = 'd0;
  reg     [DW:0]  vdma_wdata = 'd0;

  // internal signals

  wire            vdma_wr_s;
  wire    [ 6:0]  vdma_addr_diff_s;
  wire            vdma_ovf_s;
  wire            vdma_unf_s;
  wire    [DW:0]  dac_rdata_s;

  // binary to grey coversion

  function [7:0] b2g;
    input [7:0] b;
    reg   [7:0] g;
    begin
      g[7] = b[7];
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

  // grey to binary conversion

  function [7:0] g2b;
    input [7:0] g;
    reg   [7:0] b;
    begin
      b[7] = g[7];
      b[6] = b[7] ^ g[6];
      b[5] = b[6] ^ g[5];
      b[4] = b[5] ^ g[4];
      b[3] = b[4] ^ g[3];
      b[2] = b[3] ^ g[2];
      b[1] = b[2] ^ g[1];
      b[0] = b[1] ^ g[0];
      g2b = b;
    end
  endfunction

  // dac read interface

  always @(posedge dac_clk) begin
    if (dac_rst == 1'b1) begin
      dac_start_m1 <= 'd0;
      dac_start <= 'd0;
      dac_resync_m1 <= 'd0;
      dac_resync <= 'd0;
    end else begin
      dac_start_m1 <= vdma_start;
      dac_start <= dac_start_m1;
      dac_resync_m1 <= vdma_resync;
      dac_resync <= dac_resync_m1;
    end
    if ((dac_start == 1'b0) || (dac_resync == 1'b1) || (dac_rst == 1'b1)) begin
      dac_raddr <= 6'd0;
    end else if (dac_rd == 1'b1) begin
      dac_raddr <= dac_raddr + 1'b1;
    end
    dac_raddr_g <= b2g(dac_raddr);
    dac_rd_d <= dac_rd;
    dac_rd_2d <= dac_rd_d;
    dac_valid <= dac_rd_2d;
    dac_data <= dac_rdata_s;
  end
    
  // generate fsync

  always @(posedge vdma_clk) begin
    if ((vdma_resync == 1'b1) || (vdma_rst == 1'b1) || (vdma_clkcnt >= vdma_frmcnt)) begin
      vdma_clkcnt <= 16'd0;
    end else begin
      vdma_clkcnt <= vdma_clkcnt + 1'b1;
    end
    if (vdma_clkcnt == 32'd1) begin
      vdma_fs <= 1'b1;
    end else begin
      vdma_fs <= 1'b0;
    end
  end

  // overflow or underflow status

  assign vdma_addr_diff_s = {1'b1, vdma_waddr} - vdma_raddr;
  assign vdma_ovf_s = (vdma_addr_diff < BUF_THRESHOLD_LO) ? vdma_almost_full : 1'b0;
  assign vdma_unf_s = (vdma_addr_diff > BUF_THRESHOLD_HI) ? vdma_almost_empty : 1'b0;

  always @(posedge vdma_clk) begin
    if (vdma_rst == 1'b1) begin
      vdma_raddr_g_m1 <= 'd0;
      vdma_raddr_g_m2 <= 'd0;
    end else begin
      vdma_raddr_g_m1 <= dac_raddr_g;
      vdma_raddr_g_m2 <= vdma_raddr_g_m1;
    end
    vdma_raddr <= g2b(vdma_raddr_g_m2);
    vdma_addr_diff <= vdma_addr_diff_s[5:0];
    if (vdma_addr_diff >= RDY_THRESHOLD_HI) begin
      vdma_ready <= 1'b0;
    end else if (vdma_addr_diff <= RDY_THRESHOLD_LO) begin
      vdma_ready <= 1'b1;
    end
    if (vdma_addr_diff > BUF_THRESHOLD_HI) begin
      vdma_almost_full <= 1'b1;
    end else begin
      vdma_almost_full <= 1'b0;
    end
    if (vdma_addr_diff < BUF_THRESHOLD_LO) begin
      vdma_almost_empty <= 1'b1;
    end else begin
      vdma_almost_empty <= 1'b0;
    end
    vdma_ovf <= vdma_ovf_s;
    vdma_unf <= vdma_unf_s;
    vdma_resync <= vdma_ovf | vdma_unf;
  end

  // vdma write

  assign vdma_wr_s = vdma_valid & vdma_ready;

  always @(posedge vdma_clk) begin
    if (vdma_rst == 1'b1) begin
      vdma_start <= 1'b0;
    end else if (vdma_wr_s == 1'b1) begin
      vdma_start <= 1'b1;
    end
    vdma_wr <= vdma_wr_s;
    if ((vdma_resync == 1'b1) || (vdma_rst == 1'b1)) begin
      vdma_waddr <= 6'd0;
    end else if (vdma_wr == 1'b1) begin
      vdma_waddr <= vdma_waddr + 1'b1;
    end
    vdma_wdata <= vdma_data;
  end

  // memory

  mem #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(6)) i_mem (
    .clka (vdma_clk),
    .wea (vdma_wr),
    .addra (vdma_waddr),
    .dina (vdma_wdata),
    .clkb (dac_clk),
    .addrb (dac_raddr),
    .doutb (dac_rdata_s));

endmodule

// ***************************************************************************
// ***************************************************************************
