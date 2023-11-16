#include "tls.h"

typedef struct
{
    adi_hal_thread_t thread;
    void* value;
} thread_to_value_t;

/* ADI Error Reporting Dependency on HAL_TLS_ERR being set to NULL if not configured */
static thread_to_value_t store[HAL_TLS_END][ADI_HAL_MAX_THREADS] = { { { 0, NULL } }, { { 0, NULL } } };
static adi_hal_mutex_t storeMutex;

adi_hal_Err_e common_TlsInit(void)
{
    if (adi_hal_MutexInit == NULL)
    {
        return ADI_HAL_ERR_NOT_IMPLEMENTED;
    }

    /* Error Reporting Dependency on HAL_TLS_ERR being set to NULL if not configured */
    ADI_LIBRARY_MEMSET(store, 0, sizeof(store));

    return adi_hal_MutexInit(&storeMutex);
}

adi_hal_Err_e common_TlsSet(const adi_hal_TlsType_e tlsType, void* const value)
{
    adi_hal_Err_e retVal = ADI_HAL_ERR_OK;
    uint32_t idx = 0U;
    uint32_t freeSlotIdx = ADI_HAL_MAX_THREADS;
    adi_hal_thread_t thisThread = adi_hal_ThreadSelf();

    if (tlsType == HAL_TLS_END)
    {
        /* Special convenience case that a thread can remove all it TLS
         * items in one call by passing (HAL_TLS_END, NULL); */
        if (value != NULL)
        {
            return ADI_HAL_ERR_PARAM;
        }

        /* Passing NULL cannot fail by definition - no need to check rtn values */
        (void) common_TlsSet(HAL_TLS_ERR, NULL);
        (void) common_TlsSet(HAL_TLS_USR, NULL);

        return ADI_HAL_ERR_OK;
    }

    (void) adi_hal_MutexLock(&storeMutex);

    /* Find the thread's slot or failing that a free slot */
    for (idx = 0; idx < ADI_HAL_MAX_THREADS; ++idx)
    {
        if ((store[tlsType][idx].value == NULL) && (freeSlotIdx == ADI_HAL_MAX_THREADS))
        {
            /* This is the first free slot found - remember it in case there
             * is no slot found for the thread. For subsequent free slots this won't
             * happen as freeSlotIdx is no longer == ADI_HAL_MAX_THREADS.
             */
            freeSlotIdx = idx;
        }

        if (store[tlsType][idx].thread == thisThread)
        {
            /* Slot found for this thread */
            break;
        }
    }

    if (idx < ADI_HAL_MAX_THREADS)
    {
        /* idx is at slot for this thread - update */
        store[tlsType][idx].thread = thisThread;
        store[tlsType][idx].value = value;
    }
    else if (freeSlotIdx < ADI_HAL_MAX_THREADS)
    {
        /* no entry for this thread use the free slot */
        store[tlsType][freeSlotIdx].thread = thisThread;
        store[tlsType][freeSlotIdx].value = value;
    }
    else if (value == NULL)
    {
        /* no slot available but as we are storing NULL which is used to indicate that
         * the value entry for this thread is being removed that is success.
         */
        retVal = ADI_HAL_ERR_OK;
    }
    else
    {
        /* no slot available for the entry */
        retVal = ADI_HAL_ERR_MEMORY;
    }

    (void) adi_hal_MutexUnlock(&storeMutex);

    return retVal;
}

void* common_TlsGet(adi_hal_TlsType_e tlsType)
{
    uint32_t idx = 0;
    void* retVal = NULL;

    adi_hal_thread_t thisThread = adi_hal_ThreadSelf();

    if (tlsType == HAL_TLS_END)
    {
        return NULL;
    }

    (void) adi_hal_MutexLock(&storeMutex);

    /* Find the thread's slot */
    for (idx = 0; idx < ADI_HAL_MAX_THREADS; idx++)
    {
        if (store[tlsType][idx].thread == thisThread)
        {
            break;
        }
    }

    if (idx < ADI_HAL_MAX_THREADS)
    {
        retVal = store[tlsType][idx].value;
    }

    (void) adi_hal_MutexUnlock(&storeMutex);

    return retVal;
}
