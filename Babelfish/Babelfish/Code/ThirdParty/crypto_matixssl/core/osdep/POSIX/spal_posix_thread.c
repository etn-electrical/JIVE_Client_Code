/* spal_posix_thread.c
 *
 * Description: Posix thread APIs
 */

/*****************************************************************************
* Copyright (c) 2007-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#include "spal_thread.h"
#include "implementation_defs.h"

#ifndef CERTLIB_NO_MULTITHREADING
# include "osdep_pthread.h"
#endif /* !CERTLIB_NO_MULTITHREADING */
#include "osdep_stdlib.h"
#include "osdep_errno.h"


COMPILE_GLOBAL_ASSERT(sizeof(SPAL_Thread_t) >= sizeof(pthread_t));


SPAL_Thread_t
SPAL_Thread_Self(
    void)
{
#ifndef CERTLIB_NO_MULTITHREADING
    return (SPAL_Thread_t) pthread_self();
#else
    return 1;
#endif /* !CERTLIB_NO_MULTITHREADING */
}


SPAL_Result_t
SPAL_Thread_Create(
    SPAL_Thread_t * const Thread_p,
    const void * const Reserved_p,
    void * (*StartFunction_p)(void * const Param_p),
    void * const ThreadParam_p)
{
#ifndef CERTLIB_NO_MULTITHREADING
    pthread_t NewThread;
    int rval;

    PRECONDITION(Thread_p != NULL);
    PRECONDITION(Reserved_p == NULL);
    PRECONDITION(StartFunction_p != NULL);

# ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    PARAMETER_NOT_USED(Reserved_p);
# endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */

    rval =
        pthread_create(
            &NewThread,
            /* attr: */ NULL,
            StartFunction_p,
            ThreadParam_p);

    if (rval != 0)
    {
        ASSERT(errno == EAGAIN);

        return SPAL_RESULT_NORESOURCE;
    }

    *Thread_p = NewThread;
#endif /* !CERTLIB_NO_MULTITHREADING */

    return SPAL_SUCCESS;
}


SPAL_Result_t
SPAL_Thread_Detach(
    const SPAL_Thread_t Thread)
{
    SPAL_Result_t Result = SPAL_SUCCESS;

#ifndef CERTLIB_NO_MULTITHREADING
    pthread_t th = (pthread_t) Thread;
    int rval;

    rval =
        pthread_detach(
            th);

    if (rval != 0)
    {
        switch (errno)
        {
        case ESRCH:
            Result = SPAL_RESULT_NOTFOUND;
            break;

        case EINVAL:
            Result = SPAL_RESULT_INVALID;
            break;
        }

        POSTCONDITION(Result != SPAL_SUCCESS);

        return Result;
    }

    POSTCONDITION(Result == SPAL_SUCCESS);
#endif /* !CERTLIB_NO_MULTITHREADING */

    return Result;
}



SPAL_Result_t
SPAL_Thread_Join(
    const SPAL_Thread_t Thread,
    void ** const Status_p)
{
#ifndef CERTLIB_NO_MULTITHREADING
    SPAL_Result_t Result = SPAL_SUCCESS;
    pthread_t th = Thread;
    void *thread_return;

    int rval;

    PRECONDITION(Thread != SPAL_Thread_Self());

    rval = pthread_join(th, &thread_return);

    if (rval != 0)
    {
        switch (errno)
        {
        case ESRCH:
            /* No thread with the ID */
            Result = SPAL_RESULT_NOTFOUND;
            break;


        case EINVAL:
            /* The thread was detached or */
            /* another thread already waiting the thread. */
            Result = SPAL_RESULT_INVALID;
            break;
# if !defined(EDEADLOCK)
        case EDEADLK:
            PANIC("pthread join returned EDEADLK");
            break;
# else
        case EDEADLOCK:
            PANIC("pthread join returned EDEADLOCK");
            break;
# endif
        }

        POSTCONDITION(Result != SPAL_SUCCESS);

        return Result;
    }

    POSTCONDITION(Result == SPAL_SUCCESS);

    if (Status_p != NULL)
    {
        *Status_p = thread_return;
    }
#endif /* !CERTLIB_NO_MULTITHREADING */

    return SPAL_SUCCESS;
}


void
SPAL_Thread_Exit(
    void * const Status)
{
#ifndef CERTLIB_NO_MULTITHREADING
    pthread_exit(Status);
#endif /* !CERTLIB_NO_MULTITHREADING */
}

/* end of file spal_posix_thread.c */
