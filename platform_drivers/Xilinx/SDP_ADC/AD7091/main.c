/**************************************************************************//**
*   @file   main.c
*   @brief  Implementation of main program for FMC-SDP evaluation projects.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "Communication.h"
#include "xaxidma.h"
#include "xdma_config.h"
#include "ad7091.h"

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/

#define NULL        ((void *)0)
#define SUCCESS      0
#define ERROR       -1

/* ADC core base address */
#define ADC_BASEADDR        XPAR_AXI_AD7091_0_BASEADDR

/* Memory allocation macros */
#define DMA_DEV_ID          XPAR_AXIDMA_0_DEVICE_ID
#define DMA_BASEADDR        XPAR_AXI_DMA_0_BASEADDR
#define DDR_BASEADDR        XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#define MEM_BASEADDR        (DDR_BASEADDR + 0x1000000)
// memory space for the buffer descriptors (63 descriptors)
#define RX_BD_SPACE_BASE    (MEM_BASEADDR + 0x00001000)
#define RX_BD_SPACE_HIGH    (MEM_BASEADDR + 0x00001FFF)
// memory space for the data buffer
#define RX_BUFFER_BASE      (MEM_BASEADDR + 0x00300000)
#define RX_BUFFER_HIGH      (MEM_BASEADDR + 0x004FFFFF)

// Number of samples
// Note: XDMA buffer length is 14 -> max sample is 2^14 - > 16k
#define SAMPLE_NO           16 * 1024

// maximum packet length per descriptor
#define MAX_PKT_LEN         0x400
// number of packets
#define NUMBER_OF_PACKETS   0x20
// Stream Data Width is 32
#define BUS_WIDTH           4

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
/* Device instance definitions */
XAxiDma AxiDma;
/* Global variable for script interactions */
int startAcquisition = 0;
u32 * prxData = NULL;
u32 numberOfSample = 0;

/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

static int AxiDmaSetup(XAxiDma * AxiDmaInstPtr);

/**************************************************************************//**
* @brief Main function implementation.
*
* @return None
******************************************************************************/
int main(void)
{
    int Status;

    // pointer to the Rx Buffer
    prxData = (u32*)RX_BUFFER_BASE;
    // number of samples
    numberOfSample = SAMPLE_NO;

    // initialize the UART
    UART_Init(115200);

    // setup the AXI DMA
    Status = AxiDmaSetup(&AxiDma);
    if(Status != SUCCESS)
    {
        xil_printf("AXI DMA setup failed!\r\n");
        return ERROR;
    }
    else
    {
        xil_printf("AXI DMA setup complete.\r\n");
    }

    // initialize the ADC
    AD7091_Init(ADC_BASEADDR);

    // waiting for script
    while(!startAcquisition);

    // start data acquisition
    Status = AD7091_GetData(numberOfSample);
    if(Status != SUCCESS)
    {
        xil_printf("Overflow occurred\r\n");
    }
    xil_printf("Data transaction was finished!\r\n");

    // waiting for script to save the received data
    while(startAcquisition);

    xil_printf("Data was saved to Acquisition.csv file!\r\n");

    return SUCCESS;
}

/**************************************************************************//**
* @brief    This function sets up RX channel of the DMA engine to be ready
*           for packet reception
*
* @param    AxiDmaInstPtr is the pointer to the instance of the DMA engine.
*
* @return   SUCCESS if the setup is successful, ERROR otherwise.
*
******************************************************************************/
static int AxiDmaSetup(XAxiDma * AxiDmaInstPtr)
{
    XAxiDma_BdRing *RxRingPtr;
    XAxiDma_Config *Config;
    int Delay = 0;
    int Coalesce = 1;
    int Status;
    XAxiDma_Bd BdTemplate;
    XAxiDma_Bd *BdPtr;
    XAxiDma_Bd *BdCurPtr;
    u32 BdCount;
    u32 FreeBdCount;
    u32 RxBufferPtr;
    int Index;

    Config = XAxiDma_LookupConfig(DMA_DEV_ID);
    if (!Config) {
        xil_printf("No configuration found for %d\r\n", DMA_DEV_ID);
        return ERROR;
    }

    /* Initialize DMA engine */
    Status = XAxiDma_CfgInitialize(AxiDmaInstPtr, Config);
    if (Status != SUCCESS) {
        xil_printf("Initialization failed %d\r\n", Status);
        return ERROR;
    }

    if(!XAxiDma_HasSg(AxiDmaInstPtr)) {
        xil_printf("Device configured as Simple mode \r\n");
        return ERROR;
    }

    RxRingPtr = XAxiDma_GetRxRing(AxiDmaInstPtr);

    /* Disable all RX interrupts before RxBD space setup */

    XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

    /* Set delay and coalescing */
    XAxiDma_BdRingSetCoalesce(RxRingPtr, Coalesce, Delay);

    /* Setup Rx BD space */
    BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,
                RX_BD_SPACE_HIGH - RX_BD_SPACE_BASE + 1);

    Status = XAxiDma_BdRingCreate(RxRingPtr, RX_BD_SPACE_BASE,
                RX_BD_SPACE_BASE,
                XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);

    if (Status != SUCCESS) {
        xil_printf("RX create BD ring failed %d\r\n", Status);
        return ERROR;
    }

    /*
     * Setup an all-zero BD as the template for the Rx channel.
     */
    XAxiDma_BdClear(&BdTemplate);

    Status = XAxiDma_BdRingClone(RxRingPtr, &BdTemplate);
    if (Status != SUCCESS) {
        xil_printf("RX clone BD failed %d\r\n", Status);
        return ERROR;
    }

    /* Attach buffers to RxBD ring so we are ready to receive packets */

    FreeBdCount = XAxiDma_BdRingGetFreeCnt(RxRingPtr);

    Status = XAxiDma_BdRingAlloc(RxRingPtr, FreeBdCount, &BdPtr);
    if (Status != SUCCESS) {
        xil_printf("RX alloc BD failed %d\r\n", Status);
        return ERROR;
    }

    BdCurPtr = BdPtr;
    RxBufferPtr = RX_BUFFER_BASE;
    for (Index = 0; Index < FreeBdCount; Index++) {
        Status = XAxiDma_BdSetBufAddr(BdCurPtr, RxBufferPtr);

        if (Status != SUCCESS) {
            xil_printf("Set buffer addr %x on BD %x failed %d\r\n",
                (unsigned int)RxBufferPtr,
                (unsigned int)BdCurPtr, Status);
            return ERROR;
        }

        Status = XAxiDma_BdSetLength(BdCurPtr, MAX_PKT_LEN,
                RxRingPtr->MaxTransferLen);
        if (Status != SUCCESS) {
            xil_printf("Rx set length %d on BD %x failed %d\r\n",
                MAX_PKT_LEN, (unsigned int)BdCurPtr, Status);
            return ERROR;
        }

        /* Receive BDs do not need to set anything for the control
         * The hardware will set the SOF/EOF bits per stream status
         */
        XAxiDma_BdSetCtrl(BdCurPtr, 0);
        XAxiDma_BdSetId(BdCurPtr, RxBufferPtr);

        RxBufferPtr += MAX_PKT_LEN;
        BdCurPtr = XAxiDma_BdRingNext(RxRingPtr, BdCurPtr);
    }

    /* Clear the receive buffer, so we can verify data
     */
    memset((void *)RX_BUFFER_BASE, 0, (MAX_PKT_LEN * NUMBER_OF_PACKETS * 4));

    Status = XAxiDma_BdRingToHw(RxRingPtr, FreeBdCount,
                        BdPtr);
    if (Status != SUCCESS) {
        xil_printf("RX submit hw failed %d\r\n", Status);
        return ERROR;
    }

    /* Start RX DMA channel */
    Status = XAxiDma_BdRingStart(RxRingPtr);
    if (Status != SUCCESS) {
        xil_printf("RX start hw failed %d\r\n", Status);
        return ERROR;
    }

    return SUCCESS;
}
