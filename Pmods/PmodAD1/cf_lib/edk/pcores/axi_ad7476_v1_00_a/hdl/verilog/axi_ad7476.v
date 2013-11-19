// ***************************************************************************
// ***************************************************************************
// Copyright 2013(c) Analog Devices, Inc.
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

module axi_ad7476 (

  // physical interface

  // ADD USER PORTS HERE
  data_0_i,
  data_1_i,
  sclk_o,
  cs_o,

  ref_clk,
  rx_clk,

  // master-slave interface

  adc_start_out,
  dma_start_out,
  adc_start_in,
  dma_start_in,


  // dma interface

  s_axis_s2mm_clk,
  s_axis_s2mm_tvalid,
  s_axis_s2mm_tdata,
  s_axis_s2mm_tkeep,
  s_axis_s2mm_tlast,
  s_axis_s2mm_tready,

  // axi interface

  s_axi_aclk,
  s_axi_aresetn,
  s_axi_awvalid,
  s_axi_awaddr,
  s_axi_awready,
  s_axi_wvalid,
  s_axi_wdata,
  s_axi_wstrb,
  s_axi_wready,
  s_axi_bvalid,
  s_axi_bresp,
  s_axi_bready,
  s_axi_arvalid,
  s_axi_araddr,
  s_axi_arready,
  s_axi_rvalid,
  s_axi_rresp,
  s_axi_rdata,
  s_axi_rready,

  // debug signals

  adc_clk,
  adc_mon_valid,
  adc_mon_data);

  parameter PCORE_ID = 0;
  parameter PCORE_DEVICE_TYPE = 0;
  parameter PCORE_IODELAY_GROUP = "adc_if_delay_group";
  parameter C_S_AXI_MIN_SIZE = 32'hffff;
  parameter C_BASEADDR = 32'hffffffff;
  parameter C_HIGHADDR = 32'h00000000;

  // ADD USER PORTS HERE
  input           ref_clk;  // system clock
  input           rx_clk;   // clock used for ADC data acquisition, designed for 50 MHz
  //ADC control and data interface
  input           data_0_i;
  input           data_1_i;
  output          sclk_o;
  output          cs_o;

  // master-slave interface

  output          adc_start_out;
  output          dma_start_out;
  input           adc_start_in;
  input           dma_start_in;


  // dma interface

  input           s_axis_s2mm_clk;
  output          s_axis_s2mm_tvalid;
  output  [31:0]  s_axis_s2mm_tdata;
  output  [ 3:0]  s_axis_s2mm_tkeep;
  output          s_axis_s2mm_tlast;
  input           s_axis_s2mm_tready;

  // axi interface

  input           s_axi_aclk;
  input           s_axi_aresetn;
  input           s_axi_awvalid;
  input   [31:0]  s_axi_awaddr;
  output          s_axi_awready;
  input           s_axi_wvalid;
  input   [31:0]  s_axi_wdata;
  input   [ 3:0]  s_axi_wstrb;
  output          s_axi_wready;
  output          s_axi_bvalid;
  output  [ 1:0]  s_axi_bresp;
  input           s_axi_bready;
  input           s_axi_arvalid;
  input   [31:0]  s_axi_araddr;
  output          s_axi_arready;
  output          s_axi_rvalid;
  output  [ 1:0]  s_axi_rresp;
  output  [31:0]  s_axi_rdata;
  input           s_axi_rready;

  // debug signals

  output          adc_clk;
  output          adc_mon_valid;
  output  [31:0]  adc_mon_data;

  // internal registers

  reg             adc_start_out = 'd0;
  reg             adc_valid = 'd0;
  reg     [31:0]  adc_data = 'd0;
  reg     [31:0]  up_rdata = 'd0;
  reg             up_ack = 'd0;

  // internal clocks & resets

  wire            adc_rst;
  wire            dma_clk;
  wire            dma_rst;
  wire            up_rstn;
  wire            up_clk;

  // internal signals

  wire            adc_start_s;
  wire            dma_start_s;
  wire    [15:0]  adc_data0_s; // ADC DATA 
  wire    [15:0]  adc_data1_s; // ADC DATA 
  wire    [15:0]  adc_dcfilter_data_a_s;
  wire    [15:0]  adc_dcfilter_data_b_s;
  wire            adc_enable_s;
  wire            adc_status_s;
  wire            dma_valid_s;
  wire            dma_last_s;
  wire    [31:0]  dma_data_s;
  wire            dma_ready_s;
  wire            dma_stream_s;
  wire    [31:0]  dma_count_s;
  wire            dma_ovf_s;
  wire            dma_unf_s;
  wire            dma_status_s;
  wire    [31:0]  dma_bw_s;
  wire            up_sel_s;
  wire            up_wr_s;
  wire    [13:0]  up_addr_s;
  wire    [31:0]  up_wdata_s;
  wire    [31:0]  up_adc_common_rdata_s;
  wire            up_adc_common_ack_s;
  wire    [31:0]  pid_s;
  wire            adc_data_valid;
  wire            data_rd_ready_s; // ADC DATA READY
  // signal name changes

  assign up_clk = s_axi_aclk;
  assign up_rstn = s_axi_aresetn;
  assign dma_clk = s_axis_s2mm_clk;
  assign dma_ready_s = s_axis_s2mm_tready;
  assign s_axis_s2mm_tvalid = dma_valid_s;
  assign s_axis_s2mm_tdata  = dma_data_s;
  assign s_axis_s2mm_tlast = dma_last_s;
  assign s_axis_s2mm_tkeep = 4'hf;

  // monitor signals

  assign adc_mon_valid = data_rd_ready_s;
  assign adc_mon_data[15: 0] = adc_data0_s;
  assign adc_mon_data[31:16] = adc_data1_s;

  // multiple instances synchronization
  assign pid_s = 32'd0;
  assign adc_start_s = (pid_s == 32'd0) ? adc_start_out : adc_start_in;
  assign dma_start_s = (pid_s == 32'd0) ? dma_start_out : dma_start_in;

  assign adc_clk = s_axi_aclk;

  always @(posedge adc_clk) begin
    if (adc_rst == 1'b1) begin
      adc_start_out <= 1'b0;
    end else begin
      adc_start_out <= 1'b1;
    end
  end

  // adc channels - dma interface

  always @(posedge adc_clk)
  begin
      adc_valid <= data_rd_ready_s;
      adc_data  <= { adc_data0_s, adc_data1_s};
  end

  // processor read interface

  always @(negedge up_rstn or posedge up_clk) begin
    if (up_rstn == 0) begin
      up_rdata  <= 'd0;
      up_ack    <= 'd0;
    end else begin
      up_rdata  <= up_adc_common_rdata_s;
      up_ack    <= up_adc_common_ack_s;
    end
  end

  // main (device interface)
  // USER DEVICE INTERFACE INSTANTIATION

  axi_ad7476_dev_if i_if (
    //clock and reset signals
    .fpga_clk_i(ref_clk),
    .adc_clk_i(rx_clk),
    .reset_n_i(~adc_rst),
        
    //IP control and data interface
    .en_0_i(1'b1),
    .en_1_i(1'b1),        
    .data_rdy_o(data_rd_ready_s),
    .data_0_o(adc_data0_s),
    .data_1_o(adc_data1_s),
        
    //ADC control and data interface
    .data_0_i(data_0_i),
    .data_1_i(data_1_i),
    .sclk_o(sclk_o),
    .cs_o(cs_o)    
    );
    
  // dma transfer 
  dma_core #(.DATA_WIDTH(32)) i_dma_core (
    .dma_clk (dma_clk),
    .dma_rst (dma_rst),
    .dma_valid (dma_valid_s),
    .dma_last (dma_last_s),
    .dma_data (dma_data_s),
    .dma_ready (dma_ready_s),
    .dma_ovf (dma_ovf_s),
    .dma_unf (dma_unf_s),
    .dma_status (dma_status_s),
    .dma_bw (dma_bw_s),
    .adc_clk (adc_clk),
    .adc_rst (adc_rst),
    .adc_valid (adc_valid),
    .adc_data (adc_data),
    .dma_start (dma_start_s),
    .dma_stream (dma_stream_s),
    .dma_count (dma_count_s));

  // common processor control

  up_adc_common i_up_adc_common (
    .adc_clk (adc_clk),
    .adc_rst (adc_rst),
    .adc_r1_mode (),
    .adc_ddr_edgesel (),
    .adc_pin_mode (),
    .adc_status (data_rd_ready_s),
    .adc_clk_ratio (32'd1),
    .delay_clk (1'b0),
    .delay_rst (),
    .delay_sel (),
    .delay_rwn (),
    .delay_addr (),
    .delay_wdata (),
    .delay_rdata (5'd0),
    .delay_ack_t (1'b0),
    .delay_locked (1'b0),
    .drp_clk (1'd0),
    .drp_rst (),
    .drp_sel (),
    .drp_wr (),
    .drp_addr (),
    .drp_wdata (),
    .drp_rdata (16'd0),
    .drp_ack_t (1'd0),
    .dma_clk (dma_clk),
    .dma_rst (dma_rst),
    .dma_start (dma_start_out),
    .dma_stream (dma_stream_s),
    .dma_count (dma_count_s),
    .dma_ovf (dma_ovf_s),
    .dma_unf (dma_unf_s),
    .dma_status (dma_status_s),
    .dma_bw (dma_bw_s),
    .up_usr_chanmax (),
    .adc_usr_chanmax (8'd0),
    .up_rstn (up_rstn),
    .up_clk (up_clk),
    .up_sel (up_sel_s),
    .up_wr (up_wr_s),
    .up_addr (up_addr_s),
    .up_wdata (up_wdata_s),
    .up_rdata (up_adc_common_rdata_s),
    .up_ack (up_adc_common_ack_s)
);
  // up bus interface

  up_axi #(
    .PCORE_BASEADDR (C_BASEADDR),
    .PCORE_HIGHADDR (C_HIGHADDR))
  i_up_axi (
    .up_rstn (up_rstn),
    .up_clk (up_clk),
    .up_axi_awvalid (s_axi_awvalid),
    .up_axi_awaddr (s_axi_awaddr),
    .up_axi_awready (s_axi_awready),
    .up_axi_wvalid (s_axi_wvalid),
    .up_axi_wdata (s_axi_wdata),
    .up_axi_wstrb (s_axi_wstrb),
    .up_axi_wready (s_axi_wready),
    .up_axi_bvalid (s_axi_bvalid),
    .up_axi_bresp (s_axi_bresp),
    .up_axi_bready (s_axi_bready),
    .up_axi_arvalid (s_axi_arvalid),
    .up_axi_araddr (s_axi_araddr),
    .up_axi_arready (s_axi_arready),
    .up_axi_rvalid (s_axi_rvalid),
    .up_axi_rresp (s_axi_rresp),
    .up_axi_rdata (s_axi_rdata),
    .up_axi_rready (s_axi_rready),
    .up_sel (up_sel_s),
    .up_wr (up_wr_s),
    .up_addr (up_addr_s),
    .up_wdata (up_wdata_s),
    .up_rdata (up_rdata),
    .up_ack (up_ack));

endmodule

// ***************************************************************************
// ***************************************************************************

