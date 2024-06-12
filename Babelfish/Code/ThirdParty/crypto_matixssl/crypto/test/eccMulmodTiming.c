/* eccMulmodTiming.c
 * Timing analysis for eccMulmod, with number of leading zero bits
 * in the scalar as a parameter.
 */
# include "crypto/cryptoImpl.h"

# ifndef USE_MATRIX_ECC

int main(int argc, char **argv)
{
    printf("This test currently requires USE_MATRIX_ECC\n");
    return 1;
}

# else
# include <time.h>
# include "osdep_unistd.h"

extern
psEccPoint_t *eccNewPoint(psPool_t *pool,
        short size);
extern
void eccFreePoint(psEccPoint_t *p);
extern
int32_t eccMulmod(psPool_t *pool,
        const pstm_int *k,
        const psEccPoint_t *G,
        psEccPoint_t *R,
        pstm_int *modulus,
        uint8_t map,
        pstm_int *tmp_int);

# define log_err(...)                           \
    do                                          \
    {                                           \
        fprintf(stderr, __VA_ARGS__);           \
    }                                           \
    while(0)

# ifdef ECCMULMOD_VERBOSE
#  define log_info(...)                         \
    do                                          \
    {                                           \
        fprintf(stderr, __VA_ARGS__);           \
    }                                           \
    while(0)
# else
#  define log_info(...)
# endif

# define BENCHMARK(...)                                                 \
    do                                                                  \
    {                                                                   \
        clock_t start_time, end_time;                                   \
        double elapsed_time;                                            \
        start_time = clock();                                           \
        __VA_ARGS__;                                                    \
        end_time = clock();                                             \
        elapsed_time = difftime(end_time, start_time) / CLOCKS_PER_SEC; \
        log_info("%s took %f\n", #__VA_ARGS__, elapsed_time);           \
        time_out = elapsed_time;                                        \
    }                                                                   \
    while (0)

static
double get_clock_precision(void)
{
    clock_t a, b;
    double elapsed;

    a = clock();
    b = clock();
    while (a == b)
    {
        b = clock();
    }

    elapsed = difftime(b, a) / CLOCKS_PER_SEC;

    return elapsed;
}

static
void func_fail(
        const char *called_func,
        int rc,
        const char *func,
        const char *file,
        unsigned int line)
{
    log_err("%s (in %s) failed with return value %d (%s:%d)\n",
            called_func,
            func,
            rc,
            file,
            line);
}

# define FAIL_FUNC_IF(expr, rc, called_func, fail_action)               \
    do {                                                                \
        if ((expr))                                                     \
        {                                                               \
            func_fail(called_func, rc, __func__, __FILE__, __LINE__);   \
            fail_action;                                                \
        }                                                               \
    }                                                                   \
    while (0)

static
int generate_k(pstm_int *k, psSize_t curveSize, int nlz)
{
    unsigned char rnd_bytes[66] = { 0 };
    int nlzb = nlz/8;
    size_t num_bytes = curveSize - nlzb;
    psRandom_t ctx;
    uint16_t nbits;

    log_info("Trying to generate scalar with %hu bytes and "    \
            "at least %d leading zeros...\n", curveSize, nlz);

    if ((nlz % 8) != 0)
    {
        log_err("Error: currently only supporting nlz = 8k\n");
        return -1;
    }

    psInitPrng(&ctx, NULL);
    psGetPrng(&ctx,
            rnd_bytes + nlzb,
            num_bytes,
            NULL);
    pstm_init_for_read_unsigned_bin(NULL, k, num_bytes);
    pstm_read_unsigned_bin(k, rnd_bytes, num_bytes);
    nbits = pstm_count_bits(k);
    (void)nbits;
    log_info("%hu bit scalar\n", nbits);
    log_info("check: %hu <= 8*%hu - %d == %d)\n",
            nbits,
            curveSize,
            nlz,
            curveSize*8 - nlz);

    return 0;
}

typedef struct
{
    psEccKey_t *key;
    psEccPoint_t *base;
    pstm_int *prime;
    pstm_int *A;
} eccMulmodArgs_st;

static
eccMulmodArgs_st eccmulmod_args_get(
        const psEccCurve_t *curve,
        pstm_int *k)
{
    int32_t err;
    psSize_t keysize, slen;
    psEccPoint_t *base;
    pstm_int *A = NULL;
    pstm_int *prime;
    psEccKey_t *key;
    eccMulmodArgs_st args = {.key = NULL,
                             .base = NULL,
                             .prime = NULL,
                             .A = NULL};

    base = NULL;
    psEccNewKey(NULL, &key, curve);
    psEccInitKey(NULL, key, curve);
    keysize = curve->size;
    slen = keysize * 2;

    if (key->curve->isOptimized == 0)
    {
        if ((A = psMalloc(NULL, sizeof(pstm_int))) == NULL)
        {
            err = PS_MEM_FAIL;
            goto out;
        }
        if (pstm_init_for_read_unsigned_bin(NULL, A, keysize) < 0)
        {
            err = PS_MEM_FAIL;
            goto out;
        }
        if ((err = pstm_read_radix(NULL, A, key->curve->A, slen, 16))
            != PS_SUCCESS)
        {
            goto out;
        }
    }

    prime = psMalloc(NULL, sizeof(pstm_int));

    if (pstm_init_for_read_unsigned_bin(NULL, prime, keysize) < 0)
    {
        err = PS_MEM_FAIL;
        goto out;
    }

    base = eccNewPoint(NULL, prime->alloc);
    if (base == NULL)
    {
        err = PS_MEM_FAIL;
        goto out;
    }

    /* read in the specs for this key */
    if ((err = pstm_read_radix(NULL, prime, key->curve->prime, slen, 16))
        != PS_SUCCESS)
    {
        goto out;
    }
    if ((err = pstm_read_radix(NULL, &base->x, key->curve->Gx, slen, 16))
        != PS_SUCCESS)
    {
        goto out;
    }
    if ((err = pstm_read_radix(NULL, &base->y, key->curve->Gy, slen, 16))
        != PS_SUCCESS)
    {
        goto out;
    }
    pstm_set(&base->z, 1);

    /* copy the scalar */
    err = pstm_init_copy(NULL, &key->k, k, 0);
    if (err != PS_SUCCESS)
    {
        goto out;
    }

    /* make the public key */
    if (pstm_init_size(NULL, &key->pubkey.x, (key->k.used * 2) + 1) < 0)
    {
        err = PS_MEM_FAIL;
        goto out;
    }
    if (pstm_init_size(NULL, &key->pubkey.y, (key->k.used * 2) + 1) < 0)
    {
        err = PS_MEM_FAIL;
        goto out;
    }
    if (pstm_init_size(NULL, &key->pubkey.z, (key->k.used * 2) + 1) < 0)
    {
        err = PS_MEM_FAIL;
        goto out;
    }

    args.key = key;
    args.base = base;
    args.prime = prime;
    args.A = A;

out:
    return args;
}

void eccmulmod_args_clear(eccMulmodArgs_st *args)
{
    eccFreePoint(args->base);
    pstm_clear(args->prime);
    pstm_clear(args->A);
    psFree(args->A, NULL);
    psEccClearKey(args->key);
}

static
int scalar_mult(eccMulmodArgs_st *args)
{
    int rc;

    rc = eccMulmod(NULL,
            &args->key->k,
            args->base,
            &args->key->pubkey,
            args->prime,
            1,
            args->A);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    return 0;
}

static
int iterate(
        const psEccCurve_t *curve,
        pstm_int *scalar,
        int num_iters,
        int nlz)
{
    int i, rc;
    eccMulmodArgs_st args;

    args = eccmulmod_args_get(curve, scalar);

    for (i = 0; i < num_iters; i++)
    {
        rc = scalar_mult(&args);
        FAIL_FUNC_IF(rc < 0, rc, "scalar_mult", return -1);
    }

    eccmulmod_args_clear(&args);
    return 0;
}

int main(int argc, char **argv)
{
    pstm_int k;
    int i, num_iters;
    const psEccCurve_t *curve;
    int32_t rc;
    const char *curvename;
    /* Use symmetric sequence to mitigate effect of hot/cold cache. */
    int nlz[] = {0, 8, 16, 24, 24, 16, 8, 0};
    int nlz_count = sizeof(nlz)/sizeof(nlz[0]);
    double res[nlz_count];
    double time_out;
    double maxmin_delta, maxmin_delta_percent;

    memset(&res, 0, sizeof(res));

    if (argc != 2)
    {
        printf("Usage: ./%s num_iterations\n", argv[0]);
        return 1;
    }
    num_iters = atoi(argv[1]);
    curvename = "secp256r1";

    printf("Using %d iterations\n", num_iters);
    printf("clock() precision seems to be %lfs\n",
            get_clock_precision());
    printf("Using curve %s\n", curvename);
# ifdef USE_NON_CONSTANT_TIME_ECC_MULMOD
    printf("Using leaky eccMulmod\n");
# else
    printf("Using constant-time eccMulmod\n");
# endif

    psCryptoOpen(PSCRYPTO_CONFIG);

    rc = getEccParamByName(curvename, &curve);
    FAIL_FUNC_IF(rc < 0, rc, "getEccParamByName", return 1);

    printf("BENCHMARKING...\n");

    for (i = 0; i < nlz_count; i++)
    {
        rc = generate_k(&k, curve->size, nlz[i]);
        FAIL_FUNC_IF(rc < 0, rc, "generate_k", return 1);
        log_info("nlz == %2d: ", nlz[i]);
        BENCHMARK(iterate(curve, &k, num_iters, nlz[i]));
        if (res[i] == 0.0f)
        {
            res[i] = time_out;
        }
        else
        {
            res[i] = (res[i] + time_out)/2;
        }
    }

    for (i = nlz_count/2; i < nlz_count; i++)
    {
        printf("nlz == %2d: took %lfs\n",
                nlz[i],
                res[i]);
    }
    maxmin_delta = res[nlz_count/2] - res[nlz_count - 1];
    maxmin_delta_percent = (maxmin_delta / res[nlz_count/2]) * 100;

    printf("Delta between largest and smallest scalar: %2.0lf (%lfs)\n",
            maxmin_delta_percent,
            maxmin_delta);

    return 0;
}

# endif /* USE_MATRIX_ECC */
