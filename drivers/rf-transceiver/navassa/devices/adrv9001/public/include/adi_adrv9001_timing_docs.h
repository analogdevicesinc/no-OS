/**
 * \file
 * 
 * \brief Description of message types and timing characteristics
 * 
 * \page timing Timing information
 * API calls fall into groups based on the message type used:
 *      - \subpage timing_direct
 *      - \subpage timing_mailbox
 *      - \subpage timing_prioritymailbox
 *      - \subpage timing_fastmessaging
 *      - \subpage timing_interrupt
 * 
 * \page timing_direct Direct ADRV9001 register access
 * API calls that bypass the ADRV9001 processor and directly write or read SPI registers
 *
 * \page timing_mailbox ADRV9001 mailbox commands
 * API calls that communicate via normal mailbox commands to the ADRV9001 onboard processor
 * 
 * \page timing_prioritymailbox High-priority ADRV9001 mailbox commands
 * API calls that communicate via high-priority mailbox commands to the ADRV9001 onboard processor
 * 
 * \page timing_fastmessaging Fast-messaging
 * API calls that communicate via fast message to the ADRV9001 onboard processor for more deterministic timing
 * 
 * \page timing_interrupt Software interrupt
 * API calls that communicate via software interrupt to the ADRV9001 onboard processor 
 *
 */