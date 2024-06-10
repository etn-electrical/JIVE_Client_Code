#include "osdep_stdio.h"
#include "osdep_stdlib.h"
#include "crypto/cryptoApi.h"
#include "crypto/keyformat/asn1.h"

#if (defined USE_X509 && defined USE_FULL_CERT_PARSE)

#define MAX_BUF 65536

/* Allow der-decoding to be processed as a standalone C compilation. */
int main(void)
{
    char *str;
    size_t len;
    unsigned char der[MAX_BUF];

    len = Fread(der, 1, MAX_BUF, stdin);
    if (len <= 0)
    {
        Fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    str = asnFormatDer(NULL, der, len, 0, 0, 64, 0);
    
    if (str)
    {
        Printf("%s\n", str);
    }
    else
    {
        Printf("(null)\n");
    }
    psFree(str, NULL);
    return 0;
}

#else

int main(void)
{
    Fprintf(stderr,
            "DER parse not supported: change Matrix configuration options.\n");
    exit(1);
}

#endif
