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
// FILE NAME : axi_admp441_dev_if.v
// MODULE NAME : axi_admp441_dev_if
// AUTHOR : atofan
// AUTHOR’S EMAIL : alexandru.tofan@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: 468
// -----------------------------------------------------------------------------
// KEYWORDS : PmodMIC2, ADMP441
// -----------------------------------------------------------------------------
// PURPOSE : Driver for the ADMP441
// -----------------------------------------------------------------------------
// REUSE ISSUES
// Reset Strategy      : Active low reset signal
// Clock Domains       : N/A
// Critical Timing     : N/A
// Test Features       : N/A
// Asynchronous I/F    : N/A
// Instantiations      : N/A
// Synthesizable (y/n) : Y
// Target Device       : ADMP441
// Other               : 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------
module axi_admp441_dev_if
    (
        input               fpga_clk_i,
        input               reset_n_i,
        input               sd_i,
        input               chip_en_i,
        output              sck_o,
        output              ws_o,
        output reg [31:0]   data_o,
        output              data_rdy_o,
        output              data_l_r_o,
        output              chip_en_o
    );
 
// SCK Pulses 
parameter SCK_PERIODS = 7'd63;

// FSM States
parameter LEFT_READ_STATE   = 4'b0001;
parameter LEFT_READY_STATE  = 4'b0010;
parameter RIGHT_READ_STATE  = 4'b0100;
parameter RIGHT_READY_STATE = 4'b1000;
 
reg [4:0] clk_divider;                              // SCK Generator
reg [5:0] present_state;                            // FSM Present State
reg [5:0] next_state;                               // FSM Next State
reg [6:0] sck_cnt;                                  // SCK Counter
reg [31:0] data_int_s;                              // Internal use serial data shift register
reg data_rdy_int_s;                                 // Internal use data ready
reg data_l_r_int_s;                                 // Internal use left/right channel

wire sck_int_s;                                     // Internal use SCK signal

assign sck_int_s = clk_divider[4];                  // 3.125 MHz SCK clock divided from 100 MHz input clock
assign sck_o = sck_int_s;                           // Output SCK
assign ws_o = (sck_cnt >= 7'd32) ? 1'b0 : 1'b1;     // Output WS
assign data_l_r_o = data_l_r_int_s;                 // Output Left/Right
assign data_rdy_o = data_rdy_int_s;                 // Output Data Ready
assign chip_en_o = chip_en_i;                       // Output Chip Enable (just passes Input to Output)

// Initialize values
initial
begin
    sck_cnt = SCK_PERIODS + 1; 
    data_int_s = 32'd0;
end 

// Clock divider
// Divide 100 by 32 to obtain a 3.125 MHz serial clock 
always @(posedge fpga_clk_i) 
begin
    if(reset_n_i == 1'b0) 
    begin
        clk_divider <= 5'd0; 
    end
    else
    begin
        clk_divider <= clk_divider + 5'd1; 
    end 
end

// Count SCK
always @(negedge sck_int_s)
begin
    if(sck_cnt == 7'd0)
    begin
        sck_cnt <= SCK_PERIODS;
    end 
    else
    begin
        sck_cnt <= sck_cnt - 7'd1;
    end
end

// Shift in data
always @(posedge sck_int_s)
begin
    data_int_s <= {data_int_s[30:0], sd_i};
end 

// Register states
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        present_state <= LEFT_READ_STATE;
    end
    else
    begin
        present_state <= next_state;
    end
end

// Switch states
always @(*)
begin
    next_state <= present_state;
    case(present_state)
        LEFT_READ_STATE:
            begin
                if(sck_cnt == 7'd31)
                begin
                    next_state <= LEFT_READY_STATE;
                end
            end
        LEFT_READY_STATE:
            begin
                next_state <= RIGHT_READ_STATE;
            end
        RIGHT_READ_STATE:
            begin 
                if(sck_cnt == 7'd63)
                begin
                    next_state <= RIGHT_READY_STATE;
                end            
            end
        RIGHT_READY_STATE:
            begin
                next_state <= LEFT_READ_STATE;
            end  
        default:
            begin
                next_state <= LEFT_READ_STATE;
            end
    endcase
end

// State output logic
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        data_rdy_int_s <= 1'b0;
        data_l_r_int_s <= 1'b0;
    end
    else
    begin
        case(present_state)
            LEFT_READ_STATE:
                begin
                    data_rdy_int_s <= 1'b0;
                end
            LEFT_READY_STATE:
                begin
                    data_rdy_int_s <= 1'b1;
                    data_l_r_int_s <= 1'b0;
                    data_o <= data_int_s;
                end
            RIGHT_READ_STATE:
                begin
                    data_rdy_int_s <= 1'b0;
                end
            RIGHT_READY_STATE:
                begin
                    data_rdy_int_s <= 1'b1;
                    data_l_r_int_s <= 1'b1;
                    data_o <= data_int_s;
                end  
            default:
                begin
                    data_rdy_int_s <= 1'b0;
                    data_l_r_int_s <= 1'b0;
                end
        endcase
    end
end

endmodule
