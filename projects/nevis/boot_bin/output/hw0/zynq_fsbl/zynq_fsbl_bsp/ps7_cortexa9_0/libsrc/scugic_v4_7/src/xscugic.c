/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscugic.c
* @addtogroup scugic_v4_7
* @{
*
* Contains required functions for the XScuGic driver for the Interrupt
* Controller. See xscugic.h for a detailed description of the driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- --------------------------------------------------------
* 1.00a drg  01/19/10 First release
* 1.01a sdm  11/09/11 Changes are made in function XScuGic_CfgInitialize
*                     Since entry is now made as pointer in the XScuGic
*                     structure, necessary changes are made.
*                     The HandlerTable can now be populated through the low
*                     level routine XScuGic_RegisterHandler added in this
*                     release. Hence necessary checks are added not to
*                     overwrite the HandlerTable entries in function
*                     XScuGic_CfgInitialize.
* 1.03a srt  02/27/13 Added APIs
*                       -XScuGic_SetPriTrigTypeByDistAddr()
*                       -XScuGic_GetPriTrigTypeByDistAddr()
*                     Removed Offset calculation macros, defined in _hw.h
*                     (CR 702687)
*                     added support to direct interrupts  to the appropriate
*                     CPU Earlier interrupts were directed to CPU1
*                     (hard coded).Now depending upon the CPU selected by the
*                     user(xparameters.h), interrupts will be directed to
*                     the  relevant CPU. This fixes CR 699688.
*
* 1.04a hk   05/04/13 Assigned EffectiveAddr to CpuBaseAddress in
*                     XScuGic_CfgInitialize. Fix for CR#704400 to remove
*                     warnings.
*                     Moved functions XScuGic_SetPriTrigTypeByDistAddr and
*                    XScuGic_GetPriTrigTypeByDistAddr to xscugic_hw.c.
*                    This is fix for CR#705621.
* 1.06a asa  16/11/13 Fix for CR#749178. Assignment for EffectiveAddr
*                     in function XScuGic_CfgInitialize is removed as it was
*                     a bug.
* 3.00  kvn  02/13/14 Modified code for MISRA-C:2012 compliance.
* 3.01  pkp  06/19/15 Added XScuGic_InterruptMaptoCpu API for an interrupt
*                     target CPU mapping
* 3.02  pkp  11/09/15 Modified DistributorInit function for AMP case to add
*                     the current cpu to interrupt processor targets registers
* 3.2   asa  02/29/16 Modified DistributorInit function for Zynq AMP case. The
*                     distributor is left uninitialized for Zynq AMP. It is
*                     assumed that the distributor will be initialized by
*                     Linux master. However for CortexR5 case, the earlier code
*                     is left unchanged where the interrupt processor target
*                     registers in the distributor is initialized with the
*                     corresponding CPU ID on which the application built over
*                     the scugic driver runs. These changes fix CR#937243.
* 3.3   pkp  05/12/16 Modified XScuGic_InterruptMaptoCpu to write proper value
*                     to interrupt target register to fix CR#951848
*
* 3.4   asa  04/07/16 Created a new static function DoDistributorInit to
*                     simplify the flow and avoid code duplication. Changes are
*                     made for USE_AMP use case for R5. In a scenario
*                     (in R5 split mode) when one R5 is operating with A53 in
*                     open amp config and other R5 running baremetal app, the
*                     existing code had the potential to stop the whole AMP
*                     solution to work (if for some reason the R5 running the
*                     baremetal app tasked to initialize the Distributor hangs
*                     or crashes before initializing).Changes are made so that
*                     the R5 under AMP first checks if the distributor is
*                     enabled or not and if not, it does the standard
*                     Distributor initialization.This fixes the CR#952962.
* 3.4   mus  09/08/16 Added assert to avoid invalid access of GIC from CPUID 1
*                     for single core zynq-7000s
* 3.5   mus  10/05/16 Modified DistributorInit function to avoid
*                     re-initialization of distributor, If it is already
*                     initialized by other CPU.
* 3.5   pkp  10/17/16 Modified XScuGic_InterruptMaptoCpu to correct the CPU Id
*                     value and properly mask interrupt target processor value
*                     to modify interrupt target processor register for a given
*                     interrupt ID and cpu ID
* 3.6   pkp  20/01/17 Added new API XScuGic_Stop to Disable distributor and
*                     interrupts in case they are being used only by current
*                     cpu. It also removes current cpu from interrupt target
*                     registers for all interrupts.
*       kvn  02/17/17 Add support for changing GIC CPU master at run time.
*       kvn  02/28/17 Make the CpuId as static variable and Added new
*                     XScugiC_GetCpuId to access CpuId.
* 3.9   mus  02/21/18 Added new API's XScuGic_UnmapAllInterruptsFromCpu and
*                     XScuGic_InterruptUnmapFromCpu, These API's can be used
*                     by applications to unmap specific/all interrupts from
*                     target CPU. It fixes CR#992490.
* 3.10  mus  07/17/18 Updated file to fix the various coding style issues
*                     reported by checkpatch. It fixes CR#1006344.
* 3.10  aru  08/23/18 Resolved MISRA-C:2012 compliance mandatory violations
*                     It fixes CR#1007753.
* 3.10  mus  09/19/18 Fix cppcheck warnings
* 4.0   mus  11/22/18 Fixed bugs in software interrupt generation through
*                     XScuGic_SoftwareIntr API
* 4.1   asa  03/30/19 Made changes not to direct each interrupt to all
*                     available CPUs by default. This was breaking AMP
*                     behavior. Instead every time an interrupt enable
*                     request is received, the interrupt was mapped to
*                     the respective CPU. There were several other changes
*                     made to implement this. This set of changes was to
*                     fix CR-1024716.
* 4.1   mus  06/19/19 Added API's XScuGic_MarkCoreAsleep and
*                     XScuGic_MarkCoreAwake to mark processor core as
*                     asleep or awake. Fix for CR#1027220.
* 4.3   mus  10/14/20 Updated XScuGic_Stop for scenarios where USE_AMP flag
*                     is set to 1. Now, XScuGic_Stop would not disable
*                     distributor for USE_AMP=1, even when SPI interrupts
*                     are not being used by other CPU. This update is needed
*                     to support scenarios, where master CPU is using only
*                     SGI/PPI interrupts. It fixes CR#1079241.
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
* 4.6	sk   08/05/21 Fix scugic Misra-c violations.
* 4.7	sk   09/14/21 Fix gcc compiler warnings for A72 processor.
* 4.7	sk   10/13/21 Update APIs to perform interrupt mapping/unmapping only
* 		      when (Int_Id >= XSCUGIC_SPI_INT_ID_START).
* 4.7   dp   11/22/21 Added new API XScuGic_IsInitialized() to check and return
*                     the GIC initialization status.
* 4.7   mus  03/17/22 Fixed XScuGic_InterruptMaptoCpu and
*                     XScuGic_InterruptUnmapFromCpu for GICv3. It fixes
*                     CR#1126156.
* 4.7   asa  03/29/22 The API XScuGic_IsInitialized has a bug where if the
*                     Distributor Control Register, GICD_CTLR has both
*                     bit 0 and bit 1 set, the API wrongly returns 0,
*                     This is fixed with this change.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xil_types.h"
#include "xil_assert.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
static u32 CpuId = XPAR_CPU_ID; /**< CPU Core identifier */

/************************** Function Prototypes ******************************/

static void StubHandler(void *CallBackRef);

/*****************************************************************************/
/**
*
* DoDistributorInit initializes the distributor of the GIC. The
* initialization entails:
*
* - Write the trigger mode, priority
* - All interrupt sources are disabled
* - Enable the distributor
*
* @param	InstancePtr is a pointer to the XScuGic instance.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void DoDistributorInit(const XScuGic *InstancePtr)
{
	u32 Int_Id;

#if defined (GICv3)
	u32 Temp;

	Temp = XScuGic_DistReadReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET);
	Temp = (XSCUGIC500_DCTLR_ARE_NS_ENABLE | XSCUGIC500_DCTLR_ARE_S_ENABLE);
	Temp &= ~(XSCUGIC_EN_INT_MASK);
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET, Temp);
#else
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET, 0U);
#endif

	/*
	 * Set the security domains in the int_security registers for
	 * non-secure interrupts
	 * All are secure, so leave at the default. Set to 1 for non-secure
	 * interrupts.
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
		XScuGic_DistWriteReg(InstancePtr,
					XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id),
					0U);
	}


#define DEFAULT_PRIORITY    0xa0a0a0a0U
	for (Int_Id = 0U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+4U) {
		/*
		 * 2. The priority using int the priority_level register
		 * The priority_level and spi_target registers use one byte per
		 * INT_ID.
		 * Write a default value that can be changed elsewhere.
		 */
		XScuGic_DistWriteReg(InstancePtr,
					XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id),
					DEFAULT_PRIORITY);
	}

#if defined (GICv3)
	for (Int_Id = 0U; Int_Id<XSCUGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+32U) {

		XScuGic_DistWriteReg(InstancePtr,
				XSCUGIC_SECURITY_TARGET_OFFSET_CALC(Int_Id),
				XSCUGIC_DEFAULT_SECURITY);
	}
	/*
	 * Set security for SGI/PPI
	 *
	 */
	XScuGic_ReDistSGIPPIWriteReg(InstancePtr,XSCUGIC_RDIST_IGROUPR_OFFSET,
									XSCUGIC_DEFAULT_SECURITY);
#endif
	for (Int_Id = 0U; Int_Id<XSCUGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+32U) {
		/*
		 * 4. Enable the SPI using the enable_set register. Leave all
		 * disabled for now.
		 */
		XScuGic_DistWriteReg(InstancePtr,
		XSCUGIC_EN_DIS_OFFSET_CALC(XSCUGIC_DISABLE_OFFSET, Int_Id),
			0xFFFFFFFFU);

	}
#if defined (GICv3)
	Temp = XScuGic_DistReadReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET);
	Temp |= XSCUGIC_EN_INT_MASK;
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET, Temp);
	XScuGic_Enable_Group1_Interrupts();
	XScuGic_Enable_Group0_Interrupts();
#else
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET,
					XSCUGIC_EN_INT_MASK);
#endif
}

/*****************************************************************************/
/**
*
* DistributorInit initializes the distributor of the GIC. It calls
* DoDistributorInit to finish the initialization.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void DistributorInit(XScuGic *InstancePtr)
{
	u32 RegValue;

#if (defined (USE_AMP) && (USE_AMP==1)) && (defined (ARMA9) || defined(__aarch64__))
#warning "Building GIC for AMP"
	/*
	 * GIC initialization is taken care by master CPU in
	 * openamp configuration, so do nothing and return.
	 */
	return;
#endif

	Xil_AssertVoid(InstancePtr != NULL);
	RegValue = XScuGic_DistReadReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET);
	if ((RegValue & XSCUGIC_EN_INT_MASK) == 0U) {
		DoDistributorInit(InstancePtr);
		return;
	}
}

#if !defined (GICv3)
/*****************************************************************************/
/**
*
* CPUInitialize initializes the CPU Interface of the GIC. The initialization
* entails:
*
*	- Set the priority of the CPU
*	- Enable the CPU interface
*
* @param	InstancePtr is a pointer to the XScuGic instance.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void CPUInitialize(const XScuGic *InstancePtr)
{
	/*
	 * Program the priority mask of the CPU using the Priority mask register
	 */
	XScuGic_CPUWriteReg(InstancePtr, XSCUGIC_CPU_PRIOR_OFFSET, 0xF0U);


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
	 * 2. Set EnableS=1, to enable the CPU interface to signal secure
	 *  interrupts. Only enable the IRQ output unless secure interrupts
	 * are needed.
	 */
	XScuGic_CPUWriteReg(InstancePtr, XSCUGIC_CONTROL_OFFSET, 0x07U);

}
#endif

/*****************************************************************************/
/**
*
* CfgInitialize a specific interrupt controller instance/driver. The
* initialization entails:
*
* - Initialize fields of the XScuGic structure
* - Initial vector table with stub function calls
* - All interrupt sources are disabled
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	ConfigPtr is a pointer to a config table for the particular
*		device this driver is associated with.
* @param	EffectiveAddr is the device base address in the virtual memory
*		address space. The caller is responsible for keeping the address
*		mapping from EffectiveAddr to the device physical base address
*		unchanged once this function is invoked. Unexpected errors may
*		occur if the address mapping changes after this function is
*		called. If address translation is not used, use
*		Config->BaseAddress for this parameters, passing the physical
*		address instead.
*
* @return
*		- XST_SUCCESS if initialization was successful
*
* @note		None.
*
******************************************************************************/
s32  XScuGic_CfgInitialize(XScuGic *InstancePtr,
				XScuGic_Config *ConfigPtr,
				u32 EffectiveAddr)
{
	u32 Int_Id;
	(void) EffectiveAddr;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);
	/*
	 * Check Zynq-7000 base silicon configuration.
	 * If it is single CPU configuration then invoke assert for CPU ID 1.
	 */
#ifdef ARMA9
	if (XPAR_CPU_ID == 0x01) {
		Xil_AssertNonvoid((Xil_In32(XPS_EFUSE_BASEADDR
			+ EFUSE_STATUS_OFFSET) & EFUSE_STATUS_CPU_MASK) == 0);
	}
#endif

	if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY) {

		InstancePtr->IsReady = 0U;
		InstancePtr->Config = ConfigPtr;


		for (Int_Id = 0U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
				Int_Id++) {
			/*
			* Initialize the handler to point to a stub to handle an
			* interrupt which has not been connected to a handler
			* Only initialize it if the handler is 0 which means it
			* was not initialized statically by the tools/user. Set
			* the callback reference to this instance so that
			* unhandled interrupts can be tracked.
			*/
			if ((InstancePtr->Config->HandlerTable[Int_Id].Handler
					== (Xil_InterruptHandler)NULL)) {
				InstancePtr->Config->HandlerTable[Int_Id].Handler
						= (Xil_InterruptHandler)StubHandler;
			}
			InstancePtr->Config->HandlerTable[Int_Id].CallBackRef =
								InstancePtr;
		}
#if defined (GICv3)
	u32 Waker_State;
	Waker_State = XScuGic_ReDistReadReg(InstancePtr,XSCUGIC_RDIST_WAKER_OFFSET);
	XScuGic_ReDistWriteReg(InstancePtr,XSCUGIC_RDIST_WAKER_OFFSET,
							Waker_State & (~ XSCUGIC_RDIST_WAKER_LOW_POWER_STATE_MASK));
		/* Enable system reg interface through ICC_SRE_EL1 */
		#if EL3
			XScuGic_Enable_SystemReg_CPU_Interface_EL3();
		#endif
			XScuGic_Enable_SystemReg_CPU_Interface_EL1();
		isb();
#endif
		XScuGic_Stop(InstancePtr);
		DistributorInit(InstancePtr);
#if defined (GICv3)
		XScuGic_set_priority_filter(0xff);
#else
		CPUInitialize(InstancePtr);
#endif
		InstancePtr->IsReady = XIL_COMPONENT_IS_READY;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Makes the connection between the Int_Id of the interrupt source and the
* associated handler that is to run when the interrupt is recognized. The
* argument provided in this call as the Callbackref is used as the argument
* for the handler when it is called.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to XSCUGIC_MAX_NUM_INTR_INPUTS - 1
* @param	Handler to the handler for that interrupt.
* @param	CallBackRef is the callback reference, usually the instance
*		pointer of the connecting driver.
*
* @return
*
*		- XST_SUCCESS if the handler was connected correctly.
*
* @note
*
* WARNING: The handler provided as an argument will overwrite any handler
* that was previously connected.
*
****************************************************************************/
s32  XScuGic_Connect(XScuGic *InstancePtr, u32 Int_Id,
				Xil_InterruptHandler Handler, void *CallBackRef)
{
	/*
	 * Assert the arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertNonvoid(Handler != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * The Int_Id is used as an index into the table to select the proper
	 * handler
	 */
	InstancePtr->Config->HandlerTable[Int_Id].Handler = (Xil_InterruptHandler)Handler;
	InstancePtr->Config->HandlerTable[Int_Id].CallBackRef = CallBackRef;

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Updates the interrupt table with the Null Handler and NULL arguments at the
* location pointed at by the Int_Id. This effectively disconnects that interrupt
* source from any handler. The interrupt is disabled also.
*
* @param	InstancePtr is a pointer to the XScuGic instance to be worked on.
* @param	Int_Id contains the ID of the interrupt source and should
*		be in the range of 0 to XSCUGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XScuGic_Disconnect(XScuGic *InstancePtr, u32 Int_Id)
{
	u32 Mask;

	/*
	 * Assert the arguments
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position. Int_Id currently limited to 0 - 31
	 */
	Mask = (u32)0x00000001U << (Int_Id % 32U);

	/*
	 * Disable the interrupt such that it won't occur while disconnecting
	 * the handler, only disable the specified interrupt id without
	 * modifying the other interrupt ids
	 */
	XScuGic_DistWriteReg(InstancePtr, (u32)XSCUGIC_DISABLE_OFFSET +
						((Int_Id / 32U) * 4U), Mask);

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
	/*
	 * Disconnect the handler and connect a stub, the callback reference
	 * must be set to this instance to allow unhandled interrupts to be
	 * tracked
	 */
	InstancePtr->Config->HandlerTable[Int_Id].Handler = (Xil_InterruptHandler)StubHandler;
	InstancePtr->Config->HandlerTable[Int_Id].CallBackRef = InstancePtr;
}

/*****************************************************************************/
/**
*
* Enables the interrupt source provided as the argument Int_Id. Any pending
* interrupt condition for the specified Int_Id will occur after this function is
* called.
* This API also maps the interrupt to the requesting CPU.
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
void XScuGic_Enable(XScuGic *InstancePtr, u32 Int_Id)
{
	u32 Mask;
	u8 Cpu_Identifier = (u8)CpuId;

#if defined (GICv3)
	u32 Temp;
#endif
	/*
	 * Assert the arguments
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

#if defined (GICv3)
	if (Int_Id < XSCUGIC_SPI_INT_ID_START) {

		Int_Id &= 0x1f;
		Int_Id = 1 << Int_Id;

		Temp = XScuGic_ReDistSGIPPIReadReg(InstancePtr,XSCUGIC_RDIST_ISENABLE_OFFSET);
		Temp |= Int_Id;
		XScuGic_ReDistSGIPPIWriteReg(InstancePtr,XSCUGIC_RDIST_ISENABLE_OFFSET,Temp);
		return;
	}
#endif
	XScuGic_InterruptMaptoCpu(InstancePtr, Cpu_Identifier, Int_Id);
	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();
	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position.
	 */
	Mask = (u32)0x00000001U << (Int_Id % 32U);
	/*
	 * Enable the selected interrupt source by setting the
	 * corresponding bit in the Enable Set register.
	 */
	XScuGic_DistWriteReg(InstancePtr, (u32)XSCUGIC_ENABLE_SET_OFFSET +
				((Int_Id / 32U) * 4U), Mask);

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
* This API also unmaps the interrupt for the requesting CPU.
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
void XScuGic_Disable(XScuGic *InstancePtr, u32 Int_Id)
{
	u32 Mask;
	u8 Cpu_Identifier = (u8)CpuId;
#if defined (GICv3)
	u32 Temp;
#endif

	/*
	 * Assert the arguments
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

#if defined (GICv3)
	if (Int_Id < XSCUGIC_SPI_INT_ID_START) {

		Int_Id &= 0x1f;
		Int_Id = 1 << Int_Id;

		Temp = XScuGic_ReDistSGIPPIReadReg(InstancePtr,XSCUGIC_RDIST_ISENABLE_OFFSET);
		Temp &= ~Int_Id;
		XScuGic_ReDistSGIPPIWriteReg(InstancePtr,XSCUGIC_RDIST_ISENABLE_OFFSET,Temp);
		return;
	}
#endif
	XScuGic_InterruptUnmapFromCpu(InstancePtr, Cpu_Identifier, Int_Id);

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();
	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position. Int_Id currently limited to 0 - 31
	 */
	Mask = (u32)0x00000001U << (Int_Id % 32U);

	/*
	 * Disable the selected interrupt source by setting the
	 * corresponding bit in the IDR.
	 */
	XScuGic_DistWriteReg(InstancePtr, (u32)XSCUGIC_DISABLE_OFFSET +
					((Int_Id / 32U) * 4U), Mask);

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/*****************************************************************************/
/**
*
* Allows software to simulate an interrupt in the interrupt controller.  This
* function will only be successful when the interrupt controller has been
* started in simulation mode.  A simulated interrupt allows the interrupt
* controller to be tested without any device to drive an interrupt input
* signal into it.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	Int_Id is the software interrupt ID to simulate an interrupt.
* @param	Cpu_Identifier is the list of CPUs to send the interrupt.
*
* @return
*
* XST_SUCCESS if successful, or XST_FAILURE if the interrupt could not be
* simulated
*
* @note		None.
*
******************************************************************************/
s32  XScuGic_SoftwareIntr(XScuGic *InstancePtr, u32 Int_Id, u32 Cpu_Identifier)
{
	u32 Mask;

	/*
	 * Assert the arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Int_Id <= 15U);
	Xil_AssertNonvoid(Cpu_Identifier <= 255U);

#if defined (GICv3)
	Mask = (Cpu_Identifier | (Int_Id << XSCUGIC_SGIR_EL1_INITID_SHIFT));
#if EL3
	XScuGic_WriteICC_SGI0R_EL1(Mask);
#else
	XScuGic_WriteICC_SGI1R_EL1(Mask);
#endif
#else

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired interrupt. Int_Id currently limited to 0 - 15
	 * Use the target list for the Cpu ID.
	 */
	Mask = ((Cpu_Identifier << 16U) | Int_Id) &
		(XSCUGIC_SFI_TRIG_CPU_MASK | XSCUGIC_SFI_TRIG_INTID_MASK);

	/*
	 * Write to the Software interrupt trigger register. Use the appropriate
	 * CPU Int_Id.
	 */
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_SFI_TRIG_OFFSET, Mask);

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();

	/* Indicate the interrupt was successfully simulated */
#endif
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* A stub for the asynchronous callback. The stub is here in case the upper
* layers forget to set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubHandler(void *CallBackRef)
{
	/*
	 * verify that the inputs are valid
	 */
	Xil_AssertVoid(CallBackRef != NULL);

	/*
	 * Indicate another unhandled interrupt for stats
	 */
	((XScuGic *)((void *)CallBackRef))->UnhandledInterrupts++;
}

/****************************************************************************/
/**
* Sets the interrupt priority and trigger type for the specificd IRQ source.
*
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is the new priority for the IRQ source. 0 is highest
*           priority, 0xF8(248) is lowest. There are 32 priority levels
*           supported with a step of 8. Hence the supported priorities are
*           0, 8, 16, 32, 40 ..., 248.
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
* @note		None.
*
*****************************************************************************/
void XScuGic_SetPriorityTriggerType(XScuGic *InstancePtr, u32 Int_Id,
					u8 Priority, u8 Trigger)
{
	u32 RegValue;
#if defined (GICv3)
	u32 Temp;
	u32 Index;
#endif
	u8 LocalPriority;
	LocalPriority = Priority;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(Trigger <= (u8)XSCUGIC_INT_CFG_MASK);
	Xil_AssertVoid(LocalPriority <= (u8)XSCUGIC_MAX_INTR_PRIO_VAL);
#if defined (GICv3)
	if (Int_Id < XSCUGIC_SPI_INT_ID_START )
	{
		XScuGic_ReDistSGIPPIWriteReg(InstancePtr,XSCUGIC_RDIST_INT_PRIORITY_OFFSET_CALC(Int_Id),Priority);
		Temp = XScuGic_ReDistSGIPPIReadReg(InstancePtr,XSCUGIC_RDIST_INT_CONFIG_OFFSET_CALC(Int_Id));
		Index = XScuGic_Get_Rdist_Int_Trigger_Index(Int_Id);
		Temp |= (Trigger << Index);
		XScuGic_ReDistSGIPPIWriteReg(InstancePtr,XSCUGIC_RDIST_INT_CONFIG_OFFSET_CALC(Int_Id),Temp);
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
	RegValue = XScuGic_DistReadReg(InstancePtr,
			XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id));

	/*
	 * The priority bits are Bits 7 to 3 in GIC Priority Register. This
	 * means the number of priority levels supported are 32 and they are
	 * in steps of 8. The priorities can be 0, 8, 16, 32, 48, ... etc.
	 * The lower order 3 bits are masked before putting it in the register.
	 */
	LocalPriority = LocalPriority & (u8)XSCUGIC_INTR_PRIO_MASK;
	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue &= ~((u32)XSCUGIC_PRIORITY_MASK << ((Int_Id%4U)*8U));
	RegValue |= (u32)LocalPriority << ((Int_Id%4U)*8U);

	/*
	 * Write the value back to the register.
	 */
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_PRIORITY_OFFSET_CALC(Int_Id),
				RegValue);

	/*
	 * Determine the register to write to using the Int_Id.
	 */
	RegValue = XScuGic_DistReadReg(InstancePtr,
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
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_INT_CFG_OFFSET_CALC(Int_Id),
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
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is a pointer to the value of the priority of the IRQ
*		source. This is a return value.
* @param	Trigger is pointer to the value of the trigger of the IRQ
*		source. This is a return value.
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_GetPriorityTriggerType(XScuGic *InstancePtr, u32 Int_Id,
					u8 *Priority, u8 *Trigger)
{
	u32 RegValue;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS);
	Xil_AssertVoid(Priority != NULL);
	Xil_AssertVoid(Trigger != NULL);

	/*
	 * Determine the register to read to using the Int_Id.
	 */
	RegValue = XScuGic_DistReadReg(InstancePtr,
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
	RegValue = XScuGic_DistReadReg(InstancePtr,
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
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Cpu_Identifier is a CPU number for which the interrupt has to be targeted
* @param	Int_Id is the IRQ source number to modify
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_InterruptMaptoCpu(XScuGic *InstancePtr, u8 Cpu_Identifier, u32 Int_Id)
{
	u32 RegValue;

if (Int_Id >= XSCUGIC_SPI_INT_ID_START) {
#if defined (GICv3)
	Xil_AssertVoid(InstancePtr != NULL);
	RegValue = XScuGic_DistReadReg(InstancePtr,
			XSCUGIC_IROUTER_OFFSET_CALC(Int_Id));
	RegValue |= Cpu_Identifier;
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_IROUTER_OFFSET_CALC(Int_Id),
					  RegValue);
#else
	u8 Cpu_CoreId;
	u32 Offset;
	Xil_AssertVoid(InstancePtr != NULL);

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	RegValue = XScuGic_DistReadReg(InstancePtr,
			XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));

	Offset = (Int_Id & 0x3U);
	Cpu_CoreId = (0x1U << Cpu_Identifier);

	RegValue |= (u32)(Cpu_CoreId) << (Offset*8U);
	XScuGic_DistWriteReg(InstancePtr,
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
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Cpu_Identifier is a CPU number from which the interrupt has to be
*			unmapped
* @param	Int_Id is the IRQ source number to modify
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_InterruptUnmapFromCpu(XScuGic *InstancePtr, u8 Cpu_Identifier, u32 Int_Id)
{
	u32 RegValue;

if (Int_Id >= XSCUGIC_SPI_INT_ID_START) {
#if defined (GICv3)
	Xil_AssertVoid(InstancePtr != NULL);
	RegValue = XScuGic_DistReadReg(InstancePtr,
			XSCUGIC_IROUTER_OFFSET_CALC(Int_Id));
	RegValue &= ~Cpu_Identifier;
	XScuGic_DistWriteReg(InstancePtr, XSCUGIC_IROUTER_OFFSET_CALC(Int_Id),
						  RegValue);
#else
	u32 Cpu_CoreId;
	u32 Offset;
	Xil_AssertVoid(InstancePtr != NULL);

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	RegValue = XScuGic_DistReadReg(InstancePtr,
				XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));

	Offset = (Int_Id & 0x3U);
	Cpu_CoreId = ((u32)0x1U << Cpu_Identifier);

	RegValue &= ~(Cpu_CoreId << (Offset*8U));
	XScuGic_DistWriteReg(InstancePtr,
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
* Unmaps all SPI interrupts from the target CPU
*
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Cpu_Identifier is a CPU number from which the interrupts has to be
*			unmapped
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_UnmapAllInterruptsFromCpu(XScuGic *InstancePtr, u8 Cpu_Identifier)
{
	u32 Int_Id;
	u32 Target_Cpu;
	u32 LocalCpuID = ((u32)1U << Cpu_Identifier);

	Xil_AssertVoid(InstancePtr != NULL);

    /*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	LocalCpuID |= LocalCpuID << 8U;
	LocalCpuID |= LocalCpuID << 16U;

	for (Int_Id = 32U; Int_Id  < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+4U) {

		Target_Cpu = XScuGic_DistReadReg(InstancePtr,
				XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));
		/* Remove LocalCpuID from interrupt target register */
		Target_Cpu &= (~LocalCpuID);
		XScuGic_DistWriteReg(InstancePtr,
			XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id), Target_Cpu);

	}

	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}
/****************************************************************************/
/**
* It checks if the interrupt target register contains all interrupts to be
* targeted for current CPU. If they are programmed to be forwarded to current
* cpu, this API disable all interrupts and disable GIC distributor.
* This API also removes current CPU from interrupt target registers for all
* interrupt.
*
* @param	InstancePtr is a pointer to the instance to be worked on.
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_Stop(XScuGic *InstancePtr)
{
	u32 Int_Id;
	u32 RegValue;
	u32 Target_Cpu;
	#if (defined (USE_AMP) && (USE_AMP==1))
	u32 DistDisable = 0; /* Do not disable distributor */
	#else
	u32 DistDisable = 1; /* Track distributor status*/
	#endif
	u32 LocalCpuID = ((u32)0x1 << CpuId);

	Xil_AssertVoid(InstancePtr != NULL);

	/* If distributor is already disabled, no need to do anything */
	RegValue = XScuGic_DistReadReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET);
	if ((RegValue & XSCUGIC_EN_INT_MASK) == 0U) {
		return;
	}

	/*
	 * Call spinlock to protect multiple applications running at separate
	 * CPUs to write to the same register. This macro also ensures that
	 * the spinlock mechanism is used only if spinlock is enabled by
	 * user.
	 */
	XIL_SPINLOCK();

	LocalCpuID |= LocalCpuID << 8U;
	LocalCpuID |= LocalCpuID << 16U;

	/*
	 * Check if the interrupt are targeted to current cpu only or not.
	 * Also remove current cpu from interrupt target register for all
	 * interrupts.
	 */
	for (Int_Id = 32U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
			Int_Id = Int_Id+4U) {

		Target_Cpu = XScuGic_DistReadReg(InstancePtr,
					XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id));
		if ((Target_Cpu != LocalCpuID) && (Target_Cpu != (u32)0)) {
			/*
			 * If any other CPU is also programmed to target
			 * register, GIC distributor can not be disabled.
			 */
			DistDisable = 0;
		}

		/* Remove current CPU from interrupt target register */
		Target_Cpu &= (~LocalCpuID);
		XScuGic_DistWriteReg(InstancePtr,
			XSCUGIC_SPI_TARGET_OFFSET_CALC(Int_Id), Target_Cpu);

	}

	/*
	 * If GIC distributor is safe to be disabled, disable all the interrupt
	 * and then disable distributor.
	 */
	if (DistDisable == (u32)1) {
		for (Int_Id = 0U; Int_Id < XSCUGIC_MAX_NUM_INTR_INPUTS;
				Int_Id = Int_Id+32U) {
			/*
			 * Disable all the interrupts
			 */
			XScuGic_DistWriteReg(InstancePtr,
			  XSCUGIC_EN_DIS_OFFSET_CALC(XSCUGIC_DISABLE_OFFSET,
							Int_Id),
			0xFFFFFFFFU);
		}
		XScuGic_DistWriteReg(InstancePtr, XSCUGIC_DIST_EN_OFFSET, 0U);
	}
	/*
	 * Release the lock previously taken. This macro ensures that the lock
	 * is given only if spinlock mechanism is enabled by the user.
	 */
	XIL_SPINUNLOCK();
}

/****************************************************************************/
/**
* This updates the CpuId global variable.
*
* @param	CpuCoreId is the CPU core number.
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XScuGic_SetCpuID(u32 CpuCoreId)
{
	Xil_AssertVoid(CpuCoreId <= 1U);

	CpuId = CpuCoreId;
}

/****************************************************************************/
/**
* This function returns the CpuId variable.
*
* @return	The CPU core number.
*
* @note        None.
*
*****************************************************************************/
u32 XScuGic_GetCpuID(void)
{
	return CpuId;
}

/****************************************************************************/
/**
* It checks whether the XScGic is initialized or not given the device id.
*
* @param	DeviceId the XScuGic device.
*
* @return	Returns 1 if initialized otherwise 0.
*
* @note		None
*
*****************************************************************************/
u8 XScuGic_IsInitialized(u32 DeviceId)
{
	u8 Device_Initilaized = 0U;
	XScuGic_Config *CfgPtr;
	u32 RegVal;

	CfgPtr = XScuGic_LookupConfig(DeviceId);
	if (CfgPtr != NULL) {
		RegVal = XScuGic_ReadReg(CfgPtr->DistBaseAddress, XSCUGIC_DIST_EN_OFFSET);
		if (RegVal & XSCUGIC_EN_INT_MASK) {
			Device_Initilaized = 1U;
		}
	}

	return Device_Initilaized;
}
#if defined (GICv3)
/****************************************************************************/
/**
* It marks processor core which calls this API as asleep
*
* @return	None.
*
* @note 	It should be called before suspending processor core. Once this
* 			API is invoked, pending interrupts for processor core asserts
* 			WakeRequest, to indicate that the PE is to have its power
* 			restored  In case of Versal SoC, WakeRequest will be consumed by
* 			psv_psm processor and psmfw will wake up APU processor core.
*
*****************************************************************************/
void XScuGic_MarkCoreAsleep(XScuGic *InstancePtr)
{
	u32 Waker_State;

	Waker_State = XScuGic_ReDistReadReg(InstancePtr,XSCUGIC_RDIST_WAKER_OFFSET);
	XScuGic_ReDistWriteReg(InstancePtr,XSCUGIC_RDIST_WAKER_OFFSET,
							Waker_State |
							XSCUGIC_RDIST_WAKER_LOW_POWER_STATE_MASK);
}

/****************************************************************************/
/**
* It marks processor core which calls this API as awake
*
* @return	None.
*
* @note 	None
*
*****************************************************************************/
void XScuGic_MarkCoreAwake(XScuGic *InstancePtr)
{
	u32 Waker_State;

	Waker_State = XScuGic_ReDistReadReg(InstancePtr,
			XSCUGIC_RDIST_WAKER_OFFSET);
	XScuGic_ReDistWriteReg(InstancePtr,XSCUGIC_RDIST_WAKER_OFFSET,
							Waker_State &
							(~ XSCUGIC_RDIST_WAKER_LOW_POWER_STATE_MASK));
}
#endif
/** @} */
