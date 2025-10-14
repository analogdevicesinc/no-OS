/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscugic_hw.c
* @addtogroup scugic_v4_7
* @{
*
* This file contains low-level driver functions that can be used to access the
* device.  The user should refer to the hardware device specification for more
* details of the device operation.
* These routines are used when the user does not want to create an instance of
* XScuGic structure but still wants to use the ScuGic device. Hence the
* routines provided here take device id or scugic base address as arguments.
* Separate static versions of DistInit and CPUInit are provided to implement
* the low level driver routines.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.01a sdm  07/18/11 First release
* 1.03a srt  02/27/13 Moved Offset calculation macros from *_hw.c (CR
*                     702687).
*                     Added support to direct interrupts to the appropriate
*                     CPU. Earlier interrupts were directed to CPU1
*                     (hard coded). Now depending upon the CPU selected by
*                     the user (xparameters.h), interrupts will be directed
*                     to the relevant CPU.This fixes CR 699688.
* 1.04a hk   05/04/13 Fix for CR#705621. Moved functions
*                     XScuGic_SetPriTrigTypeByDistAddr and
*                     XScuGic_GetPriTrigTypeByDistAddr here from xscugic.c
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.6   kvn  02/17/17 Add support for changing GIC CPU master at run time.
*       kvn  02/28/17 Make the CpuId as static variable and Added new
*                     XScugiC_GetCpuId to access CpuId.
* 3.9   mus  02/21/18 Added new API's
*                     XScuGic_InterruptUnmapFromCpuByDistAddr and
*                     XScuGic_UnmapAllInterruptsFromCpuByDistAddr, These
*                     API's can be used by applications to unmap
*                     specific/all interrupts from target CPU. It fixes
*                     CR#992490.
* 3.10  mus  07/17/18 Updated XScuGic_DeviceInterruptHandler to fix array
*                     overrun reported by coverity tool. It fixes
*                     CR#1006344.
* 3.10  mus  07/17/18 Updated file to fix the various coding style issues
*                     reported by checkpatch. It fixes CR#1006344.
* 3.10  aru  08/23/18 Resolved MISRA-C:2012 compliance mandatory violations
*                     It fixes CR#1007753
* 3.10  mus  09/19/18 Update documentation for XScuGic_RegisterHandler to
*                     fix doxygen warnings.
* 4.1   asa  03/30/19 Made changes not to direct each interrupt to all
*                     available CPUs by default. This was breaking AMP
*                     behavior. Instead every time an interrupt enable
*                     request is received, the interrupt was mapped to
*                     the respective CPU. There were several other changes
*                     made to implement this including adding APIs:
*                     XScuGic_InterruptMapFromCpuByDistAddr,
*                     XScuGic_EnableIntr, and XScuGic_DisableIntr.
*                     This set of changes was to fix CR-1024716.
* 4.1   mus  06/12/19 Updated existing low level API's to support GIC500. It
*                     fixes CR#1033401.
* 4.3   mus  05/11/20 Added assert checks in XScuGic_EnableIntr and
*                     XScuGic_DisableIntr API. It fixes CR#1063034.
* 4.5   asa  03/07/21 Added spinlock protection for critical sections in the
*                     code. The sections are critical when applications
*                     running on separate CPUs (R5s or A9s) try to update
*                     the same register. The spinlock mechanism used here
*                     used exclusive load and store instructions. To ensure
*                     that legacy behavior is not broken, unless someone
*                     enables spinlocks explicitely in their applications
*                     the existing flow will remain unchanged. On how to
*                     enable spinlocks, please refer to the documentations
*                     at: lib/bsp/standalone/src/arm/common/gcc/xil_spinlock.c
* 4.6	sk   08/05/21 Fix Scugic Misrac violations.
* 4.7	sk   09/14/21 Fix gcc compiler warnings for A72 processor.
* 4.7	sk   10/13/21 Update APIs to perform interrupt mapping/unmapping only
* 		      when (Int_Id >= XSCUGIC_SPI_INT_ID_START).
* </pre>
*
******************************************************************************/


/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xscugic.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

static void DistInit(const XScuGic_Config *Config);
#if !defined (GICv3)
static void CPUInit(const XScuGic_Config *Config);
#endif
static XScuGic_Config *LookupConfigByBaseAddress(u32 CpuBaseAddress);

/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* DistInit initializes the distributor of the GIC. The
* initialization entails:
*
* - Write the trigger mode, priority and target CPU
* - All interrupt sources are disabled
* - Enable the distributor
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	CpuID is the Cpu ID to be initialized.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void DistInit(const XScuGic_Config *Config)
{
	u32 Int_Id;

#if (defined (USE_AMP) && (USE_AMP==1))
	#warning "Building GIC for AMP"

	/*
	 * The distributor should not be initialized by FreeRTOS in the case of
	 * AMP -- it is assumed that Linux is the master of this device in that
	 * case.
	 */
	return;
#endif

#if defined (GICv3)
	u32 Temp;
	u32 Waker_State;

	Waker_State = XScuGic_ReadReg((Config->DistBaseAddress) +
			XSCUGIC_RDIST_OFFSET,XSCUGIC_RDIST_WAKER_OFFSET);
	XScuGic_WriteReg((Config->DistBaseAddress) +
			XSCUGIC_RDIST_OFFSET,XSCUGIC_RDIST_WAKER_OFFSET,
			Waker_State & (~ XSCUGIC_RDIST_WAKER_LOW_POWER_STATE_MASK));
	/* Enable system reg interface through ICC_SRE_EL1 */
#if EL3
	XScuGic_Enable_SystemReg_CPU_Interface_EL3();
#endif
	XScuGic_Enable_SystemReg_CPU_Interface_EL1();
	isb();

	Temp = XScuGic_ReadReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET);
	Temp |= (XSCUGIC500_DCTLR_ARE_NS_ENABLE | XSCUGIC500_DCTLR_ARE_S_ENABLE);
	Temp &= ~(XSCUGIC_EN_INT_MASK);
	XScuGic_WriteReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET, Temp);
#else
	XScuGic_WriteReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET, 0U);
#endif

	/*
	 * Set the security domains in the int_security registers for non-secure
	 * interrupts. All are secure, so leave at the default. Set to 1 for
	 * non-secure interrupts.
	 */


	/*
	 * For the Shared Peripheral Interrupts INT_ID[MAX..32], set:
	 */

	/*
	 * 1. The trigger mode in the int_config register
	 * Only write to the SPI interrupts, so start at 32
	 */
	for (Int_Id = 32U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
		Int_Id = Int_Id+16U) {
		/*
		 * Each INT_ID uses two bits, or 16 INT_ID per register
		 * Set them all to be level sensitive, active HIGH.
		 */
		XScuGic_WriteReg(Config->DistBaseAddress,
			XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id), 0U);
	}


#define DEFAULT_PRIORITY	0xa0a0a0a0U
	for (Int_Id = 0U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+4U) {
		/*
		 * 2. The priority using int the priority_level register
		 * The priority_level and spi_target registers use one byte per
		 * INT_ID.
		 * Write a default value that can be changed elsewhere.
		 */
		XScuGic_WriteReg(Config->DistBaseAddress,
				XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id),
				DEFAULT_PRIORITY);
	}

#if defined (GICv3)
	for (Int_Id = 0U; Int_Id<XSCUGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+32U) {
		XScuGic_WriteReg(Config->DistBaseAddress,
				XSCUGIC_SECURITY_TARGET_OFFSET_CALC(Int_Id),
				XSCUGIC_DEFAULT_SECURITY);
	}
	/*
	 * Set security for SGI/PPI
	 *
	 */
	XScuGic_WriteReg( Config->DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_IGROUPR_OFFSET, XSCUGIC_DEFAULT_SECURITY);
#endif

	for (Int_Id = 0U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+32U) {
		/*
		 * 4. Enable the SPI using the enable_set register.
		 * Leave all disabled for now.
		 */
		XScuGic_WriteReg(Config->DistBaseAddress,
		XSCUGIC_EN_DIS_OFFSET_CALC(XSCUGIC_DISABLE_OFFSET,
		Int_Id),
		0xFFFFFFFFU);

	}

#if defined (GICv3)
	Temp = XScuGic_ReadReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET);
	Temp |= XSCUGIC_EN_INT_MASK;
	XScuGic_WriteReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET, Temp);
	XScuGic_Enable_Group1_Interrupts();
	XScuGic_Enable_Group0_Interrupts();
	XScuGic_set_priority_filter(0xff);
#else

	XScuGic_WriteReg(Config->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET,
						XSCUGIC_EN_INT_MASK);
#endif

}

#if !defined (GICv3)
/*****************************************************************************/
/**
*
* CPUInit initializes the CPU Interface of the GIC. The initialization entails:
*
* - Set the priority of the CPU.
* - Enable the CPU interface
*
* @param	ConfigPtr is a pointer to a config table for the particular
*		device this driver is associated with.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void CPUInit(const XScuGic_Config *Config)
{
	/*
	 * Program the priority mask of the CPU using the Priority mask
	 * register
	 */
	XScuGic_WriteReg(Config->CpuBaseAddress, XSCUGIC_CPU_PRIOR_OFFSET,
									0xF0U);

	/*
	 * If the CPU operates in both security domains, set parameters in the
	 * control_s register.
	 * 1. Set FIQen=1 to use FIQ for secure interrupts,
	 * 2. Program the AckCtl bit
	 * 3. Program the SBPR bit to select the binary pointer behavior
	 * 4. Set EnableS = 1 to enable secure interrupts
	 * 5. Set EnbleNS = 1 to enable non secure interrupts
	 */

	/*
	 * If the CPU operates only in the secure domain, setup the
	 * control_s register.
	 * 1. Set FIQen=1,
	 * 2. Set EnableS=1, to enable the CPU interface to signal secure .
	 * interrupts Only enable the IRQ output unless secure interrupts
	 * are needed.
	 */
	XScuGic_WriteReg(Config->CpuBaseAddress, XSCUGIC_CONTROL_OFFSET, 0x07U);

}
#endif

/*****************************************************************************/
/**
*
* Initialize the GIC based on the device id. The
* initialization entails:
*
* - Initialize distributor interface
* - Initialize cpu interface
*
* @param DeviceId is device id to be worked on.
*
* @return
*
* - XST_SUCCESS if initialization was successful
*
* @note
*
* None.
*
******************************************************************************/
s32 XScuGic_DeviceInitialize(u32 DeviceId)
{
	XScuGic_Config *Config;

	Config = &XScuGic_ConfigTable[(u32)DeviceId];
	DistInit(Config);
#if !defined (GICv3)
	CPUInit(Config);
#endif

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function is the primary interrupt handler for the driver.  It must be
* connected to the interrupt source such that it is called when an interrupt of
* the interrupt controller is active. It will resolve which interrupts are
* active and enabled and call the appropriate interrupt handler. It uses
* the Interrupt Type information to determine when to acknowledge the
* interrupt.Highest priority interrupts are serviced first.
*
* This function assumes that an interrupt vector table has been previously
* initialized.  It does not verify that entries in the table are valid before
* calling an interrupt handler.
*
* @param	DeviceId is the unique identifier for the ScuGic device.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XScuGic_DeviceInterruptHandler(void *DeviceId)
{

	u32 InterruptID;
#if !defined (GICv3)
	u32 IntIDFull;
#endif
	XScuGic_VectorTableEntry *TablePtr;
	XScuGic_Config *CfgPtr;

	CfgPtr = &XScuGic_ConfigTable[(INTPTR)DeviceId];

#if defined (GICv3)
	InterruptID = XScuGic_get_IntID();
#else
	/*
	 * Read the int_ack register to identify the highest priority
	 * interrupt ID and make sure it is valid. Reading Int_Ack will
	 * clear the interrupt in the GIC.
	 */
	IntIDFull = XScuGic_ReadReg(CfgPtr->CpuBaseAddress,
					XSCUGIC_INT_ACK_OFFSET);
	InterruptID = IntIDFull & XSCUGIC_ACK_INTID_MASK;

#endif
	if (XSCUGIC_MAX_NUM_INTR_INPUTS <= InterruptID) {
		goto IntrExit;
	}

	/*
	 * If the interrupt is shared, do some locking here if there are
	 * multiple processors.
	 */
	/*
	 * If preemption is required:
	 * Re-enable preemption by setting the CPSR I bit for non-secure ,
	 * interrupts or the F bit for secure interrupts
	 */

	/*
	 * If we need to change security domains, issue a SMC instruction here.
	 */

	/*
	 * Execute the ISR. Jump into the Interrupt service routine based on
	 * the IRQSource. A software trigger is cleared by the ACK.
	 */
	TablePtr = &(CfgPtr->HandlerTable[InterruptID]);
	if (TablePtr != NULL) {
		TablePtr->Handler(TablePtr->CallBackRef);
	}

IntrExit:
	/*
	 * Write to the EOI register, we are all done here.
	 * Let this function return, the boot code will restore the stack.
	 */
#if defined (GICv3)
	XScuGic_ack_Int(InterruptID);
#else
	XScuGic_WriteReg(CfgPtr->CpuBaseAddress, XSCUGIC_EOI_OFFSET, IntIDFull);
#endif

	/*
	 * Return from the interrupt. Change security domains could happen
	 * here.
	 */
}

/*****************************************************************************/
/**
*
* Register a handler function for a specific interrupt ID.  The vector table
* of the interrupt controller is updated, overwriting any previous handler.
* The handler function will be called when an interrupt occurs for the given
* interrupt ID.
*
* @param	BaseAddress is the CPU Interface Register base address of the
*		interrupt controller whose vector table will be modified.
* @param	InterruptID is the interrupt ID to be associated with the input
*		handler.
* @param	IntrHandler is the function pointer that will be added to
*		the vector table for the given interrupt ID.
* @param	CallBackRef is the argument that will be passed to the new
*		handler function when it is called. This is user-specific.
*
* @return	None.
*
* @note
*
* Note that this function has no effect if the input base address is invalid.
*
******************************************************************************/
void XScuGic_RegisterHandler(u32 BaseAddress, s32 InterruptID,
		Xil_InterruptHandler IntrHandler, void *CallBackRef)
{
	XScuGic_Config *CfgPtr;
	CfgPtr = LookupConfigByBaseAddress(BaseAddress);

	if (CfgPtr != NULL) {
		if (IntrHandler != NULL) {
			CfgPtr->HandlerTable[InterruptID].Handler =
					(Xil_InterruptHandler)IntrHandler;
		}
		if (CallBackRef != NULL) {
			CfgPtr->HandlerTable[InterruptID].CallBackRef =
				CallBackRef;
		}
	}
}

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the CPU interface base address of
* the device. A table contains the configuration info for each device in the
* system.
*
* @param	CpuBaseAddress is the CPU Interface Register base address.
*
* @return	A pointer to the configuration structure for the specified
*		device, or NULL if the device was not found.
*
* @note		None.
*
******************************************************************************/
static XScuGic_Config *LookupConfigByBaseAddress(u32 CpuBaseAddress)
{
	XScuGic_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < XPAR_SCUGIC_NUM_INSTANCES; Index++) {
		if (XScuGic_ConfigTable[Index].CpuBaseAddress ==
				CpuBaseAddress) {
			CfgPtr = &XScuGic_ConfigTable[Index];
			break;
		}
	}

	return (XScuGic_Config *)CfgPtr;
}

/****************************************************************************/
/**
* Sets the interrupt priority and trigger type for the specificd IRQ source.
*
* @param	DistBaseAddress is the distributor base address
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is the new priority for the IRQ source. 0 is highest
*			priority, 0xF8(248) is lowest. There are 32 priority
*			levels supported with a step of 8. Hence the supported
*			priorities are 0, 8, 16, 32, 40 ..., 248.
* @param	Trigger is the new trigger type for the IRQ source.
* Each bit pair describes the configuration for an INT_ID.
* SFI    Read Only    b10 always
* PPI    Read Only    depending on how the PPIs are configured.
*                    b01    Active HIGH level sensitive
*                    b11 Rising edge sensitive
* SPI                LSB is read only.
*                    b01    Active HIGH level sensitive
*                    b11 Rising edge sensitive/
*
* @return	None.
*
* @note		This API has the similar functionality of XScuGic_SetPriority
*	        TriggerType() and should be used when there is no InstancePtr.
*
*****************************************************************************/
void XScuGic_SetPriTrigTypeByDistAddr(u32 DistBaseAddress, u32 Int_Id,
					u8 Priority, u8 Trigger)
{
	u32 RegValue;
#if defined (GICv3)
	u32 Temp;
	u32 Index;
#endif
	u8 LocalPriority = Priority;

	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(Trigger <= XSCUGIC_INT_CFG_MASK);
	Xil_AssertVoid(LocalPriority <= XSCUGIC_MAX_INTR_PRIO_VAL);
#if defined (GICv3)
	if (Int_Id < XSCUGIC_SPI_INT_ID_START )
	{
		XScuGic_WriteReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_INT_PRIORITY_OFFSET_CALC(Int_Id),Priority);
		Temp = XScuGic_ReadReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_INT_CONFIG_OFFSET_CALC(Int_Id));
		Index = XScuGic_Get_Rdist_Int_Trigger_Index(Int_Id);
		Temp |= (Trigger << Index);
		XScuGic_WriteReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_INT_CONFIG_OFFSET_CALC(Int_Id),Temp);
		return;
	}
#endif

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	/*
	 * Determine the register to write to using the Int_Id.
	 */
	RegValue = XScuGic_ReadReg(DistBaseAddress,
			XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id));

	/*
	 * The priority bits are Bits 7 to 3 in GIC Priority Register. This
	 * means the number of priority levels supported are 32 and they are
	 * in steps of 8. The priorities can be 0, 8, 16, 32, 48, ... etc.
	 * The lower order 3 bits are masked before putting it in the register.
	 */
	LocalPriority = LocalPriority & XSCUGIC_INTR_PRIO_MASK;
	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue &= ~((u32)XSCUGIC_PRIORITY_MASK << ((Int_Id%4U)*8U));
	RegValue |= (u32)LocalPriority << ((Int_Id%4U)*8U);

	/*
	 * Write the value back to the register.
	 */
	XScuGic_WriteReg(DistBaseAddress, XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id),
					RegValue);
	/*
	 * Determine the register to write to using the Int_Id.
	 */
	RegValue = XScuGic_ReadReg(DistBaseAddress,
			XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id));

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue &= ~((u32)XSCUGIC_INT_CFG_MASK << ((Int_Id%16U)*2U));
	RegValue |= (u32)Trigger << ((Int_Id%16U)*2U);

	/*
	 * Write the value back to the register.
	 */
	XScuGic_WriteReg(DistBaseAddress, XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id),
				RegValue);

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/****************************************************************************/
/**
* Gets the interrupt priority and trigger type for the specificd IRQ source.
*
* @param	DistBaseAddress is the distributor  base address
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is a pointer to the value of the priority of the IRQ
*		source. This is a return value.
* @param	Trigger is pointer to the value of the trigger of the IRQ
*		source. This is a return value.
*
* @return	None.
*
* @note		This API has the similar functionality of XScuGic_GetPriority
*	        TriggerType() and should be used when there is no InstancePtr.
*
*****************************************************************************/
void XScuGic_GetPriTrigTypeByDistAddr(u32 DistBaseAddress, u32 Int_Id,
					u8 *Priority, u8 *Trigger)
{
	u32 RegValue;

	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(Priority != NULL);
	Xil_AssertVoid(Trigger != NULL);

	/*
	 * Determine the register to read to using the Int_Id.
	 */
	RegValue = XScuGic_ReadReg(DistBaseAddress,
	    XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id));

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue = RegValue >> ((Int_Id%4U)*8U);
	*Priority = (u8)(RegValue & XSCUGIC_PRIORITY_MASK);

	/*
	 * Determine the register to read to using the Int_Id.
	 */
	RegValue = XScuGic_ReadReg(DistBaseAddress,
	    XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id));

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue = RegValue >> ((Int_Id%16U)*2U);

	*Trigger = (u8)(RegValue & XSCUGIC_INT_CFG_MASK);
}

/****************************************************************************/
/**
* Sets the target CPU for the interrupt of a peripheral
*
* @param	DistBaseAddress is the device base address
* @param	Cpu_Id is a CPU number from which the interrupt has to be
*			unmapped
* @param	Int_Id is the IRQ source number to modify
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_InterruptMapFromCpuByDistAddr(u32 DistBaseAddress,
		u8 Cpu_Id, u32 Int_Id)
{
	u32 RegValue;
#if !defined (GICv3)
	u32 Offset;
	u8 Cpu_CoreId;
#endif

	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);

if (Int_Id >= XSCUGIC_SPI_INT_ID_START) {
#if defined (GICv3)
	u32 Temp;
	Temp = Int_Id - 32;
	RegValue = XScuGic_ReadReg(DistBaseAddress,
			XSCUGIC_IROUTER_OFFSET_CALC(Temp));
	RegValue |= (Cpu_Id);
	XScuGic_WriteReg(DistBaseAddress, XSCUGIC_IROUTER_OFFSET_CALC(Temp),
		RegValue);
#else
	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();
	RegValue = XScuGic_ReadReg(DistBaseAddress,
					XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));

	Offset = (Int_Id & 0x3U);
	Cpu_CoreId = (0x1U << Cpu_Id);

	RegValue |= (u32)(Cpu_CoreId) << (Offset*8U);

	XScuGic_WriteReg(DistBaseAddress,
					XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id),
					RegValue);
	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
#endif
}
}

/****************************************************************************/
/**
* Unmaps specific SPI interrupt from the target CPU
*
* @param	DistBaseAddress is the device base address
* @param	Cpu_Id is a CPU number from which the interrupt has to be
*			unmapped
* @param	Int_Id is the IRQ source number to modify
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_InterruptUnmapFromCpuByDistAddr(u32 DistBaseAddress,
			u8 Cpu_Id, u32 Int_Id)
{
	u32 RegValue;
#if !defined (GICv3)
	u32 Offset;
	u32 Cpu_CoreId;
#endif

	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);

if (Int_Id >= XSCUGIC_SPI_INT_ID_START) {
#if defined (GICv3)
	u32 Temp;
	Temp = Int_Id - 32;
	RegValue = XScuGic_ReadReg(DistBaseAddress,
			XSCUGIC_IROUTER_OFFSET_CALC(Temp));
	RegValue &= ~(Cpu_Id);
	XScuGic_WriteReg(DistBaseAddress, XSCUGIC_IROUTER_OFFSET_CALC(Temp),
		RegValue);
#else
    /*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	RegValue = XScuGic_ReadReg(DistBaseAddress,
				XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));

	Offset = (Int_Id & 0x3U);
	Cpu_CoreId = ((u32)0x1U << Cpu_Id);

	RegValue &= ~(Cpu_CoreId << (Offset*8U));
	XScuGic_WriteReg(DistBaseAddress,
			XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id), RegValue);

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
#endif
}
}

/****************************************************************************/
/**
* Unmaps all SPI interrupts from the target CPU
*
* @param	DistBaseAddress is the device base address
* @param	Cpu_Id is a CPU number from which the interrupts has to be
*			unmapped
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_UnmapAllInterruptsFromCpuByDistAddr(u32 DistBaseAddress,
			u8 Cpu_Id)
{
	u32 Int_Id;
	u32 Target_Cpu;
	u32 LocalCpuID = ((u32)1U << Cpu_Id);

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	LocalCpuID |= LocalCpuID << 8U;
	LocalCpuID |= LocalCpuID << 16U;

	for (Int_Id = 32U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+4U) {

		Target_Cpu = XScuGic_ReadReg(DistBaseAddress,
				XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));
		/* Remove LocalCpuID from interrupt target register */
		Target_Cpu &= (~LocalCpuID);
		XScuGic_WriteReg(DistBaseAddress,
			XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id), Target_Cpu);

	}

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/*****************************************************************************/
/**
*
* Enables the interrupt source provided as the argument Int_Id. Any pending
* interrupt condition for the specified Int_Id will occur after this function is
* called.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to XSCUGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XScuGic_EnableIntr (u32 DistBaseAddress, u32 Int_Id)
{
	u8 Cpu_Id = (u8)XScuGic_GetCpuID();
#if defined (GICv3)
	u32 Temp;
#endif
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);

#if defined (GICv3)
	if (Int_Id < XSCUGIC_SPI_INT_ID_START) {

		Int_Id &= 0x1f;
		Int_Id = 1 << Int_Id;

		Temp = XScuGic_ReadReg(DistBaseAddress +
			XSCUGIC_RDIST_SGI_PPI_OFFSET, XSCUGIC_RDIST_ISENABLE_OFFSET);
		Temp |= Int_Id;
		XScuGic_WriteReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_ISENABLE_OFFSET,Temp);
		return;
	}
#endif

	XScuGic_InterruptMapFromCpuByDistAddr(DistBaseAddress, Cpu_Id, Int_Id);
	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();
	XScuGic_WriteReg((DistBaseAddress), XSCUGIC_ENABLE_SET_OFFSET +
			(((Int_Id) / 32U) * 4U), ((u32)0x00000001U << ((Int_Id) % 32U)));
	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/*****************************************************************************/
/**
*
* Disables the interrupt source provided as the argument Int_Id such that the
* interrupt controller will not cause interrupts for the specified Int_Id. The
* interrupt controller will continue to hold an interrupt condition for the
* Int_Id, but will not cause an interrupt.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to XSCUGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XScuGic_DisableIntr (u32 DistBaseAddress, u32 Int_Id)
{
	u8 Cpu_Id = (u8)XScuGic_GetCpuID();
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);

#if defined (GICv3)
	u32 Temp;

	if (Int_Id < XSCUGIC_SPI_INT_ID_START) {

		Int_Id &= 0x1f;
		Int_Id = 1 << Int_Id;

		Temp = XScuGic_ReadReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_ISENABLE_OFFSET);
		Temp &= ~Int_Id;
		XScuGic_WriteReg(DistBaseAddress + XSCUGIC_RDIST_SGI_PPI_OFFSET,
			XSCUGIC_RDIST_ISENABLE_OFFSET,Temp);
		return;
	}
#endif
	XScuGic_InterruptUnmapFromCpuByDistAddr(DistBaseAddress, Cpu_Id, Int_Id);
	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();
	XScuGic_WriteReg((DistBaseAddress), XSCUGIC_DISABLE_OFFSET +
			(((Int_Id) / 32U) * 4U), ((u32)0x00000001U << ((Int_Id) % 32U)));
	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/** @} */
