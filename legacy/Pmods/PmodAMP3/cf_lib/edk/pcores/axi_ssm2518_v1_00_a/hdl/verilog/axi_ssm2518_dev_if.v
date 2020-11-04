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
// FILE NAME : axi_ssm2518_dev_if.v
// MODULE NAME : axi_ssm2518_dev_if
// AUTHOR : ATofan  
// AUTHOR’S EMAIL : alexandru.tofan@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: $WCREV$
// -----------------------------------------------------------------------------
// KEYWORDS : General file searching keywords, leave blank if none.
// -----------------------------------------------------------------------------
// PURPOSE : I2S Core, Simple Stereo Mode, 1 BCLK data delay
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
`timescale 1ns / 1ps

module axi_ssm2518_dev_if
    (
        input           fpga_clk_i,                     // Input clock from FPGA - 100 MHz
        input           reset_n_i,                      // Input reset, active low
        input           en_tx_i,                        // Input Enable Transmission
        output          mclk_o,                         // Output MCLK - 12.5 MHz
        output          bclk_o,                         // Output BCLK - 3.125 MHz serial clock - for a 48 KHz Sample Rate
        output          lrclk_o,                        // Output LRCLK - 32-bit L, 32-bit R
        output          sdata_o,                        // Output SDATA
        input [31:0]    tx_data_i,                      // Input slot data
        output          req_data_o                      // Request new slot data
    );

// BCLK Pulses
parameter   BCLK_PERIODS = 32'd63;                      // 32 bits L Channel, 32 bits R Channel

// FSM States
parameter   I2S_IDLE_STATE          = 32'h00000001;     // I2S Core Idle State. Depending on transmission type, switches to appropiate state
parameter   I2S_LEFT_WRITE_STATE    = 32'h00000002;     // I2S Stereo Mode, write Left channel
parameter   I2S_LEFT_DONE_STATE     = 32'h00000004;     // I2S Left channel write done
parameter   I2S_RIGHT_WRITE_STATE   = 32'h00000008;     // I2S Stereo Mode, write Right channel
parameter   I2S_RIGHT_DONE_STATE    = 32'h00000010;     // I2S Right channel write done

reg [31:0]  clk_divider;                                // Used to divide FPGA clock 
reg [31:0]  bclk_cnt;                                   // Used to count number of BLCKs        
reg         sdata_int_s;                                // Internal sdata_o signal
reg [31:0]  data_int_s;                                 // Internal tx_data_i signal
reg         load_data_s;                                // Load new data signal

reg [31:0]  present_state;
reg [31:0]  next_state;

reg         req_new_data_s;                             // Request new data from uP

wire        bclk_int_s;                                 // Used to shift data

initial
begin
    bclk_cnt = BCLK_PERIODS + 1;
    data_int_s = 32'ha518f05a;
end

assign mclk_o       = clk_divider[1];
assign bclk_int_s   = clk_divider[4];
assign bclk_o       = bclk_int_s;  
assign lrclk_o      = (bclk_cnt >= 32'd32) ? 1'b0 : 1'b1;
assign sdata_o      = sdata_int_s;
assign req_data_o   = req_new_data_s;

// Register states
always @(posedge fpga_clk_i)
begin
    if((reset_n_i == 1'b0)||(en_tx_i == 1'b0))
    begin
        present_state <= I2S_IDLE_STATE;
    end
    else
    begin
        present_state <= next_state;
    end
end

// State switch logic
always @(bclk_cnt, present_state)
begin
    next_state <= present_state;
    case(present_state)
        I2S_IDLE_STATE:
            begin
                next_state <= I2S_LEFT_WRITE_STATE;
            end
        I2S_LEFT_WRITE_STATE:
            begin
                if(bclk_cnt == 32'd31)
                begin
                    next_state <= I2S_LEFT_DONE_STATE;
                end
            end
        I2S_LEFT_DONE_STATE:
            begin
                next_state <= I2S_RIGHT_WRITE_STATE;
            end
        I2S_RIGHT_WRITE_STATE:
            begin
                if(bclk_cnt == 32'd63)
                begin
                    next_state <= I2S_RIGHT_DONE_STATE;
                end
            end
        I2S_RIGHT_DONE_STATE:
            begin
                next_state <= I2S_LEFT_WRITE_STATE;
            end
        default:
            begin
            end
    endcase
end

// State output logic
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        load_data_s     <= 1'b1;
        req_new_data_s  <= 1'b1;
    end
    else
    begin
        case(present_state)
            I2S_IDLE_STATE:
                begin
                    load_data_s     <= 1'b1;
                    req_new_data_s  <= 1'b1;
                end
            I2S_LEFT_WRITE_STATE:
                begin
                    load_data_s     <= 1'b0;
                    req_new_data_s  <= 1'b0;
                end
            I2S_LEFT_DONE_STATE:
                begin
                    load_data_s     <= 1'b1;
                    req_new_data_s  <= 1'b1;
                end
            I2S_RIGHT_WRITE_STATE:
                begin
                    load_data_s     <= 1'b0;
                    req_new_data_s  <= 1'b0;
                end
            I2S_RIGHT_DONE_STATE:
                begin
                    load_data_s     <= 1'b1;
                    req_new_data_s  <= 1'b1;
                end
            default:
                begin
                    load_data_s     <= 1'b1;
                    req_new_data_s  <= 1'b1;
                end
        endcase    
    end
end

// Counter for clock divide process
always @(posedge fpga_clk_i)
begin
    if((reset_n_i == 1'b0)||(en_tx_i == 1'b0))
    begin
        clk_divider <= 32'd0;
    end 
    else
    begin
        clk_divider <= clk_divider + 32'd1;
    end
end

// Counter for BCLK periods
always @(posedge bclk_int_s)
begin
    if((bclk_cnt == 32'd0)||(en_tx_i == 1'b0))
    begin
        bclk_cnt <= BCLK_PERIODS; 
    end
    else
    begin
        bclk_cnt <= bclk_cnt - 32'd1;
    end
end

// Shift data out
always @(negedge bclk_int_s or posedge load_data_s)
begin
    if((load_data_s == 1'b1)||(en_tx_i == 1'b0))
    begin
        data_int_s <= tx_data_i;
    end
    else
    begin
        sdata_int_s <= data_int_s[31];
        data_int_s <= {data_int_s[30:0], 1'b0};
    end
end

endmodule
