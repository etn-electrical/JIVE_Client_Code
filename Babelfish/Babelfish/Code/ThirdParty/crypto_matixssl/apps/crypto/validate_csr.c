# include "crypto/cryptoApi.h"

int main(int argc, char **argv)
{
# ifndef USE_CERT_GEN
    fprintf(stderr, "Error: need #define USE_CERT_GEN\n");
    return EXIT_FAILURE;
# else
    int32 rc;
    char *filename;
    unsigned char *csr = NULL;
    int32 csr_len;
    x509DNattributes_t *subjectDN = NULL;
    psPubKey_t *reqPubKey = NULL;
    x509v3extensions_t *reqExt = NULL;
    psCertReq_t *parsedReq = NULL;

    psCryptoOpen(PSCRYPTO_CONFIG);

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        rc = 1;
        goto out;
    }

    filename = argv[1];

    printf("Verifying %s...\n", filename);
    rc = psParseCertReqFile(NULL,
            filename,
            &csr,
            &csr_len);
    if (rc < 0)
    {
        fprintf(stderr, "psParseCertReqFile failed\n");
        rc = 1;
        goto out;
    }

    rc = psParseCertReqBufExt(NULL,
            csr,
            csr_len,
            &subjectDN,
            &reqPubKey,
            &reqExt,
            &parsedReq);
    if (rc < 0)
    {
        fprintf(stderr, "psParseCertReqBufExt failed\n");
        rc = 1;
        goto out;
    }

    printf("OK\n");
    printf("CN: %s\n", subjectDN->commonName);

    rc = 0;

out:

    psFree(parsedReq, NULL);
    x509FreeExtensions(reqExt);
    psDeletePubKey(&reqPubKey);
    if (subjectDN != NULL)
    {
        psX509FreeDNStruct(subjectDN, NULL);
    }
    free(csr);
    return rc;
# endif
}
