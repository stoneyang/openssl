/* crypto/cryptlib.c */
/* ====================================================================
 * Copyright (c) 1998-2006 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * ECDH support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */

#include "internal/cryptlib.h"
#include <openssl/safestack.h>

#if defined(OPENSSL_SYS_WIN32)
static double SSLeay_MSVC5_hack = 0.0; /* and for VC1.5 */
#endif

DECLARE_STACK_OF(CRYPTO_dynlock)

/* real #defines in crypto.h, keep these upto date */
static const char *const lock_names[CRYPTO_NUM_LOCKS] = {
    "<<ERROR>>",
    "err",
    "ex_data",
    "x509",
    "x509_info",
    "x509_pkey",
    "x509_crl",
    "x509_req",
    "dsa",
    "rsa",
    "evp_pkey",
    "x509_store",
    "ssl_ctx",
    "ssl_cert",
    "ssl_session",
    "ssl_sess_cert",
    "ssl",
    "ssl_method",
    "rand",
    "rand2",
    "debug_malloc",
    "BIO",
    "gethostbyname",
    "getservbyname",
    "readdir",
    "RSA_blinding",
    "dh",
    "debug_malloc2",
    "dso",
    "dynlock",
    "engine",
    "ui",
    "ecdsa",
    "ec",
    "ecdh",
    "bn",
    "ec_pre_comp",
    "store",
    "comp",
    "fips",
    "fips2",
#if CRYPTO_NUM_LOCKS != 41
# error "Inconsistency between crypto.h and cryptlib.c"
#endif
};

/*
 * This is for applications to allocate new type names in the non-dynamic
 * array of lock names.  These are numbered with positive numbers.
 */
static STACK_OF(OPENSSL_STRING) *app_locks = NULL;

/*
 * For applications that want a more dynamic way of handling threads, the
 * following stack is used.  These are externally numbered with negative
 * numbers.
 */
static STACK_OF(CRYPTO_dynlock) *dyn_locks = NULL;

static void (*locking_callback) (int mode, int type,
                                 const char *file, int line) = 0;
static int (*add_lock_callback) (int *pointer, int amount,
                                 int type, const char *file, int line) = 0;
static struct CRYPTO_dynlock_value *(*dynlock_create_callback)
 (const char *file, int line) = 0;
static void (*dynlock_lock_callback) (int mode,
                                      struct CRYPTO_dynlock_value *l,
                                      const char *file, int line) = 0;
static void (*dynlock_destroy_callback) (struct CRYPTO_dynlock_value *l,
                                         const char *file, int line) = 0;

int CRYPTO_get_new_lockid(char *name)
{
    char *str;
    int i;

#if defined(OPENSSL_SYS_WIN32)
    /*
     * A hack to make Visual C++ 5.0 work correctly when linking as a DLL
     * using /MT. Without this, the application cannot use any floating point
     * printf's. It also seems to be needed for Visual C 1.5 (win16)
     */
    SSLeay_MSVC5_hack = (double)name[0] * (double)name[1];
#endif

    if ((app_locks == NULL)
        && ((app_locks = sk_OPENSSL_STRING_new_null()) == NULL)) {
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_LOCKID, ERR_R_MALLOC_FAILURE);
        return (0);
    }
    if ((str = BUF_strdup(name)) == NULL) {
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_LOCKID, ERR_R_MALLOC_FAILURE);
        return (0);
    }
    i = sk_OPENSSL_STRING_push(app_locks, str);
    if (!i)
        OPENSSL_free(str);
    else
        i += CRYPTO_NUM_LOCKS;  /* gap of one :-) */
    return (i);
}

int CRYPTO_num_locks(void)
{
    return CRYPTO_NUM_LOCKS;
}

int CRYPTO_get_new_dynlockid(void)
{
    int i = 0;
    CRYPTO_dynlock *pointer = NULL;

    if (dynlock_create_callback == NULL) {
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_DYNLOCKID,
                  CRYPTO_R_NO_DYNLOCK_CREATE_CALLBACK);
        return (0);
    }
    CRYPTO_w_lock(CRYPTO_LOCK_DYNLOCK);
    if ((dyn_locks == NULL)
        && ((dyn_locks = sk_CRYPTO_dynlock_new_null()) == NULL)) {
        CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_DYNLOCKID, ERR_R_MALLOC_FAILURE);
        return (0);
    }
    CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);

    pointer = OPENSSL_malloc(sizeof(*pointer));
    if (pointer == NULL) {
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_DYNLOCKID, ERR_R_MALLOC_FAILURE);
        return (0);
    }
    pointer->references = 1;
    pointer->data = dynlock_create_callback(__FILE__, __LINE__);
    if (pointer->data == NULL) {
        OPENSSL_free(pointer);
        CRYPTOerr(CRYPTO_F_CRYPTO_GET_NEW_DYNLOCKID, ERR_R_MALLOC_FAILURE);
        return (0);
    }

    CRYPTO_w_lock(CRYPTO_LOCK_DYNLOCK);
    /* First, try to find an existing empty slot */
    i = sk_CRYPTO_dynlock_find(dyn_locks, NULL);
    /* If there was none, push, thereby creating a new one */
    if (i == -1)
        /*
         * Since sk_push() returns the number of items on the stack, not the
         * location of the pushed item, we need to transform the returned
         * number into a position, by decreasing it.
         */
        i = sk_CRYPTO_dynlock_push(dyn_locks, pointer) - 1;
    else
        /*
         * If we found a place with a NULL pointer, put our pointer in it.
         */
        (void)sk_CRYPTO_dynlock_set(dyn_locks, i, pointer);
    CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);

    if (i == -1) {
        dynlock_destroy_callback(pointer->data, __FILE__, __LINE__);
        OPENSSL_free(pointer);
    } else
        i += 1;                 /* to avoid 0 */
    return -i;
}

void CRYPTO_destroy_dynlockid(int i)
{
    CRYPTO_dynlock *pointer = NULL;
    if (i)
        i = -i - 1;
    if (dynlock_destroy_callback == NULL)
        return;

    CRYPTO_w_lock(CRYPTO_LOCK_DYNLOCK);

    if (dyn_locks == NULL || i >= sk_CRYPTO_dynlock_num(dyn_locks)) {
        CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);
        return;
    }
    pointer = sk_CRYPTO_dynlock_value(dyn_locks, i);
    if (pointer != NULL) {
        --pointer->references;
#ifdef REF_CHECK
        if (pointer->references < 0) {
            fprintf(stderr,
                    "CRYPTO_destroy_dynlockid, bad reference count\n");
            abort();
        } else
#endif
        if (pointer->references <= 0) {
            (void)sk_CRYPTO_dynlock_set(dyn_locks, i, NULL);
        } else
            pointer = NULL;
    }
    CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);

    if (pointer) {
        dynlock_destroy_callback(pointer->data, __FILE__, __LINE__);
        OPENSSL_free(pointer);
    }
}

struct CRYPTO_dynlock_value *CRYPTO_get_dynlock_value(int i)
{
    CRYPTO_dynlock *pointer = NULL;
    if (i)
        i = -i - 1;

    CRYPTO_w_lock(CRYPTO_LOCK_DYNLOCK);

    if (dyn_locks != NULL && i < sk_CRYPTO_dynlock_num(dyn_locks))
        pointer = sk_CRYPTO_dynlock_value(dyn_locks, i);
    if (pointer)
        pointer->references++;

    CRYPTO_w_unlock(CRYPTO_LOCK_DYNLOCK);

    if (pointer)
        return pointer->data;
    return NULL;
}

struct CRYPTO_dynlock_value *(*CRYPTO_get_dynlock_create_callback(void))
 (const char *file, int line) {
    return (dynlock_create_callback);
}

void (*CRYPTO_get_dynlock_lock_callback(void)) (int mode,
                                                struct CRYPTO_dynlock_value
                                                *l, const char *file,
                                                int line) {
    return (dynlock_lock_callback);
}

void (*CRYPTO_get_dynlock_destroy_callback(void))
 (struct CRYPTO_dynlock_value *l, const char *file, int line) {
    return (dynlock_destroy_callback);
}

void CRYPTO_set_dynlock_create_callback(struct CRYPTO_dynlock_value *(*func)
                                         (const char *file, int line))
{
    dynlock_create_callback = func;
}

void CRYPTO_set_dynlock_lock_callback(void (*func) (int mode,
                                                    struct
                                                    CRYPTO_dynlock_value *l,
                                                    const char *file,
                                                    int line))
{
#ifdef OPENSSL_FIPS
    FIPS_set_locking_callbacks(CRYPTO_lock, CRYPTO_add_lock);
#endif
    dynlock_lock_callback = func;
}

void CRYPTO_set_dynlock_destroy_callback(void (*func)
                                          (struct CRYPTO_dynlock_value *l,
                                           const char *file, int line))
{
    dynlock_destroy_callback = func;
}

void (*CRYPTO_get_locking_callback(void)) (int mode, int type,
                                           const char *file, int line) {
    return (locking_callback);
}

int (*CRYPTO_get_add_lock_callback(void)) (int *num, int mount, int type,
                                           const char *file, int line) {
    return (add_lock_callback);
}

void CRYPTO_set_locking_callback(void (*func) (int mode, int type,
                                               const char *file, int line))
{
#ifdef OPENSSL_FIPS
    FIPS_set_locking_callbacks(CRYPTO_lock, CRYPTO_add_lock);
#endif
    locking_callback = func;
}

void CRYPTO_set_add_lock_callback(int (*func) (int *num, int mount, int type,
                                               const char *file, int line))
{
    add_lock_callback = func;
}

void CRYPTO_lock(int mode, int type, const char *file, int line)
{
#ifdef LOCK_DEBUG
    {
        CRYPTO_THREADID id;
        char *rw_text, *operation_text;

        if (mode & CRYPTO_LOCK)
            operation_text = "lock  ";
        else if (mode & CRYPTO_UNLOCK)
            operation_text = "unlock";
        else
            operation_text = "ERROR ";

        if (mode & CRYPTO_READ)
            rw_text = "r";
        else if (mode & CRYPTO_WRITE)
            rw_text = "w";
        else
            rw_text = "ERROR";

        CRYPTO_THREADID_current(&id);
        fprintf(stderr, "lock:%08lx:(%s)%s %-18s %s:%d\n",
                CRYPTO_THREADID_hash(&id), rw_text, operation_text,
                CRYPTO_get_lock_name(type), file, line);
    }
#endif
    if (type < 0) {
        if (dynlock_lock_callback != NULL) {
            struct CRYPTO_dynlock_value *pointer
                = CRYPTO_get_dynlock_value(type);

            OPENSSL_assert(pointer != NULL);

            dynlock_lock_callback(mode, pointer, file, line);

            CRYPTO_destroy_dynlockid(type);
        }
    } else if (locking_callback != NULL)
        locking_callback(mode, type, file, line);
}

int CRYPTO_add_lock(int *pointer, int amount, int type, const char *file,
                    int line)
{
    int ret = 0;

    if (add_lock_callback != NULL) {
#ifdef LOCK_DEBUG
        int before = *pointer;
#endif

        ret = add_lock_callback(pointer, amount, type, file, line);
#ifdef LOCK_DEBUG
        {
            CRYPTO_THREADID id;
            CRYPTO_THREADID_current(&id);
            fprintf(stderr, "ladd:%08lx:%2d+%2d->%2d %-18s %s:%d\n",
                    CRYPTO_THREADID_hash(&id), before, amount, ret,
                    CRYPTO_get_lock_name(type), file, line);
        }
#endif
    } else {
        CRYPTO_lock(CRYPTO_LOCK | CRYPTO_WRITE, type, file, line);

        ret = *pointer + amount;
#ifdef LOCK_DEBUG
        {
            CRYPTO_THREADID id;
            CRYPTO_THREADID_current(&id);
            fprintf(stderr, "ladd:%08lx:%2d+%2d->%2d %-18s %s:%d\n",
                    CRYPTO_THREADID_hash(&id),
                    *pointer, amount, ret,
                    CRYPTO_get_lock_name(type), file, line);
        }
#endif
        *pointer = ret;
        CRYPTO_lock(CRYPTO_UNLOCK | CRYPTO_WRITE, type, file, line);
    }
    return (ret);
}

const char *CRYPTO_get_lock_name(int type)
{
    if (type < 0)
        return ("dynamic");
    else if (type < CRYPTO_NUM_LOCKS)
        return (lock_names[type]);
    else if (type - CRYPTO_NUM_LOCKS > sk_OPENSSL_STRING_num(app_locks))
        return ("ERROR");
    else
        return (sk_OPENSSL_STRING_value(app_locks, type - CRYPTO_NUM_LOCKS));
}
