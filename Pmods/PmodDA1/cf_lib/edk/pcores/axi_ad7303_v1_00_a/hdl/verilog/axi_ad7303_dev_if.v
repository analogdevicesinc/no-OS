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
// FILE NAME : PmodDA1_if.v
// MODULE NAME : PmodDA1_if.v
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
module axi_ad7303_dev_if
    (
        input           fpga_clk_i,
        input           dac_clk_i,
        input           reset_n_i,
        input   [15:0]  data0_i, 
        input   [15:0]  data1_i,
        input           en_dac_i, 
        output          cs_o,
        output          data0_o,
        output          data1_o,
        output          sclk_o    
    );
    
//------------------------------------------------------------------------------
//----------- Local Parameters -------------------------------------------------
//------------------------------------------------------------------------------    
localparam  DAC_IDLE_STATE      = 32'h00000001;
localparam  DAC_SEND_STATE      = 32'h00000002;
localparam  DAC_SYNC_STATE      = 32'h00000004;
localparam  DAC_SCLK_PERIODS    = 32'd16;

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
reg [31: 0] present_state;
reg [31: 0] present_state_m1;
reg [31: 0] next_state;
reg         en_load_s;
reg         en_shift_s;
reg         cs_s;
reg [15: 0] data0_s;
reg [15: 0] data1_s;
reg [31: 0] sclk_cnt;

//------------------------------------------------------------------------------
//----------- Wires Declarations -----------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
assign cs_o = cs_s;
assign data0_o = data0_s[15];
assign data1_o = data1_s[15];
assign sclk_o = ((present_state_m1 == DAC_SEND_STATE) && (cs_s == 1'b0) && (sclk_cnt != DAC_SCLK_PERIODS)) ? dac_clk_i : 1'b0;

// Allign states to dac clk
always @(posedge dac_clk_i)
begin
    present_state_m1 <= present_state;
end
 
// Register states
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        present_state <= DAC_IDLE_STATE;
    end
    else
    begin
        present_state <= next_state;
    end
end

// Switch states
always @(posedge fpga_clk_i)
begin
    next_state <= present_state;
    case(present_state)
        DAC_IDLE_STATE:
            begin
                if((en_dac_i == 1'b1)&&(sclk_cnt != DAC_SCLK_PERIODS))
                begin
                    next_state <= DAC_SEND_STATE;
                end
            end
        DAC_SEND_STATE: 
            begin
                if(sclk_cnt == DAC_SCLK_PERIODS)
                begin
                    next_state <= DAC_SYNC_STATE;
                end
            end
        DAC_SYNC_STATE:
            begin
                if(cs_s == 1'b1)
                begin
                    next_state <= DAC_IDLE_STATE;
                end
            end
        default:
            begin
                next_state <= DAC_IDLE_STATE;
            end
    endcase
end

// State output
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        cs_s <= 1'b1;
        en_load_s <= 1'b0;
        en_shift_s <= 1'b0;
    end
    else
    begin
        case(present_state)
            DAC_IDLE_STATE:
                begin
                    cs_s <= 1'b1;
                    en_load_s <= 1'b1;
                    en_shift_s <= 1'b0;
                end
            DAC_SEND_STATE:
                begin 
                    cs_s <= 1'b0;
                    en_load_s <= 1'b0;
                    en_shift_s <= 1'b1;                
                end
            DAC_SYNC_STATE:
                begin
                    cs_s <= 1'b1;
                    en_load_s <= 1'b0;
                    en_shift_s <= 1'b0;                
                end
            default:    
                begin
                    cs_s <= 1'b1;
                    en_load_s <= 1'b0;
                    en_shift_s <= 1'b0;                
                end
        endcase
    end
end

// Send data
always @(negedge dac_clk_i)
begin
    if(en_load_s == 1'b1)
    begin
        data0_s <= data0_i;
        data1_s <= data1_i;
        sclk_cnt <= 32'd0;
    end
    else if(en_shift_s == 1'b1)
    begin
        if((present_state_m1 == DAC_SEND_STATE) && (cs_s == 1'b0))
        begin
            sclk_cnt <= sclk_cnt + 32'd1;
            data0_s <= {data0_s[14:0], data0_s[15]};
            data1_s <= {data1_s[14:0], data1_s[15]};
        end
    end
end

endmodule
