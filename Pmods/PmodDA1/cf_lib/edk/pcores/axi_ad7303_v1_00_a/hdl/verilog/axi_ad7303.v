`timescale 1ns / 1ps
// -----------------------------------------------------------------------------
//                                                                 
// Copyright 2011(c) Analog Devices, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//  - Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//  - Neither the name of Analog Devices, Inc. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//  - The use of this software may or may not infringe the patent rights
//    of one or more patent holders.  This license does not release you
//    from the requirement that you obtain separate licenses from these
//    patent holders to use this software.
//  - Use of the software either in source or binary form, must be run
//    on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY 
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//                                                                  
// -----------------------------------------------------------------------------
// FILE NAME : axi_ad7303.v
// MODULE NAME : axi_ad7303.v
// AUTHOR : ATofan
// AUTHOR'S EMAIL : alexandru.tofan@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: $WCREV$
// -----------------------------------------------------------------------------
// KEYWORDS : AD7303, PmodDA1
// -----------------------------------------------------------------------------
// PURPOSE : Short description of functionality
// -----------------------------------------------------------------------------
// REUSE ISSUES
// Reset Strategy      :
// Clock Domains       :
// Critical Timing     :
// Test Features       :
// Asynchronous I/F    :
// Instantiations      :
// Synthesizable (y/n) :
// Target Device       :
// Other               :
// -----------------------------------------------------------------------------
//------------------------------------------------------------------------------   
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------ 
module axi_ad7303
    (
        // Physical interface
        cs_o,
        data0_o,
        data1_o,
        sclk_o,
        ref_clk,
        tx_clk,
        
        // VDMA interface
        m_axis_mm2s_clk,
        m_axis_mm2s_fsync,
        m_axis_mm2s_tvalid,
        m_axis_mm2s_tdata,
        m_axis_mm2s_tkeep,
        m_axis_mm2s_tlast,
        m_axis_mm2s_tready,        
        
        // AXI interface
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
        s_axi_rready
    );

// Physical interface
output          cs_o;
output          data0_o;
output          data1_o;
output          sclk_o;    
input           ref_clk;
input           tx_clk;

// VDMA interface
input           m_axis_mm2s_clk;
output          m_axis_mm2s_fsync;
input           m_axis_mm2s_tvalid;
input   [31:0]  m_axis_mm2s_tdata;
input   [ 3:0]  m_axis_mm2s_tkeep;
input           m_axis_mm2s_tlast;
output          m_axis_mm2s_tready;

// AXI interface
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
output  [31:0]  s_axi_rdata;
output  [ 1:0]  s_axi_rresp;
input           s_axi_rready;    
    
//------------------------------------------------------------------------------
//----------------- Parameters -------------------------------------------------
//------------------------------------------------------------------------------     
parameter   PCORE_ID = 0;
parameter   PCORE_DEVICE_TYPE = 0;
parameter   PCORE_IODELAY_GROUP = "dev_if_delay_group";
parameter   C_S_AXI_MIN_SIZE = 32'hffff;
parameter   C_BASEADDR = 32'hffffffff;
parameter   C_HIGHADDR = 32'h00000000;  

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
// Internal signals
reg             up_ack_s;
reg     [31:0]  up_rdata_s;
reg             dac_rd_s;
reg     [15:0]  dac_data0_s;
reg     [15:0]  dac_data1_s;

//------------------------------------------------------------------------------
//----------- Wires Declarations -----------------------------------------------
//------------------------------------------------------------------------------
wire            up_clk;
wire            up_rstn;
wire            up_ack_s2;
wire    [13:0]  up_addr_s;
wire    [31:0]  up_wdata_s;
wire    [31:0]  up_rdata_s2;
wire            up_sel_s;
wire            up_wr_s;
wire            vdma_clk_s;
wire            vdma_valid_s;
wire    [31: 0] vdma_data_s;
wire            vdma_fs_s;
wire            vdma_ready_s;
wire            vdma_rst_s;
wire            vdma_ovf_s;
wire            vdma_unf_s;
wire    [31:0]  vdma_frmcnt_s;
wire            dac_enable_s;
wire            dac_enable_out_s;
wire    [31:0]  dac_vdma_data_s;
wire            dac_rst_s;
wire            dac_clk_s;

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
// Signal name changes
assign  up_clk              = s_axi_aclk;
assign  dac_clk_s           = s_axi_aclk;
assign  up_rstn             = s_axi_aresetn;
assign  vdma_clk_s          = m_axis_mm2s_clk;  
assign  vdma_valid_s        = m_axis_mm2s_tvalid;
assign  vdma_data_s         = m_axis_mm2s_tdata;
assign  m_axis_mm2s_fsync   = vdma_fs_s;
assign  m_axis_mm2s_tready  = vdma_ready_s;  

assign dac_enable_s = dac_enable_out_s;

always @(posedge dac_clk_s)
begin
    dac_rd_s <= dac_enable_s;
end

// DAC outputs
always @(dac_clk_s)
begin
    dac_data0_s <= dac_vdma_data_s[15: 0];
    dac_data1_s <= dac_vdma_data_s[31:16];
end

// Processor read interface
always @(negedge up_rstn or posedge up_clk)
begin
    if(up_rstn == 0)
    begin
        up_rdata_s <= 32'd0;
        up_ack_s   <= 1'b0; 
    end
    else
    begin
        up_rdata_s <= up_rdata_s2;
        up_ack_s   <= up_ack_s2;
    end
end
  
// Device instantiation
axi_ad7303_dev_if ad7303_i 
    (
        .fpga_clk_i(ref_clk), 
        .dac_clk_i(tx_clk), 
        .reset_n_i(~dac_rst_s), 
        .data0_i(dac_data0_s), 
        .data1_i(dac_data1_s), 
        .en_dac_i(1'b1), 
        .cs_o(cs_o), 
        .data0_o(data0_o), 
        .data1_o(data1_o), 
        .sclk_o(sclk_o)
    );

// DAC Common registers instantiation    
up_dac_common 
    #(
        .PCORE_ID(PCORE_ID)
    )
    up_dac_common_i 
    (
        .mmcm_rst(), 
        .dac_clk(dac_clk_s), 
        .dac_rst(dac_rst_s), 
        .dac_enable(dac_enable_out_s), 
        .vdma_clk(vdma_clk_s), 
        .vdma_rst(vdma_rst_s), 
        .vdma_frmcnt(vdma_frmcnt_s), 
        .vdma_ovf(vdma_ovf_s), 
        .vdma_unf(vdma_unf_s), 
        .up_rstn(up_rstn), 
        .up_clk(up_clk), 
        .up_sel(up_sel_s), 
        .up_wr(up_wr_s), 
        .up_addr(up_addr_s), 
        .up_wdata(up_wdata_s), 
        .up_rdata(up_rdata_s2), 
        .up_ack(up_ack_s2)
    );

// AXI Interface instantiation    
up_axi 
    #(
        .PCORE_BASEADDR (C_BASEADDR),
        .PCORE_HIGHADDR (C_HIGHADDR)
    )
    up_axi_i 
    (
        .up_rstn(up_rstn), 
        .up_clk(up_clk), 
        .up_axi_awvalid(s_axi_awvalid), 
        .up_axi_awaddr(s_axi_awaddr),  
        .up_axi_awready(s_axi_awready), 
        .up_axi_wvalid(s_axi_wvalid), 
        .up_axi_wdata(s_axi_wdata),  
        .up_axi_wstrb(s_axi_wstrb), 
        .up_axi_wready(s_axi_wready), 
        .up_axi_bvalid(s_axi_bvalid), 
        .up_axi_bresp(s_axi_bresp), 
        .up_axi_bready(s_axi_bready), 
        .up_axi_arvalid(s_axi_arvalid), 
        .up_axi_araddr(s_axi_araddr), 
        .up_axi_arready(s_axi_arready), 
        .up_axi_rvalid(s_axi_rvalid), 
        .up_axi_rresp(s_axi_rresp), 
        .up_axi_rdata(s_axi_rdata), 
        .up_axi_rready(s_axi_rready), 
        .up_sel(up_sel_s), 
        .up_wr(up_wr_s), 
        .up_addr(up_addr_s), 
        .up_wdata(up_wdata_s), 
        .up_rdata(up_rdata_s), 
        .up_ack(up_ack_s)
    );

// VDMA Core Interface instantiation
vdma_core 
    #(
        .DATA_WIDTH(32)
    )
    vdma_core_i 
    (
        .vdma_clk(vdma_clk_s), 
        .vdma_rst(vdma_rst_s), 
        .vdma_fs(vdma_fs_s), 
        .vdma_valid(vdma_valid_s), 
        .vdma_data(vdma_data_s), 
        .vdma_ready(vdma_ready_s), 
        .vdma_ovf(vdma_ovf_s), 
        .vdma_unf(vdma_unf_s), 
        .dac_clk(cs_o),
        .dac_rst(dac_rst_s), 
        .dac_rd(dac_rd_s), 
        .dac_valid(), 
        .dac_data(dac_vdma_data_s), 
        .vdma_frmcnt(vdma_frmcnt_s)
    );    
endmodule
