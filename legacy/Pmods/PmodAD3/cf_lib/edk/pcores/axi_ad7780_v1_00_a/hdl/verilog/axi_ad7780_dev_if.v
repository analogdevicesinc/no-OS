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
// FILE NAME : ad7780.v 
// MODULE NAME : AD7780
// AUTHOR : ATofan
// AUTHOR’S EMAIL : alexandru.tofan@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: $WCREV$
// -----------------------------------------------------------------------------
// KEYWORDS : General file searching keywords, leave blank if none.
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

`timescale 1ns/100ps    // Use a timescale that is best for simulation

//------------------------------------------------------------------------------   
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------  
module axi_ad7780_dev_if
//----------- Ports Declarations -----------------------------------------------
(
    input           fpga_clk_i,             // System Clock (100 MHz)
    input           adc_clk_i,              // SCLK (5 MHz Maximum)
    input           reset_n_i,              // Reset signal, active low
    input           dout_rdy_n_i,           // DOUT/RDY signal
    input           filter_i,               // FILTER State Input
    input           gain_i,                 // GAIN State Input
    input           pdrst_i,                // PDRST State Input
    output          filter_o,               // FILTER State Output
    output          gain_o,                 // GAIN State Output
    output          pdrst_o,                // PDRST State Output
    output          sclk_o,                 // SCLK
    output [31:0]   data_o,                 // Data read from the device
    output          data_rdy_o              // Data ready signal
);

//------------------------------------------------------------------------------
//----------- Local Parameters -------------------------------------------------
//------------------------------------------------------------------------------
parameter   ADC_IDLE_STATE      = 5'b00001;
parameter   ADC_WAIT_RDY        = 5'b00010;
parameter   ADC_READ_DATA       = 5'b00100;
parameter   ADC_READ_LAST       = 5'b01000;
parameter   ADC_DATA_READY      = 5'b10000;
parameter   ADC_SCLK_PERIODS    = 6'd32;

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
reg [4:0]   present_state;
reg [4:0]   next_state;
reg         adc_clk_en;
reg [5:0]   sclk_cnt;     
reg [31:0]  data_reg;
reg         data_rdy;

//------------------------------------------------------------------------------
//----------- Wires Declarations -----------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
assign data_o = data_reg;
assign filter_o = filter_i;
assign gain_o = gain_i;
assign pdrst_o = pdrst_i;
assign data_rdy_o = data_rdy;
assign sclk_o = (adc_clk_en == 1'b1) ? adc_clk_i : 1'b1;

initial
begin
    data_reg = 32'h0;
end

// Register states
always @(posedge fpga_clk_i)
begin
    if(reset_n_i == 1'b0)
    begin
        present_state <= ADC_IDLE_STATE;
    end
    else
    begin
        present_state <= next_state;
    end
end

// State switching logic
always @(dout_rdy_n_i, sclk_cnt)
begin
    next_state <= present_state;
    case(present_state)
        ADC_IDLE_STATE:
            begin
                if(dout_rdy_n_i == 1'b1)
                begin
                    next_state <= ADC_WAIT_RDY;
                end
            end
        ADC_WAIT_RDY:
            begin
                if((dout_rdy_n_i == 1'b0)&&(adc_clk_i == 1'b0))
                begin
                    next_state <= ADC_READ_DATA;
                end
            end
        ADC_READ_DATA:
            begin
                if(sclk_cnt == 6'h1)
                begin
                    next_state <= ADC_READ_LAST;
                end
            end
        ADC_READ_LAST:
            begin
                if(sclk_cnt == 6'h0)
                begin
                    next_state <= ADC_DATA_READY;
                end
            end
        ADC_DATA_READY:
            begin
                next_state <= ADC_IDLE_STATE;
            end
        default:
            begin
                next_state <= ADC_IDLE_STATE;
            end
    endcase
end

// State output logic
always @(posedge fpga_clk_i)  
begin
    if(reset_n_i == 1'b0)
    begin
        data_rdy <= 1'b0;
    end
    else
    begin
        case(present_state)
            ADC_IDLE_STATE:
                begin
                    data_rdy <= 1'b0;
                end
            ADC_WAIT_RDY:
                begin
                    data_rdy <= 1'b0;
                end
            ADC_READ_DATA:
                begin
                    data_rdy <= 1'b0;
                end
            ADC_READ_LAST:
                begin
                    data_rdy <= 1'b0;
                end
            ADC_DATA_READY:
                begin
                    data_rdy <= 1'b1;
                end
            default:
                begin
                    data_rdy <= 1'b0;
                end
        endcase
    end
end 

// Determine when the ADC Clock is Valid
always @(posedge adc_clk_i) 
begin
    adc_clk_en <= (present_state == ADC_READ_DATA)&&(sclk_cnt > 6'h1);
end

// Read data from the ADC
always @(posedge adc_clk_i)
begin 
    if(adc_clk_en == 1'b1)
    begin
        data_reg <= {data_reg[30:0], dout_rdy_n_i};
        sclk_cnt <= sclk_cnt - 6'h1;
    end
    else
    begin
        sclk_cnt <= ADC_SCLK_PERIODS;
    end
end

endmodule // AD7780