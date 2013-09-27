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
// FILE NAME : axi_ad7980_dev_if.v
// MODULE NAME : axi_ad7980_dev_if
// AUTHOR : acostina
// AUTHOR’S EMAIL : adrian.costina@analog.com
// -----------------------------------------------------------------------------
// SVN REVISION: $WCWREV$
// -----------------------------------------------------------------------------
// KEYWORDS : AD7980
// -----------------------------------------------------------------------------
// PURPOSE : Driver for the AD7980  16-Bit, 1 MSPS PulSAR ADC in MSOP/QFN
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
// Target Device       : AD7980
// Other               : The driver is intended to be used for AD7980 ADCs configured
//                     : in /CS MODE, 3-WIRE, WITHOUT BUSY INDICATOR
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

`timescale 1ns/1ns //Use a timescale that is best for simulation.

//------------------------------------------------------------------------------
//----------- Module Declaration -----------------------------------------------
//------------------------------------------------------------------------------

module axi_ad7980_dev_if

//----------- Ports Declarations -----------------------------------------------
(
    //clock and reset signals
    input               fpga_clk_i,      //system clock
    input               adc_clk_i,       //clock to be applied to ADC to read the conversions results
    input               reset_i,       //active low reset signal

    //IP control and data interface
    output     [15:0]   data_o,          //data read from the ADC
    output reg          data_rd_ready_o, //when set to high the data read from the ADC is available on the data_o bus
    output reg          adc_status_o,

    //ADC control and data interface
    input               adc_sdo,        //ADC SDO signal
    input               adc_sdi,        //ADC SDI signal (not used in 3-WIRE mode)
    output              adc_sclk_o,     //ADC serial clock
    output              adc_cnv_o       //ADC CNV signal
);

//------------------------------------------------------------------------------
//----------- Registers Declarations -------------------------------------------
//------------------------------------------------------------------------------
reg [ 3:0]  adc_state;      //current state for the ADC control state machine
reg [ 3:0]  adc_next_state; //next state for the ADC control state machine
reg [ 3:0]  adc_state_m1;   //current state for the ADC control state machine in the ADC clock domain

reg [ 6:0]  adc_tcycle_cnt; //counts the number of FPGA clock cycles to determine when an ADC cycle is complete
reg [ 6:0]  adc_tcnv_cnt;   //counts the number of FPGA clock cycles to determine when an ADC conversion is complete
reg [ 4:0]  sclk_clk_cnt;   //counts the number of clocks applied to the ADC to read the conversion result

reg         adc_clk_en;     //gating signal for the clock sent to the ADC
reg         adc_cnv_s;      //internal signal used to hold the state of the ADC CNV signal
reg [15:0]  adc_data_s;     //interal register used to store the data read from the ADC
//------------------------------------------------------------------------------
//----------- Wires Declarations -----------------------------------------------
//------------------------------------------------------------------------------
wire        adc_sclk_s;     //internal signal for the clock sent to the ADC

//------------------------------------------------------------------------------
//----------- Local Parameters -------------------------------------------------
//------------------------------------------------------------------------------
//ADC states
parameter ADC_IDLE_STATE            = 4'b0001;
parameter ADC_START_CNV_STATE       = 4'b0010;
parameter ADC_END_CNV_STATE         = 4'b0100;
parameter ADC_READ_CNV_RESULT       = 4'b1000;

//ADC timing
parameter real FPGA_CLOCK_FREQ  = 100000000;    //FPGA clock frequency [Hz]
parameter real ADC_CYCLE_TIME   = 0.000001000;  //minimum time between two ADC conversions (Tcyc) [s]
parameter real ADC_CONV_TIME    = 0.000000670;  //conversion time (Tcnvh) [s]
parameter [6:0] ADC_CYCLE_CNT   = FPGA_CLOCK_FREQ * ADC_CYCLE_TIME - 1;
parameter [6:0] ADC_CNV_CNT     = FPGA_CLOCK_FREQ * ADC_CONV_TIME;

//ADC serial clock periods
parameter ADC_SCLK_PERIODS  = 5'd15; //number of clocks to be sent to the ADC to read the conversion result

//------------------------------------------------------------------------------
//----------- Assign/Always Blocks ---------------------------------------------
//------------------------------------------------------------------------------
assign adc_cnv_o      = adc_cnv_s;
assign adc_sclk_s     = adc_clk_i & adc_clk_en;
assign adc_sclk_o     = adc_sclk_s;
assign data_o         = adc_data_s;

//update the ADC timing counters
always @(posedge fpga_clk_i)
begin
    if(reset_i == 1'b1)
    begin
        adc_tcycle_cnt  <= 0;
        adc_tcnv_cnt    <= ADC_CNV_CNT;
    end
    else
    begin
        if(adc_tcycle_cnt != 0)
        begin
            adc_tcycle_cnt <= adc_tcycle_cnt - 1;
        end
        else if(adc_state == ADC_IDLE_STATE)
        begin
            adc_tcycle_cnt <= ADC_CYCLE_CNT;
        end

        if(adc_state == ADC_START_CNV_STATE)
        begin
            adc_tcnv_cnt <= adc_tcnv_cnt - 1;
        end
        else
        begin
           adc_tcnv_cnt <= ADC_CNV_CNT;
        end
    end
end

//read data from the ADC
always @(negedge adc_clk_i)
begin
    if(adc_clk_en == 1'b1)
    begin
        adc_data_s   <= {adc_data_s[14:0], adc_sdo};
        sclk_clk_cnt <= sclk_clk_cnt - 1;
    end
    else
    begin
        sclk_clk_cnt <= ADC_SCLK_PERIODS;
    end
end

//determine when the ADC clock is valid to be sent to the ADC
always @(negedge adc_clk_i)
begin
    adc_state_m1 <= adc_state;
    adc_clk_en   <= ((adc_state_m1 == ADC_END_CNV_STATE) || (adc_state_m1 == ADC_READ_CNV_RESULT) && (sclk_clk_cnt != 0)) ? 1'b1 : 1'b0;
end

//update the ADC current state and the control signals
always @(posedge fpga_clk_i)
begin
    if(reset_i == 1'b1)
    begin
        adc_state <= ADC_IDLE_STATE;
        adc_status_o <= 1'b0;
    end
    else
    begin
        adc_state <= adc_next_state;
        case (adc_state)
            ADC_IDLE_STATE:
            begin
                adc_cnv_s       <= 1'b0;
                data_rd_ready_o <= 1'b0;
            end
            ADC_START_CNV_STATE:
            begin
                adc_cnv_s       <= 1'b1;
                data_rd_ready_o <= 1'b0;
            end
            ADC_END_CNV_STATE:
            begin
                adc_cnv_s       <= 1'b0;
                data_rd_ready_o <= 1'b1;
            end
                ADC_READ_CNV_RESULT:
            begin
                adc_cnv_s       <= 1'b0;
                data_rd_ready_o <= 1'b0;
                adc_status_o    <= 1'b1;
            end
        endcase
    end
end

//update the ADC next state
always @(adc_state, adc_tcycle_cnt, adc_tcnv_cnt, sclk_clk_cnt)
begin
    adc_next_state <= adc_state;
    case (adc_state)
        ADC_IDLE_STATE:
        begin
            if(adc_tcycle_cnt == 0)
                begin
                    adc_next_state <= ADC_START_CNV_STATE;
                end
        end
        ADC_START_CNV_STATE:
        begin
            if(adc_tcnv_cnt == 0)
            begin
                adc_next_state <= ADC_END_CNV_STATE;
            end
        end
        ADC_END_CNV_STATE:
        begin
            adc_next_state <= ADC_READ_CNV_RESULT;
        end
        ADC_READ_CNV_RESULT:
        begin
            if(sclk_clk_cnt == 1)
            begin
                adc_next_state <= ADC_IDLE_STATE;
            end
        end
        default:
        begin
            adc_next_state <= ADC_IDLE_STATE;
        end
    endcase
end

endmodule
