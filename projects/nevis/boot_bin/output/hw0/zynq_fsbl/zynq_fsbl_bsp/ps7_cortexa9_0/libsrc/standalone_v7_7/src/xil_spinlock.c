/******************************************************************************
* Copyright (c) 2021 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_spinlock.c
*
* Implements a spinlocking mechanism using ARM load-exclusive and
* store-exclusive instructions.
*
* spinlocks in baremetal world are useful mainly for AMP kind of use cases
* where different applications run in different CPUs in a CPU cluster and
* they have a common resource to work with.
* A simple yet common example is:
* R5-0 and R5-1 have separate applications and at runtime they try to access a
* shared register space (GIC) or shared memory space. Mutual exclusion is
* really needed for such use cases.
* Similar use case applies for Zynq CortexA9-0 and CortesA9-1.
*
* The spinlock mechanism provided with this file is very simple to cater to
* baremetal world requirements.
*
* A) Unlike OS type of use cases, at any point of time, only a single lock
*    can be used. There is no way in BM world we can support multiple locks
*    at the same time.
* B) The spinlocking is available for ARM v7 (Cortex-R5 and Cortex-A9).
* C) Users need to provide a lock (essentially a shared address), and a flag
*    (also a shared address) for spinlocking to work. These shared addresses
*    must be agreed upon by apps running on both CPUs. Needless to say,
*    the linker scripts must change accordingly so that apps running on both
*    CPUs can have a shared DDR region from which address can be used for
*    spinlocking.
* D) The address that is used for spinlocking and the address that is used
*    as flag address must be in a memory region that is strongly-ordered
*    or device memory.
* E) Like any similar standard use cases, one application running in any
*    of the CPUs must create the spinlock address. It is advisable that the
*    the same application must destroy or release the spinlock address,
*    though nothing stops the other CPU in destroying and releasing the
*    spinlock addresses. There has to be understanding between applications
*    running on both the CPUs to ensure that correct ordering is followed.
*    Once a spinlock is released, it spinlock APIs cannot be used anymore.
* F) Once a spinlock address is created it can be used any number of times
*    to protect critical sections as long as a certain set of rules are
*    followed and a certain set of sequences are followed. More about the
*    sequences later in this description.
* G) To re-iterate, the spinlocking mechanism provided through this file is
*    pretty rudimentary. Users can always improvise. The use case that
*    the whole mechanism is trying to solve is: common register space
*    being accesses in an AMP scenario by independent applications running
*    at multiple CPUs.
*
* The usage of APIs provided in this file are summarized below.
* A) Applications running at both the CPUs ensure that they allocate shared
*    memory space to be used for locking and flag maintenance.
* B) The memory allocated as shared should be of minimum size 1 MB. This
*    limitation is because the way translation tables/memory map is setup
*    for R5 or A9. In future, this limitation can be brought down 4 KB.
* C) The spinlock and the flag address must be uncached. Users need to c
*    all Xilinx provided API Xil_SetTlbAttributes to mark the shared
*    1 MB memory space as strongly ordered or device memory.
* D) Both the applications must call the API Xil_InitializeSpinLock with
*    the spinlock address, flag address and flag value to be used. Currently
*    only one flag value is supported (i.e. XIL_SPINLOCK_ENABLE with
*    a value of 1).
*    Only if the Xil_InitializeSpinLock returns success, should they
*    proceed to use Xil_SpinLock and Xil_SpinUnlock.
* F) Afterwards applications can start using the APIs Xil_SpinLock() and
*    Xil_SpinUnlock() to protect the critical sections.
* G) It is highly suggested to use Xil_IsSpinLockEnabled() as a check
*    before using Xil_SpinLock() or Xil_SpinUnlock(). Refer to the code
*    snippet below.
* H) If spinlock is no more needed by an application, they can use the API
*    Xil_ReleaseSpinLock().
* I) Once a spinlock is created it can be used as many numbers of time as
*    needed.
*
* A typical code snippet with R5 as an example and lock address falling
* in the 1 MB memory region (0x300000 to 0x400000) is given below.
*
* Code snippet for R5-0 (that runs first):
* int main()
* {
*   u32 *sharedlockaddr = (int *)0x300008;
*   u32 *sharedlockflagaddr = int *)0x30000C;
*   Xil_SetTlbAttributes(0x300000,STRONG_ORDERD_SHARED | PRIV_RW_USER_RW);
*	Xil_InitializeSpinLock((UINTPTR)sharedlockaddr,(UINTPTR)sharedlockflagaddr,
*                                 XIL_SPINLOCK_ENABLE);
*   ....
*   ....
*   if (Xil_IsSpinLockEnabled())
*	    Xil_SpinLock();
*   // Critical Section
*   ....
*   ....
*   if (Xil_IsSpinLockEnabled())
*      Xil_SpinUnlock();
*   .....
*   .....
*   Xil_ReleaseSpinLock();
* } // End of main
*
* * Code snippet for R5-1 (that runs second):
* int main()
* {
*   u32 *sharedlockaddr = (int *)0x300008;
*   u32 *sharedlockflagaddr = int *)0x30000C;
*   Xil_SetTlbAttributes(0x300000,STRONG_ORDERD_SHARED | PRIV_RW_USER_RW);
*	Xil_InitializeSpinLock((UINTPTR)sharedlockaddr,(UINTPTR)sharedlockflagaddr,
*                                 XIL_SPINLOCK_ENABLE);
*   ....
*   if (Xil_IsSpinLockEnabled())
*	    Xil_SpinLock();
*   // Critical Section
*   ....
*   ....
*   if (Xil_IsSpinLockEnabled())
*      Xil_SpinUnlock();
*   .....
*   .....
*   Xil_ReleaseSpinLock();
* } // End of main
*
* Xil_SpinLock() and Xil_SpinLock() return XST_FAILURE if the spinlock
* is initialized. Instead of calling Xil_IsSpinLockEnabled before using
* Xil_SpinLock() or Xil_SpinLock(), users can also check the return
* value of Xil_SpinLock and Xil_SpinLock. If a lock is not initialized,
* Xil_SpinLock will just return failure and not perform load-exclusive
* operations.
*
* Guidelines on when to use the spinlocking mechanism in Xilinx baremetal
* environment:
* A) Spinlocking mechanism can only be used for Cortex-R5s (split mode)
*    and Cortex-A9s (Zynq).
* B) In Cortex-R5 split mode or Cortex-A9 cluster, applications running on
*    both the CPUs, by design should never be sharing the same peripheral.
*    It is impossible to support such use cases reliably.
*    Hence none of the peripheral drivers (except GIC driver) have
*    spinlocking support added into their APIs.
* C) GIC register space is shared by both the CPUs (R5 split mode, A9s).
*    The GIC driver APIs have spinlocking mechanism added to the relevant
*    APIs. Users need to follow the guidelines stated earlier to enable
*    the spinlocks and use them.
*    It is really advisable to use spinlock mechanism for use cases where
*    applications running on multiple CPUs can simultaneously access
*    GIC register space through driver APIs.
* D) The spinlock mechanism can be used to protect any critical region
*    implemented through shared memory space.
*
* IMPORTANT NOTE: Circular spinlocks are not allowed (as expected).
* Use case where an application calls Xil_SpinLock back to back without
* calling Xil_SpinUnlock in between will/may result in a deadlock
* condition.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 7.5 	asa      02/23/21 First release
* 7.5   asa      04/28/21 Fixed bug Xil_IsSpinLockEnabled to avoid
*                         dereferencing to address zero.
* 7.7	sk	 01/10/22 Update values from signed to unsigned to fix
* 			  misra_c_2012_rule_10_4 violation.
* </pre>
*
******************************************************************************/


/***************************** Include Files ********************************/
#if !defined (__aarch64__) && defined(__GNUC__) && !defined(__clang__)
#include "xil_spinlock.h"


/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/



/************************** Variable Definitions ****************************/
static UINTPTR Xil_Spinlock_Addr = 0x0;
static UINTPTR Xil_Spinlock_Flag_Addr = 0x0;

/************************** Function Prototypes *****************************/

/****************************************************************************/
/**
*
* @brief	Used at the beginning of a critical section. This API takes the
*           lock using load-exclusive and store-exclusive operations. In
*           case the lock is not available (being held by an application
*           running at the other CPU and doing operations inside the
*           critical section, the control will never go out from the API
*           till the other CPU releases the lock.
*
* @param    None.
*
* @return   XST_FAILURE: If spinlock is not properly initialized.
*           XST_SUCCESS: If the lock was successfully obtained.
*
* @note     None.
*
*****************************************************************************/
u32 Xil_SpinLock(void)
{
    UINTPTR lockaddr = Xil_Spinlock_Addr;
    u32 LockTempVar;

    if (Xil_Spinlock_Addr == 0U) {
		return XST_FAILURE;
	}

    __asm__ __volatile__(
	    "1:    ldrex    %0, [%1]     \n"
        "      teq		%0, %3       \n"
        "      strexeq  %0, %2, [%1] \n"
        "      teqeq	%0, #0       \n"
        "      bne      1b           \n"
        "      dmb                   \n"
		: "=&r" (LockTempVar)
		: "r" (lockaddr), "r"(XIL_SPINLOCK_LOCKVAL), "r"(XIL_SPINLOCK_RESETVAL)
		: "cc");

    return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* @brief	Used to release a lock previously held by calling Xil_SpinLock.
*
* @param    None.
*
* @return   XST_FAILURE: If spinlock is not properly initialized.
*           XST_SUCCESS: If the lock was successfully obtained.
*
* @note     None.
*
*****************************************************************************/
u32 Xil_SpinUnlock(void)
{
    UINTPTR lockaddr = Xil_Spinlock_Addr;

    if (Xil_Spinlock_Addr == 0U) {
        return XST_FAILURE;
	}
    __asm__ __volatile__(
        "dmb			         \n"
        "str 	%1, [%0]         \n"
        :
        : "r" (lockaddr), "r" (XIL_SPINLOCK_RESETVAL)
        : "cc");

    return XST_SUCCESS;
}

/****************************************************************************/
/**
* @brief	Used to initialize a spinlock.
*
* @param    lockaddr: Address of the lock variable. The address must be in
*           a shared memory region.
*           lockflagaddr: Address of the flag variable. The address must be
*           in a shared memory region.
*           lockflag: The value of the flag. Currently only one value
*           is supported, which is XIL_SPINLOCK_ENABLE.
*
* @return   XST_SUCCESS, if the initialization succeeded.
*           XST_FAILURE, if the initialization failed
*
* @note     None.
*
*****************************************************************************/
u32 Xil_InitializeSpinLock(UINTPTR lockaddr, UINTPTR lockflagaddr,
		                                                    u32 lockflag)
{
	if (Xil_Spinlock_Flag_Addr == 0U) {
		Xil_Spinlock_Flag_Addr = lockflagaddr;
		if (*(u32 *)Xil_Spinlock_Flag_Addr == XIL_SPINLOCK_ENABLED) {
			/*
			 * spinlock is already initialized by one of the CPUs, just update
			 * the local variable Xil_Spinlock_Addr with the passed address
			 * and dont write to the lock value as it might be currently
			 * getting used.
			 */
			/*
			 * Ensure to update the global variable Xil_Spinlock_Addr only
			 * if it is zero. A non-zero value may mean that there is something
			 * wrong.
			 */
			if (Xil_Spinlock_Addr == 0U) {
			    Xil_Spinlock_Addr = lockaddr;
		    } else {
				/*
				 * May be spinlock is already initialized and the user has not
				 * called Xil_ReleaseSpinLock before calling Xil_InitializeSpinLock.
	             */
				return XST_FAILURE;
			}
		} else {
			/* All good, do the necessary initializations */
			Xil_Spinlock_Addr = lockaddr;
			*(u32 *)(Xil_Spinlock_Addr) = XIL_SPINLOCK_RESETVAL;
			*(u32 *)(Xil_Spinlock_Flag_Addr) = lockflag;
		}
		return XST_SUCCESS;
	} else {
		/*
		 * Most probably spinlock is already initialized and the user has not
		 * called Xil_ReleaseSpinLock before calling Xil_InitializeSpinLock.
	     */
		return XST_FAILURE;
	}
}


/****************************************************************************/
/**
* @brief	Used to release the spinlock. Typically called by the application
*           once spinlock feature is no more required.
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
void Xil_ReleaseSpinLock(void)
{
    Xil_Spinlock_Addr = 0;
    Xil_Spinlock_Flag_Addr = 0;
}

/****************************************************************************/
/**
* @brief	Used to know if the spinlock feature has been enabled. To ensure
*           that spinlock feature does not break use cases where lock is
*           not initialized, the application must call this API first to know
*           if it can use spinlock. If this API returns non-zero value, an
*           application should then use Xil_SpinLock or Xil_SpinUnlock.
*
* @param    None
*
* @return   Non-zero, if spinlock is already initialized and can be used.
*           Zero, if spinlock is not initialized.
*
* @note
*
*****************************************************************************/
u32 Xil_IsSpinLockEnabled(void)
{
    u32 retVal = FALSE;

    if (Xil_Spinlock_Flag_Addr != 0U) {
        if (*(u32 *)(Xil_Spinlock_Flag_Addr) == XIL_SPINLOCK_ENABLED) {
            retVal = TRUE;
        }
    }
    return retVal;
}
#endif /* !(__aarch64__) &&  (__GNUC__) && !(__clang__)*/
