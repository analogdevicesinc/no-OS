;/*****************************************************************************/
;/* STARTUP.S: Startup file for ADI ADuC702x device series                    */
;/*****************************************************************************/
;/* <<< Use Configuration Wizard in Context Menu >>>                          */ 
;/*****************************************************************************/
;/* This file is part of the uVision/ARM development tools.                   */
;/* Copyright (c) 2005-2006 Keil Software. All rights reserved.               */
;/* This software may only be used under the terms of a valid, current,       */
;/* end user licence from KEIL for a compatible version of KEIL software      */
;/* development tools. Nothing else gives you the right to use this software. */
;/*****************************************************************************/


;/*
; *  The STARTUP.S code is executed after CPU Reset. This file may be 
; *  translated with the following SET symbols. In uVision these SET 
; *  symbols are entered under Options - ASM - Define.
; *
; *  RAM_INTVEC: when set the startup code copies exception vectors 
; *  from on-chip Flash to on-chip RAM and remaps RAM to address 0.
; */


; Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80            ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40            ; when F bit is set, FIQ is disabled


;// <h> Stack Configuration (Stack Sizes in Bytes)
;//   <o0> Undefined Mode      <0x0-0xFFFFFFFF:8>
;//   <o1> Supervisor Mode     <0x0-0xFFFFFFFF:8>
;//   <o2> Abort Mode          <0x0-0xFFFFFFFF:8>
;//   <o3> Fast Interrupt Mode <0x0-0xFFFFFFFF:8>
;//   <o4> Interrupt Mode      <0x0-0xFFFFFFFF:8>
;//   <o5> User/System Mode    <0x0-0xFFFFFFFF:8>
;// </h>

UND_Stack_Size  EQU     0x00000080
SVC_Stack_Size  EQU     0x00000080
ABT_Stack_Size  EQU     0x00000080
FIQ_Stack_Size  EQU     0x00000080
IRQ_Stack_Size  EQU     0x00000080
USR_Stack_Size  EQU     0x00000400

Stack_Size      EQU     (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                         FIQ_Stack_Size + IRQ_Stack_Size + USR_Stack_Size)

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size

Stack_Top       EQU     Stack_Mem + Stack_Size


;// <h> Heap Configuration
;//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF>
;// </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
Heap_Mem        SPACE   Heap_Size


; MMR definitions
MMR_BASE        EQU     0xFFFF0000      ; MMR Base Address
REMAP_OFFSET    EQU         0x0220
POWKEY1_OFFSET  EQU         0x0404
POWCON_OFFSET   EQU         0x0408
POWKEY2_OFFSET  EQU         0x040C

;// <e> PLL Setup
;//   <o1.0..2>  CD: PLL Multiplier Selection
;//               <0-7>
;//               <i> CD Value
;//   <o1.3>     FINT: Fast Interrupt
;//               <0-1>
;//               <i> Switches to CD0 for FIQ
;// </e>
PLL_SETUP       EQU     1
PLLCFG_Val      EQU     0x00000001


;// <e> Pin Setup
GPIO_SETUP      EQU     0
GPIOBASE        EQU     0xFFFFF400

;//   <h>  Port 0
;//     <o.0..1>    P0.0  <0=> GPIO      <1=> CMPOUT    <2=> MS2     <3=> PLAI[7]
;//     <o.4..5>    P0.1  <0=> GPIO      <1=> ---       <2=> XBEN0   <3=> ---
;//     <o.8..9>    P0.2  <0=> GPIO      <1=> ---       <2=> XBEN1   <3=> ---
;//     <o.12..13>  P0.3  <0=> GPIO      <1=> TRST      <2=> XA16    <3=> ADCBUSY
;//     <o.16..17>  P0.4  <0=> GPIO/IRQ0 <1=> CONVSTART <2=> MS1     <3=> PLAO[1]
;//     <o.20..21>  P0.5  <0=> GPIO/IRQ1 <1=> ADCBUSY   <2=> MS0     <3=> PLAO[2]
;//     <o.24..25>  P0.6  <0=> GPIO      <1=> MRST      <2=> XAE     <3=> PLAO[3]
;//     <o.28..29>  P0.7  <0=> GPIO      <1=> ECLK      <2=> SIN     <3=> PLAO[4]
;//   </h>
GP0CON_Val      EQU     0x00000000

;//   <h>  Port 1
;//     <o.0..1>    P1.0  <0=> GPIO      <1=> SIN       <2=> I2C0SCL <3=> PLAI[0]
;//     <o.4..5>    P1.1  <0=> GPIO      <1=> SOUT      <2=> I2C0SDA <3=> PLAI[1]
;//     <o.8..9>    P1.2  <0=> GPIO      <1=> RTS       <2=> I2C1SCL <3=> PLAI[2]
;//     <o.12..13>  P1.3  <0=> GPIO      <1=> CTS       <2=> I2C1SDA <3=> PLAI[3]
;//     <o.16..17>  P1.4  <0=> GPIO/IRQ2 <1=> RI        <2=> SPICLK  <3=> PLAI[4]
;//     <o.20..21>  P1.5  <0=> GPIO/IRQ3 <1=> DCD       <2=> SPIMISO <3=> PLAI[5]
;//     <o.24..25>  P1.6  <0=> GPIO      <1=> DSR       <2=> SPIMOSI <3=> PLAI[6]
;//     <o.28..29>  P1.7  <0=> GPIO      <1=> DTR       <2=> SPICSL  <3=> PLAO[0]
;//   </h>
GP1CON_Val      EQU     0x00000000

;//   <h>  Port 2
;//     <o.0..1>    P2.0  <0=> GPIO      <1=> CONVSTART <2=> SOUT    <3=> PLAO[5]
;//     <o.4..5>    P2.1  <0=> GPIO      <1=> ---       <2=> XWS     <3=> PLAO[6]
;//     <o.8..9>    P2.2  <0=> GPIO      <1=> ---       <2=> XRS     <3=> PLAO[7]
;//     <o.12..13>  P2.3  <0=> GPIO      <1=> ---       <2=> XAE     <3=> ---
;//     <o.16..17>  P2.4  <0=> GPIO      <1=> ---       <2=> MS0     <3=> ---
;//     <o.20..21>  P2.5  <0=> GPIO      <1=> ---       <2=> MS1     <3=> ---
;//     <o.24..25>  P2.6  <0=> GPIO      <1=> ---       <2=> MS2     <3=> ---
;//     <o.28..29>  P2.7  <0=> GPIO      <1=> ---       <2=> MS3     <3=> ---
;//   </h>
GP2CON_Val      EQU     0x00000000

;//   <h>  Port 3
;//     <o.0..1>    P3.0  <0=> GPIO      <1=> PWM0H     <2=> XAD0    <3=> PLAI[8]
;//     <o.4..5>    P3.1  <0=> GPIO      <1=> PWM0L     <2=> XAD1    <3=> PLAI[9]
;//     <o.8..9>    P3.2  <0=> GPIO      <1=> PWM1H     <2=> XAD2    <3=> PLAI[10]
;//     <o.12..13>  P3.3  <0=> GPIO      <1=> PWM1L     <2=> XAD3    <3=> PLAI[11]
;//     <o.16..17>  P3.4  <0=> GPIO      <1=> PWM2H     <2=> XAD4    <3=> PLAI[12]
;//     <o.20..21>  P3.5  <0=> GPIO      <1=> PWM2L     <2=> XAD5    <3=> PLAI[13]
;//     <o.24..25>  P3.6  <0=> GPIO      <1=> PWMTRIP   <2=> XAD6    <3=> PLAI[14]
;//     <o.28..29>  P3.7  <0=> GPIO      <1=> PWMSYNC   <2=> XAD7    <3=> PLAI[15]
;//   </h>
GP3CON_Val      EQU     0x00000000

;//   <h>  Port 4
;//     <o.0..1>    P4.0  <0=> GPIO      <1=> ---       <2=> XAD8    <3=> PLAO[8]
;//     <o.4..5>    P4.1  <0=> GPIO      <1=> ---       <2=> XAD9    <3=> PLAO[9]
;//     <o.8..9>    P4.2  <0=> GPIO      <1=> ---       <2=> XAD10   <3=> PLAO[10]
;//     <o.12..13>  P4.3  <0=> GPIO      <1=> ---       <2=> XAD11   <3=> PLAO[11]
;//     <o.16..17>  P4.4  <0=> GPIO      <1=> ---       <2=> XAD12   <3=> PLAO[12]
;//     <o.20..21>  P4.5  <0=> GPIO      <1=> ---       <2=> XAD13   <3=> PLAO[13]
;//     <o.24..25>  P4.6  <0=> GPIO      <1=> ---       <2=> XAD14   <3=> PLAO[14]
;//     <o.28..29>  P4.7  <0=> GPIO      <1=> ---       <2=> XAD15   <3=> PLAO[15]
;//   </h>
GP4CON_Val      EQU     0x00000000

;// </e>


;// <e> External Memory Interface
XM_SETUP        EQU     0
XMBASE          EQU     0xFFFFF000

;//   <e.0>  Enable Memory Region 0
;//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
;//     <o1.11>     Enable Dynamic Addressing
;//     <o1.15>     Byte Enabled Write Strobe
;//     <o1.10>     Disable extra Address Latch Hold Cycle
;//     <o1.8>      Disable extra Write Address Hold Cycle
;//     <o1.9>      Disable Read Bus Turn Cycle
;//     <o1.12..14> Address Wait States <0-7>
;//                 <i> Number of Wait States added for AE
;//     <o1.0..3>   Read Wait States <0-15>
;//                 <i> Number of Wait States added for RS
;//     <o1.4..7>   Write Wait States <0-15>
;//                 <i> Number of Wait States added for WS
;//   </e>
XM0CON_Val      EQU     0x00000000
XM0PAR_Val      EQU     0x000070FF

;//   <e.0>  Enable Memory Region 1
;//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
;//     <o1.11>     Enable Dynamic Addressing
;//     <o1.15>     Byte Enabled Write Strobe
;//     <o1.10>     Disable extra Address Latch Hold Cycle
;//     <o1.8>      Disable extra Write Address Hold Cycle
;//     <o1.9>      Disable Read Bus Turn Cycle
;//     <o1.12..14> Address Wait States <0-7>
;//                 <i> Number of Wait States added for AE
;//     <o1.0..3>   Read Wait States <0-15>
;//                 <i> Number of Wait States added for RS
;//     <o1.4..7>   Write Wait States <0-15>
;//                 <i> Number of Wait States added for WS
;//   </e>
XM1CON_Val      EQU     0x00000000
XM1PAR_Val      EQU     0x000070FF

;//   <e.0>  Enable Memory Region 2
;//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
;//     <o1.11>     Enable Dynamic Addressing
;//     <o1.15>     Byte Enabled Write Strobe
;//     <o1.10>     Disable extra Address Latch Hold Cycle
;//     <o1.8>      Disable extra Write Address Hold Cycle
;//     <o1.9>      Disable Read Bus Turn Cycle
;//     <o1.12..14> Address Wait States <0-7>
;//                 <i> Number of Wait States added for AE
;//     <o1.0..3>   Read Wait States <0-15>
;//                 <i> Number of Wait States added for RS
;//     <o1.4..7>   Write Wait States <0-15>
;//                 <i> Number of Wait States added for WS
;//   </e>
XM2CON_Val      EQU     0x00000000
XM2PAR_Val      EQU     0x000070FF

;//   <e.0>  Enable Memory Region 3
;//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
;//     <o1.11>     Enable Dynamic Addressing
;//     <o1.15>     Byte Enabled Write Strobe
;//     <o1.10>     Disable extra Address Latch Hold Cycle
;//     <o1.8>      Disable extra Write Address Hold Cycle
;//     <o1.9>      Disable Read Bus Turn Cycle
;//     <o1.12..14> Address Wait States <0-7>
;//                 <i> Number of Wait States added for AE
;//     <o1.0..3>   Read Wait States <0-15>
;//                 <i> Number of Wait States added for RS
;//     <o1.4..7>   Write Wait States <0-15>
;//                 <i> Number of Wait States added for WS
;//   </e>
XM3CON_Val      EQU     0x00000000
XM3PAR_Val      EQU     0x000070FF

;//   <e.0>         Memory Muxed Mode
XMCFG_Val       EQU     0x00000001
;//   </e>

;// </e>


                PRESERVE8
                

; Area Definition and Entry Point
;  Startup Code must be linked first at Address at which it expects to run.

                AREA    Reset, CODE, READONLY
                ARM


; Exception Vectors
;  Mapped to Address 0.
;  Absolute addressing mode must be used.
;  Dummy Handlers are implemented as infinite loops which can be modified.

Vectors         LDR     PC, Reset_Addr         
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                DCD     0xFFFFFFFF                       ; Reserved Vector 
                LDR     PC, IRQ_Addr
                LDR     PC, FIQ_Addr

                EXTERN Undef_Handler
                EXTERN SWI_Handler
                EXTERN PAbt_Handler
                EXTERN DAbt_Handler
                EXTERN IRQ_Handler
                EXTERN FIQ_Handler

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handler
                DCD     0xFFFFFFFF                       ; Reserved Address 
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler


; Reset Handler

                EXPORT  Reset_Handler
Reset_Handler   


; Setup PLL
                IF      PLL_SETUP <> 0
                LDR     R0, =MMR_BASE
                MOV     R1, #0x01         
                STR     R1, [R0,#POWKEY1_OFFSET]          
                MOV     R1, #PLLCFG_Val      
                STR     R1, [R0,#POWCON_OFFSET]    
                MOV     R1, #0xF4
                STR     R1, [R0,#POWKEY2_OFFSET]
                ENDIF   ; PLL_SETUP


; Setup Pins
                IF      GPIO_SETUP <> 0

                ADR     R10, GPIO_CFG          ; Pointer to GPIO CFG
                LDMIA   R10, {R0-R5}           ; Load GPIO Configuration
                STMIA   R0, {R1-R5}            ; Store GPxCON
                B       GPIO_END

GPIO_CFG        DCD     GPIOBASE
                DCD     GP0CON_Val
                DCD     GP1CON_Val
                DCD     GP2CON_Val
                DCD     GP3CON_Val
                DCD     GP4CON_Val
GPIO_END 

                ENDIF   ; GPIO_SETUP


; Setup External Memory Interface
                IF      XM_SETUP <> 0

                ADR     R10, XM_CFG            ; Pointer to XM CFG
                LDMIA   R10, {R0-R9}           ; Load XM Configuration
                STR     R1, [R0],#0x10         ; Store XMCFG
                STMIA   R0, {R2-R9}            ; Store XMxCON & XMxPAR
                B       XM_END

XM_CFG          DCD     XMBASE
                DCD     XMCFG_Val
                DCD     XM0CON_Val
                DCD     XM1CON_Val
                DCD     XM2CON_Val
                DCD     XM3CON_Val
                DCD     XM0PAR_Val
                DCD     XM1PAR_Val
                DCD     XM2PAR_Val
                DCD     XM3PAR_Val
XM_END

                ENDIF   ; XM_SETUP


; Copy Exception Vectors to Internal RAM and Remap Memory
;  (when Interrupt Vectors are in RAM)
									   
                IF      :DEF:RAM_INTVEC
                ADR     R8, Vectors         ; Source
                LDR     R9, =0x00010000     ; Destination
                LDMIA   R8!, {R0-R7}        ; Load Vectors 
                STMIA   R9!, {R0-R7}        ; Store Vectors 
                LDMIA   R8!, {R0-R7}        ; Load Handler Addresses 
                STMIA   R9!, {R0-R7}        ; Store Handler Addresses 
                LDR     R0, =MMR_BASE
                MOV     R1, #1     
                STR     R1, [R0,#REMAP_OFFSET]          
                ENDIF


; Setup Stack for each mode

                LDR     R0, =Stack_Top

;  Enter Undefined Instruction Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

;  Enter Abort Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

;  Enter FIQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

;  Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

;  Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

;  Enter User Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_USR
                MOV     SP, R0
                SUB     SL, SP, #USR_Stack_Size


; Enter the C code

                IMPORT  __main
                LDR     R0, =__main
                BX      R0


; User Initial Stack & Heap
                AREA    |.text|, CODE, READONLY

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + USR_Stack_Size)
                LDR     R2, = (Heap_Mem +      Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR


                END
