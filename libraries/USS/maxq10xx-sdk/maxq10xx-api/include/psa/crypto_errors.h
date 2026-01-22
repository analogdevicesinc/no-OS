/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef CRYTPO_ERROR_H
#define CRYTPO_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>

/*A status code to indicate general success*/
#define PSA_SUCCESS                         ((psa_status_t)0)

/*This error indicates that the function has detected an abnormal call, which typically indicates a
programming error in the caller, or an abuse of the API*/
#define PSA_ERROR_PROGRAMMER_ERROR          ((psa_status_t)-129)

/*A status code that indicates that the caller is not permitted to connect to a Service*/
#define PSA_ERROR_CONNECTION_REFUSED        ((psa_status_t)-130)

/*A status code that indicates that the caller cannot connect to a service*/
#define PSA_ERROR_CONNECTION_BUSY           ((psa_status_t)-131)

/*A status code that indicates an error that does not correspond to any defined failure cause*/
#define PSA_ERROR_GENERIC_ERROR             ((psa_status_t)-132)

/*A status code that indicates that the requested action is denied by a policy*/
#define PSA_ERROR_NOT_PERMITTED             ((psa_status_t)-133)

/*A status code that indicates that the requested operation or a parameter is not supported*/
#define PSA_ERROR_NOT_SUPPORTED             ((psa_status_t)-134)

/*A status code that indicates that the parameters passed to the function are invalid*/
#define PSA_ERROR_INVALID_ARGUMENT          ((psa_status_t)-135)

/*A status code that indicates that a handle parameter is not valid.A function can return this 
error any time it expects a parameter to be a handle to an existing resource, but
the handle is invalid.*/
#define PSA_ERROR_INVALID_HANDLE            ((psa_status_t)-136)

/*A status code that indicates that the requested action cannot be performed in the current state.
It is recommended that a function returns this error when an operation is requested out of sequence*/
#define PSA_ERROR_BAD_STATE                 ((psa_status_t)-137)

/*A status code that indicates that an output buffer parameter is too small*/
#define PSA_ERROR_BUFFER_TOO_SMALL          ((psa_status_t)-138)

/*A status code that indicates that an identifier or index is already in use*/
#define PSA_ERROR_ALREADY_EXISTS            ((psa_status_t)-139)

/*A status code that indicates that an identified resource does not exist*/
#define PSA_ERROR_DOES_NOT_EXIST            ((psa_status_t)-140)

/*A status code that indicates that there is not enough runtime memory*/
#define PSA_ERROR_INSUFFICIENT_MEMORY       ((psa_status_t)-141)

/*A status code that indicates that there is not enough persistent storage*/
#define PSA_ERROR_INSUFFICIENT_STORAGE      ((psa_status_t)-142)

/*A status code that indicates that a data source has insufficient capacity left*/
#define PSA_ERROR_INSUFFICIENT_DATA         ((psa_status_t)-143)

/*A status code that indicates an error within the service*/
#define PSA_ERROR_SERVICE_FAILURE           ((psa_status_t)-144)

/*A status code that indicates a communication failure between the function and another service or
component*/
#define PSA_ERROR_COMMUNICATION_FAILURE     ((psa_status_t)-145)

/*A status code that indicates a storage failure that may have led to data loss*/
#define PSA_ERROR_STORAGE_FAILURE           ((psa_status_t)-146)

/*A status code that indicates that a hardware failure was detected*/
#define PSA_ERROR_HARDWARE_FAILURE          ((psa_status_t)-147)

/*A status code that indicates that there is not enough entropy to generate random data needed for the
requested action*/
#define PSA_ERROR_INSUFFICIENT_ENTROPY      ((psa_status_t)-148)

/*A status code that indicates that a signature, MAC or hash is incorrect*/
#define PSA_ERROR_INVALID_SIGNATURE         ((psa_status_t)-149)

/*A status code that indicates that the decrypted padding is incorrect*/
#define PSA_ERROR_INVALID_PADDING           ((psa_status_t)-150)

/*A status code that indicates that internal data has been tampered with.
A function can return this error if it detects an invalid state that cannot happen during normal operation
and that indicates that the implementation’s security guarantees no longer hold.*/
#define PSA_ERROR_CORRUPTION_DETECTED       ((psa_status_t)-151)

/*A status code that indicates that stored data has been corrupted*/
#define PSA_ERROR_DATA_CORRUPT              ((psa_status_t)-152)

/*A status code that indicates that data read from storage is not valid for the implementation*/
#define PSA_ERROR_DATA_INVALID              ((psa_status_t)-153)

/*A status code that indicates that the requested operation is interruptible, and still has work to do*/
#define PSA_OPERATION_INCOMPLETE            ((psa_status_t)-248)


#ifdef __cplusplus
}
#endif


#endif //CRYTPO_ERROR_H