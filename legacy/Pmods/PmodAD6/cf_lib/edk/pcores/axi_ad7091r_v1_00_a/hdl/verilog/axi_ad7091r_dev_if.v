// -----------------------------------------------------------------------------
//
// Copyright 2013(c) Analog Devices, Inc.
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
// FILE NAME : AD7091R.v
// MODULE NAME : AD7091R
// AUTHOR : acozma
// AUTHOR’S EMAIL : adrian.costina@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: $WCREV$
// -----------------------------------------------------------------------------
// KEYWORDS : AD7091R
// -----------------------------------------------------------------------------
// PURPOSE : Driver for the AD7091R  12-Bit, 1 MSPS Ultralow Power ADC in MSOP
// and 10 Lead LFCSP
// -----------------------------------------------------------------------------
// REUSE ISSUES
// Reset Strategy      : Active low reset signal
// Clock Domains       : 2 clocks - the system clock that drives the internal logic
//                     : and a clock for ADC conversions
// Critical Timing     : N/A
// Test Features       : N/A
// Asynchronous I/F    : N/A
// Instantiations      : N/A
// Synthesizable (y/n) : Y
// Target Device       : AD7091R
// Other               : The driver is intended to be used for AD7091R ADCs configured
//                     : WITHOUT BUSY INDICATOR
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

`timescale 1ns/1ns //Use a timescale that is best for simulation.

//------------------------------------------------------------------------------
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------

module axi_ad7091r_dev_if

//----------- Ports Declarations -----------------------------------------------
(
    //clock and reset signals
    input               fpga_clk_i,     // system clock (100 MHz)
    input               adc_clk_i,      // clock to be applied to ADC to read the conversions results (50 MHZ)
    input               reset_i,        // active high reset signal

    //IP control and data interface
    output     [15:0]   data_o,         // data read from the ADC
    output  reg         data_rd_ready_o,// when set to high the data read from the ADC is available on the data_o bus
    output  reg         adc_status_o,   // signals that the first data acquisition has been performed

    //AD7938 control and data interface
    input               adc_sdo_i,
    output              adc_sclk_o,
    output              adc_cs_n_o,
    output              adc_convst_n_o
);

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
reg [ 7:0]  adc_state;          //current state for the ADC control state machine
reg [ 7:0]  adc_next_state;     //next state for the ADC control state machine
reg [ 7:0]  adc_state_nc_m1;    //current state for the ADC state machine in the ADC clock domain sampled on the falling edge
reg [ 7:0]  adc_state_pc_m1;    //current state for the ADC state machine in the ADC clock domain sampled on the rising edge

reg [ 6:0]  adc_tcycle_cnt;
reg [ 6:0]  adc_tconvst_cnt;
reg [ 6:0]  adc_tconvert_cnt;
reg [ 4:0]  sclk_clk_cnt;

reg         adc_cnv_s;
reg         adc_clk_en = 1'b0;
reg         adc_cs_n_s;
reg [15:0]  adc_data_s;
reg         adc_sw_reset;
reg         data_rd_ready_s;

//------------------------------------------------------------------------------
//----------- Local Parameters -------------------------------------------------
//------------------------------------------------------------------------------
//ADC states
parameter ADC_SW_RESET_STATE        = 8'b00000001;
parameter ADC_IDLE_STATE            = 8'b00000010;
parameter ADC_START_CNV_STATE       = 8'b00000100;
parameter ADC_WAIT_CNV_DONE_STATE   = 8'b00001000;
parameter ADC_WAIT_DATA_VALID_STATE = 8'b00010000;
parameter ADC_READ_CNV_RESULT       = 8'b00100000;
parameter ADC_END_CNV_STATE         = 8'b01000000;
parameter ADC_DATAREADY_STATE       = 8'b10000000;

//ADC timing
parameter real FPGA_CLOCK_FREQ  = 100;    //FPGA clock frequency [MHz]
parameter real ADC_CYCLE_TIME   = 1.000;  //minimum time between two ADC conversions [us]
parameter real ADC_CONVST_TIME  = 0.010;  //minimum time to keep /CONVST low [us]
parameter real ADC_CONVERT_TIME = 0.650;  //conversion time [us]
parameter [6:0] ADC_CYCLE_CNT   = FPGA_CLOCK_FREQ * ADC_CYCLE_TIME - 1;
parameter [6:0] ADC_CONVST_CNT  = FPGA_CLOCK_FREQ * ADC_CONVST_TIME - 1;
parameter [6:0] ADC_CONVERT_CNT = FPGA_CLOCK_FREQ * ADC_CONVERT_TIME - 1;

//ADC serial clock periods
parameter ADC_SCLK_PERIODS       = 5'd12;
parameter ADC_RESET_SCLK_PERIODS = 4'd3;

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
assign adc_sclk_o     = adc_clk_i & adc_clk_en;
assign adc_cs_n_o     = adc_cs_n_s;
assign adc_convst_n_o = adc_cnv_s;
assign data_o         = (data_rd_ready_o == 1'b1) ? adc_data_s : data_o;

always @(negedge fpga_clk_i)
begin
    if(reset_i == 1'b1)
    begin
        data_rd_ready_o <= 1'b0;
    end
    else
    begin
        data_rd_ready_o <= data_rd_ready_s;
    end
end

//update the ADC timing counters
always @(posedge fpga_clk_i)
begin
    if(reset_i == 1'b1)
    begin
        adc_tcycle_cnt   <= 0;
        adc_tconvst_cnt  <= ADC_CONVST_CNT;
        adc_tconvert_cnt <= ADC_CONVERT_CNT;
    end
    else
    begin
        if(adc_tcycle_cnt != 1'b0)
        begin
            adc_tcycle_cnt <= adc_tcycle_cnt - 7'h1;
        end
        else if(adc_state == ADC_IDLE_STATE || adc_state == ADC_SW_RESET_STATE)
        begin
            adc_tcycle_cnt <= ADC_CYCLE_CNT;
        end

        if(adc_state == ADC_START_CNV_STATE)
        begin
            adc_tconvst_cnt <= adc_tconvst_cnt - 7'h1;
        end
        else
        begin
           adc_tconvst_cnt <= ADC_CONVST_CNT;
        end
        if(adc_state == ADC_WAIT_CNV_DONE_STATE)
        begin
            adc_tconvert_cnt <= adc_tconvert_cnt - 7'h1;
        end
        else
        begin
           adc_tconvert_cnt <= ADC_CONVERT_CNT;
        end
    end
end

//determine when the ADC clock is valid to be sent to the ADC
always @(negedge adc_clk_i)
begin
    adc_state_nc_m1 <= adc_state;
    adc_clk_en      <= ((adc_state_nc_m1 == ADC_WAIT_DATA_VALID_STATE) || (adc_state_nc_m1 == ADC_READ_CNV_RESULT) && ( (sclk_clk_cnt != 0) || ((adc_sw_reset == 1'b1) && (sclk_clk_cnt == ADC_SCLK_PERIODS - ADC_RESET_SCLK_PERIODS)))) ? 1'b1 : 1'b0;
end

//read data from the ADC
always @(negedge adc_clk_i)
begin
    adc_state_pc_m1 <= adc_state;
    if(adc_clk_en == 1'b1)
    begin
        adc_data_s   <= {3'b0, adc_data_s[11:0], adc_sdo_i};
        sclk_clk_cnt <= sclk_clk_cnt - 5'h1;
    end
    else if(adc_state_pc_m1 != ADC_READ_CNV_RESULT && adc_state_pc_m1 != ADC_END_CNV_STATE)
    begin
        adc_data_s      <= 16'h0;
        sclk_clk_cnt    <= ADC_SCLK_PERIODS - 1;
    end
end

//update the ADC current state and the control signals
always @(posedge fpga_clk_i)
begin
    if(reset_i == 1'b1)
    begin
        adc_state <= ADC_SW_RESET_STATE;
        adc_status_o <= 1'b0;
    end
    else
    begin
        adc_state <= adc_next_state;
        case (adc_state)
            ADC_SW_RESET_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b1;
                data_rd_ready_s <= 1'b0;
                adc_sw_reset    <= 1'b1;
            end
            ADC_IDLE_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b1;
                data_rd_ready_s <= 1'b0;
                adc_sw_reset    <= 1'b0;
            end
            ADC_START_CNV_STATE:
            begin
                adc_cnv_s       <= 1'b0;
                adc_cs_n_s      <= 1'b1;
                data_rd_ready_s <= 1'b0;
            end
            ADC_WAIT_CNV_DONE_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b1;
                data_rd_ready_s <= 1'b0;
            end
            ADC_WAIT_DATA_VALID_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b1;
                data_rd_ready_s <= 1'b0;
            end
            ADC_READ_CNV_RESULT:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b0;
                data_rd_ready_s <= 1'b0;
            end
            ADC_END_CNV_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b0;
                data_rd_ready_s <= 1'b0;
            end
            ADC_DATAREADY_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                adc_cs_n_s      <= 1'b0;
                data_rd_ready_s <= 1'b1;
                adc_status_o    <= 1'b1;
            end
        endcase
    end
end

//update the ADC next state
always @(adc_state, adc_tcycle_cnt, adc_tconvst_cnt, adc_tconvert_cnt, sclk_clk_cnt, adc_sw_reset)
begin
    adc_next_state <= adc_state;
    case (adc_state)
        ADC_SW_RESET_STATE:
        begin
            adc_next_state <= ADC_START_CNV_STATE;
        end
        ADC_IDLE_STATE:
        begin
            if(adc_tcycle_cnt == 0)
            begin
                adc_next_state <= ADC_START_CNV_STATE;
            end
        end
        ADC_START_CNV_STATE:
        begin
            if(adc_tconvst_cnt == 0)
            begin
                adc_next_state <= ADC_WAIT_CNV_DONE_STATE;
            end
        end
        ADC_WAIT_CNV_DONE_STATE:
        begin
            if(adc_tconvert_cnt == 0)
            begin
                adc_next_state <= ADC_WAIT_DATA_VALID_STATE;
            end
        end
        ADC_WAIT_DATA_VALID_STATE:
        begin
           adc_next_state <= ADC_READ_CNV_RESULT;
        end
        ADC_READ_CNV_RESULT:
        begin
            if((sclk_clk_cnt == 0) || ((adc_sw_reset == 1'b1) && (sclk_clk_cnt == ADC_SCLK_PERIODS - ADC_RESET_SCLK_PERIODS)))
            begin
                adc_next_state <= ADC_END_CNV_STATE;
            end
        end
        ADC_END_CNV_STATE:
        begin
            adc_next_state <= ADC_DATAREADY_STATE;
        end
        ADC_DATAREADY_STATE:
        begin
            adc_next_state <= ADC_IDLE_STATE;
        end
        default:
        begin
            adc_next_state <= ADC_IDLE_STATE;
        end
    endcase
end

endmodule
