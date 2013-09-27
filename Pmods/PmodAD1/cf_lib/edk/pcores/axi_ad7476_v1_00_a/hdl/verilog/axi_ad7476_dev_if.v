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
// FILE NAME : PmodAD1_if.v
// MODULE NAME : PmodAD1_if
// AUTHOR : atofan
// AUTHOR’S EMAIL : alexandru.tofan@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: 468
// -----------------------------------------------------------------------------
// KEYWORDS : PmodAD1, AD7476A
// -----------------------------------------------------------------------------
// PURPOSE : Driver for the AD7476A 12 Bit 1 MSPS ADC
// -----------------------------------------------------------------------------
// REUSE ISSUES
// Reset Strategy      : Active low reset signal
// Clock Domains       : N/A
// Critical Timing     : N/A
// Test Features       : N/A
// Asynchronous I/F    : N/A
// Instantiations      : N/A
// Synthesizable (y/n) : Y
// Target Device       : AD7476A
// Other               : 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------
module axi_ad7476_dev_if
    (
        //clock and reset signals
        input           fpga_clk_i,
        input           adc_clk_i,
        input           reset_n_i,
        
        //IP control and data interface
        input           en_0_i,
        input           en_1_i,        
        output          data_rdy_o,
        output  [15:0]  data_0_o,
        output  [15:0]  data_1_o,
        
        //ADC control and data interface
        input           data_0_i,
        input           data_1_i,
        output          sclk_o,
        output          cs_o
    );

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
reg [ 7:0]  adc_state; 
reg [ 7:0]  adc_next_state;
reg [ 7:0]  adc_state_m1;
reg [31:0]  adc_tcycle_cnt;
reg [31:0]  adc_tcs_cnt;  
reg [31:0]  sclk_cnt;
reg         data_rd_rdy_s;
reg         adc_cs_s;
reg [15:0]  data_0_s;
reg [15:0]  data_1_s;
reg         adc_clk_en;

//------------------------------------------------------------------------------
//----------- Local Parameters -------------------------------------------------
//------------------------------------------------------------------------------
localparam      ADC_IDLE_STATE      = 8'b00000001;
localparam      ADC_START_STATE     = 8'b00000010;
localparam      ADC_READ_STATE      = 8'b00000100;
localparam      ADC_DONE_STATE      = 8'b00001000;
localparam real FPGA_CLOCK_FREQ     = 100000000;
localparam real ADC_CYCLE_TIME      = 0.000001000; 
localparam real ADC_CS_TIME         = 0.000000020; 
localparam      ADC_CYCLE_CNT       = FPGA_CLOCK_FREQ * ADC_CYCLE_TIME - 1;
localparam      ADC_CS_CNT          = FPGA_CLOCK_FREQ * ADC_CS_TIME;
localparam      ADC_SCLK_PERIODS    = 16; 

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
assign sclk_o       = (adc_clk_en == 1'b1)&&(sclk_cnt >= 32'd0) ? adc_clk_i : 1'b1;
assign cs_o         = adc_cs_s;
assign data_0_o     = (data_rd_rdy_s == 1'b1) ? data_0_s : data_0_o;
assign data_1_o     = (data_rd_rdy_s == 1'b1) ? data_1_s : data_1_o;  
assign data_rdy_o   = data_rd_rdy_s;

always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        adc_tcycle_cnt  <= 32'd0;
        adc_tcs_cnt     <= ADC_CS_CNT;
    end
    else
    begin
        if(adc_tcycle_cnt != 32'd0)
        begin   
            adc_tcycle_cnt <= adc_tcycle_cnt - 32'd1;
        end
        else if(adc_state == ADC_IDLE_STATE)
        begin
            adc_tcycle_cnt <= ADC_CYCLE_CNT;
        end
        
        if(adc_state == ADC_START_STATE)
        begin
            adc_tcs_cnt <= adc_tcs_cnt - 32'd1;
        end
        else
        begin
            adc_tcs_cnt <= ADC_CS_CNT;
        end
    end
end

always @(negedge adc_clk_i)
begin
    if(adc_clk_en == 1'b1)
    begin
        sclk_cnt <= sclk_cnt - 32'd1;
        data_0_s <= {data_0_s[14:0], data_0_i};
        data_1_s <= {data_1_s[14:0], data_1_i};
    end
    else
    begin
        sclk_cnt <= ADC_SCLK_PERIODS;
    end
end

always @(posedge adc_clk_i)
begin
    adc_state_m1 <= adc_state;
    adc_clk_en   <= ((adc_state_m1 == ADC_READ_STATE) && (sclk_cnt != 0) && (adc_state != ADC_IDLE_STATE)) ? 1'b1 : 1'b0;    
end

always @(adc_state, adc_tcycle_cnt, adc_tcs_cnt, sclk_cnt)
begin
    adc_next_state <= adc_state;
    case(adc_state)
        ADC_IDLE_STATE:
            begin
                if(((en_0_i == 1'b1)||(en_1_i == 1'b1))&&(adc_tcycle_cnt == 32'd0))
                begin
                    adc_next_state <= ADC_START_STATE;
                end
            end
        ADC_START_STATE:
            begin
                if(adc_tcs_cnt == 32'd0)
                begin
                    adc_next_state <= ADC_READ_STATE;
                end
            end
        ADC_READ_STATE:
            begin                
                if(sclk_cnt == 32'd0)
                begin
                    adc_next_state <= ADC_DONE_STATE;
                end
            end
        ADC_DONE_STATE:
            begin
                adc_next_state <= ADC_IDLE_STATE;
            end
        default:
            begin
                adc_next_state <= ADC_IDLE_STATE;
            end
    endcase
end

always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        adc_state       <= ADC_IDLE_STATE;
        data_rd_rdy_s   <= 1'b0;
        adc_cs_s        <= 1'b1;
    end
    else
    begin
        adc_state <= adc_next_state;
        case(adc_state)
            ADC_IDLE_STATE:
                begin
                    data_rd_rdy_s   <= 1'b0;  
                    adc_cs_s        <= 1'b1;
                end
            ADC_START_STATE:
                begin
                    data_rd_rdy_s   <= 1'b0;
                    adc_cs_s        <= 1'b1;
                end
            ADC_READ_STATE:
                begin
                    data_rd_rdy_s   <= 1'b0;
                    adc_cs_s        <= 1'b0;
                end
            ADC_DONE_STATE:
                begin
                    data_rd_rdy_s   <= 1'b1;
                    adc_cs_s        <= 1'b0;
                end
            default:
                begin
                    data_rd_rdy_s   <= 1'b0;
                    adc_cs_s        <= 1'b1;
                end
        endcase
    end
end

endmodule
