/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/


#ifdef USE_PROTOCOL_MAILBOX
#define u1 unsigned char 
#define u2 unsigned short
#define s2 signed short

#include <MXQ_API.h>
#include <Hal_Mailbox.h>
#include <error.h>

#define BLOCK_TIMEOUT (10000U)

extern mb_mailbox_t gMailboxIn;
extern mb_mailbox_t gMailboxOut;

static mb_mailbox_t *pMailboxIncoming;
static mb_mailbox_t *pMailboxOutgoing;

extern FN_STATUS Mailbox_Initialize(mb_mailbox_t *mailbox, u2 pBlockTimeout);
extern FN_STATUS Mailbox_SendBytes(mb_mailbox_t *mailbox, u1* pSrc, size_t pLen);
extern FN_STATUS Mailbox_Receive_Bytes(mb_mailbox_t *mailbox, u1* pDest, size_t* pLen);
extern FN_STATUS Mailbox_ReceiveBytesFirst(mb_mailbox_t *mailbox, u1* pDest, size_t* pLen);

mxq_err_t HOST_init_mailbox(void)
{
    mxq_err_t retVal = MXQ_OK;

    pMailboxIncoming = &gMailboxOut;
    pMailboxOutgoing = &gMailboxIn;

    if(Mailbox_Initialize(pMailboxIncoming, BLOCK_TIMEOUT))
        retVal = MXQ_RSP_ERROR_UNKNOWN;
    if(Mailbox_Initialize(pMailboxOutgoing, BLOCK_TIMEOUT))
        retVal = MXQ_RSP_ERROR_UNKNOWN;
    
    return(retVal);
}

mxq_err_t HOST_send_bytes_mailbox(const mxq_u1 *src, mxq_u2 len)
{
    mxq_err_t retVal = MXQ_OK;

    FN_STATUS mailboxResult = Mailbox_SendBytes(pMailboxOutgoing, (mxq_u1 *)src, len);

    if (mailboxResult != 0) {
        retVal = MXQ_RSP_ERROR_UNKNOWN;
    }

    return(retVal);
}

mxq_err_t HOST_receive_bytes_mailbox(mxq_u1 *dest, mxq_u2 len)
{
    mxq_err_t retVal = MXQ_OK;
    size_t data_len = len;

    FN_STATUS mailboxResult = Mailbox_Receive_Bytes(pMailboxIncoming, dest, &data_len);

    if (mailboxResult != 0) {
        retVal = MXQ_RSP_ERROR_UNKNOWN;
    }
    else
        retVal = len;

    return(retVal);
}

mxq_err_t HOST_receive_bytes_mailbox_first(mxq_u1 *dest, mxq_u2 len)
{
    mxq_err_t retVal = MXQ_OK;

    retVal = HOST_receive_bytes_mailbox(dest, len);

    if(retVal == MXQ_OK)
        retVal = len;
     
    return(retVal);
}

#endif