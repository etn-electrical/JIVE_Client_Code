/* Simple certificate generation class and invocation utility. */

# include "cryptoApi.h"
# include <string>
# include <vector>
# include <iostream>
# include <fstream>

# define logf(...) printf(__VA_ARGS__)
# define log_func_fail(called, rc)                      \
  do                                                    \
    {                                                   \
      logf("%s failed with return value %d in %s\n",    \
          called, rc, __func__);                        \
    }                                                   \
  while (0)

class Certificate
{
public:
  Certificate()
  {
    issuerCert = NULL;
    cert = NULL;
    init();
  }
  ~Certificate()
  {
    if (cert)
      {
        psX509FreeCert(cert);
      }
  }

  int setVersion(int version)
  {
    return psX509SetVersion(NULL,
        &conf,
        version == 1 ? x509_cert_v1 :
        version == 2 ? x509_cert_v2 :
        version == 3 ? x509_cert_v3 : x509_cert_v_default);
  }

  int setSerialNumber(const std::vector<unsigned char> &serialNumber)
  {
    return psX509SetSerialNum(NULL,
        &conf,
        (unsigned char*)&serialNumber[0],
        serialNumber.size());
  }

  int setValidity(std::string notBefore,
      std::string notAfter)
  {
    int rc = PS_SUCCESS;

    rc = psX509SetValidNotBefore(NULL,
        &conf,
        notBefore.c_str(),
        strlen(notBefore.c_str()),
        ASN_UTCTIME);

    rc |= psX509SetValidNotAfter(NULL,
        &conf,
        notAfter.c_str(),
        strlen(notAfter.c_str()),
        ASN_UTCTIME);

    return rc;
  }

  int setSubjectDNAttribute(std::string attribute,
      std::string value)
  {
    return psX509SetDNAttribute(NULL,
        &conf,
        attribute.c_str(),
        strlen(attribute.c_str()),
        value.c_str(),
        strlen(value.c_str()),
        ASN_UTF8STRING);
  }

  int setBasicConstraintsCA(bool isCA)
  {
    return psX509SetBasicConstraintsCA(NULL,
        &conf,
        isCA ? 1 : 0);
  }

  int setKeyUsageBit(std::string bitName, bool value)
  {
    if (value)
      {
        return psX509AddKeyUsageBit(NULL,
            &conf,
            bitName.c_str());
      }
    else
      {
        return PS_SUCCESS;
      }
  }

  int setSubjectAltName(std::string rfc822Name,
      std::string otherNameOIDDotNotation,
      std::string ipAddress,
      std::string dnsName)
  {
    subjectAltNameEntry_t an;

    memset(&an, 0, sizeof(an));
    an.rfc822Name = (char*)rfc822Name.c_str();
    an.rfc822NameLen = strlen(an.rfc822Name);
    an.otherNameOIDDotNotation = (char*)otherNameOIDDotNotation.c_str();
    an.otherNameOIDDotNotationLen = strlen(an.otherNameOIDDotNotation);
    an.iPAddress = (char*)ipAddress.c_str();
    an.iPAddressLen = strlen(an.iPAddress);
    an.dNSName = (char*)dnsName.c_str();
    an.dNSNameLen = strlen(an.dNSName);

    return psX509AddSubjectAltName(NULL,
        &conf,
        &an);
  }

  void setSelfSigned(bool isSelfSigned)
  {
    selfSigned = isSelfSigned;
  }

  int setPublicKey(psPubKey_t *pubKey)
  {
    return psX509SetPublicKey(NULL,
        &conf,
        &privKey);
  }

  int loadPrivKeyFromFile(const std::string &filename)
  {
    std::string pemSuffix = ".pem";
    int isPem = 0;
    int type = 0;

    if (filename.find(pemSuffix) != std::string::npos)
      {
        isPem = 1;
      }

    type = psParseUnknownPrivKey(NULL,
        isPem,
        (char *)filename.c_str(),
        NULL,
        &privKey);
    if (type < 0)
      {
        return type;
      }
    privKey.type = type;
  }

  std::vector<unsigned char> getDER()
  {
    int rc;

    if (!haveDer)
      {
        rc = makeDER();
        if (rc < 0)
          {
            log_func_fail("makeDER", rc);
            return std::vector<unsigned char>();
          }
      }
    return derOctets;
  }
  int setIssuerCert(Certificate *ca)
  {
    issuerCert = ca;
    if (issuerCert->cert == NULL)
      {
        issuerCert->makePsX509Cert();
      }
  }
private:
  void init()
  {
    memset(&conf, 0, sizeof(conf));
  }
  int makeDER()
  {
    unsigned char *out;
    int32_t outLen;
    int32_t rc;
    int32_t i;

    if (selfSigned)
      {
        rc = psX509WriteSelfSignedCertMem(NULL,
            &conf,
            &privKey,
            &out,
            &outLen);
        if (rc < 0)
          {
            log_func_fail("psX509WriteSelfSignedCertMem", rc);
            return rc;
          }
      }
    else
      {
        unsigned char *req;
        int32_t reqLen;
        x509DNattributes_t *DN;
        x509v3extensions_t *ext;
        psPubKey_t *reqPubKey;
        psCertReq_t *parsedReq;

        rc = psWriteCertReqMem(NULL,
            &privKey,
            &conf,
            &req,
            &reqLen);
        if (rc < 0)
          {
            log_func_fail("psWriteCertReqMem", rc);
            return rc;
          }

        rc = psParseCertReqBufExt(NULL,
            req,
            reqLen,
            &DN,
            &reqPubKey,
            &ext,
            &parsedReq);
        if (rc < 0)
          {
            log_func_fail("psParseCertReqBuf", rc);
            return rc;
          }
        free(req);

        rc = psX509WriteCAIssuedCertMem(NULL,
            &conf,
            reqPubKey,
            DN->dnenc,
            DN->dnencLen,
            issuerCert->cert,
            &issuerCert->privKey,
            &out,
            &outLen);
        if (rc < 0)
          {
            log_func_fail("psX509WriteCAIssuedCertMem", rc);
            return rc;
          }
      }
    for (i = 0; i < outLen; i++)
      {
        derOctets.push_back(out[i]);
      }
    free(out);
    return PS_SUCCESS;
  }
  int makePsX509Cert()
  {
    int32_t rc;

    if (!haveDer)
      {
        rc = makeDER();
        if (rc < 0)
          {
            log_func_fail("makeDER", rc);
            return rc;
          }
      }
    rc = psX509ParseCert(NULL,
        (const unsigned char*)&derOctets[0],
        derOctets.size(),
        &cert,
        CERT_STORE_DN_BUFFER);
    if (rc < 0)
      {
        log_func_fail("psX509ParseCert", rc);
        return rc;
      }
  }

  Certificate *issuerCert;
  psPubKey_t privKey;
  psCertConfig_t conf;
  psX509Cert_t *cert;
  std::vector<unsigned char> derOctets;
  bool haveDer;
  bool selfSigned;
};

# define CALL(...) \
  do                                                                    \
    {                                                                   \
      int rc = __VA_ARGS__;                                             \
      if (rc < 0)                                                       \
        {                                                               \
          std::cout << "Something went wrong with ";                    \
          std::cout << #__VA_ARGS__ << std::endl;                       \
            std::cout << " Got ret val " << rc << std::endl;            \
        }                                                               \
    }                                                                   \
  while(0);

Certificate createStaticSelfSigned()
{
  Certificate cert;

  CALL(cert.loadPrivKeyFromFile("../../testkeys/RSA/2048_RSA_CA_KEY.pem"));
  CALL(cert.setSerialNumber(std::vector<unsigned char>{1,2,3}));
  CALL(cert.setVersion(3));
  CALL(cert.setValidity("19101017000000Z", "29101017000000Z"));
  CALL(cert.setSubjectDNAttribute("commonName", "erkki.esimerkki.com"));
  CALL(cert.setSubjectDNAttribute("country", "Finland"));
  CALL(cert.setSubjectDNAttribute("organization", "Esimerkki Oy"));
  CALL(cert.setSubjectDNAttribute("organizationalUnit", "R&D"));
  cert.setSelfSigned(true);
  CALL(cert.setBasicConstraintsCA(true));
  CALL(cert.setKeyUsageBit("keyCertSign", true));

  return cert;
}

std::string getAnswer(std::string prompt, std::string defaultValue)
{
  std::string answer;

  std::cout << prompt << " (default: " << defaultValue << ")";
  std::getline(std::cin, answer);
  if (answer == "")
    {
      answer = defaultValue;
    }

  std::cout << "  Value: " << answer << std::endl;

  return answer;
}

int getAnswerInt(std::string prompt)
{
  int answer;

  std::cout << prompt << " ";
  std::cin >> answer;
  std::cout << "\n";
  return answer;
}

Certificate createSelfSigned()
{
  Certificate cert;

  /* Use static serial number for now. */
  CALL(cert.setSerialNumber(std::vector<unsigned char>{1,2,3}));

  CALL(cert.setSubjectDNAttribute("commonName",
          getAnswer("commonName:", "root.test.com")));
  CALL(cert.setSubjectDNAttribute("country",
          getAnswer("country:", "Finland")));
  CALL(cert.setSubjectDNAttribute("organization",
          getAnswer("organization:", "Test Organization")));
  CALL(cert.setSubjectDNAttribute("organizationalUnit",
          getAnswer("organizationalUnit:", "R&D")));
  CALL(cert.setValidity(getAnswer("notBefore:", "19101017000000Z"),
          getAnswer("notAfter:", "29101017000000Z")));
  CALL(cert.loadPrivKeyFromFile(getAnswer("Private key filename:",
              "../../testkeys/RSA/2048_RSA_CA_KEY.pem")));
  cert.setSelfSigned(true);
  CALL(cert.setBasicConstraintsCA(true));
  CALL(cert.setKeyUsageBit("keyCertSign", true));

  return cert;
}

Certificate createCASigned(Certificate *ca, bool isLeafCert)
{
  Certificate cert;

  cert.setIssuerCert(ca);
  cert.setSelfSigned(false);

  CALL(cert.loadPrivKeyFromFile(getAnswer("Private key filename:",
              "../../testkeys/RSA/2048_RSA_KEY.pem")));
  CALL(cert.setBasicConstraintsCA(!isLeafCert));

  /* Use static serial number for now. */
  CALL(cert.setSerialNumber(std::vector<unsigned char>{1,2,3}));

  CALL(cert.setSubjectDNAttribute("commonName",
          getAnswer("commonName:", "child.test.com")));
  CALL(cert.setSubjectDNAttribute("country",
          getAnswer("country:", "Finland")));
  CALL(cert.setSubjectDNAttribute("organization",
          getAnswer("organization:", "Test Organization")));
  CALL(cert.setSubjectDNAttribute("organizationalUnit",
          getAnswer("organizationalUnit:", "R&D")));
  CALL(cert.setValidity(getAnswer("notBefore:", "19101017000000Z"),
          getAnswer("notAfter:", "29101017000000Z")));

  return cert;
}

void writeCertToFile(Certificate cert, std::string filename)
{
  std::vector<unsigned char> der;

  der = cert.getDER();

  psTraceBytes("Generated DER", &der[0], der.size());

  std::ofstream outFile(filename, std::ios::out | std::ios::binary);

  outFile.write(reinterpret_cast<const char*>(&der[0]), der.size());
  outFile.close();

  std::cout << "Wrote: " << filename << std::endl;
  std::cout << "Check with: " << std::endl;
  std::cout << " openssl x509 -text -noout -inform DER -in " << filename << std::endl;
}

int main(int argc, char **argv)
{
  Certificate caCert;
  Certificate childCert;
  psPubKey_t pubKey;
  std::string in;

  in = getAnswer("Create a static self-signed cert (Y/N)?", "N");
  if (in == "Y" || in == "y" )
    {
      caCert = createStaticSelfSigned();
      writeCertToFile(caCert, "static_root.der");
    }
  else
    {
      in = getAnswer("Create a self-signed root cert (Y/N)?", "Y");
      if (in == "Y" || in == "y")
        {
          caCert = createSelfSigned();
          writeCertToFile(caCert, "root.der");
          in = getAnswer("Add another cert to the chain? (Y/N)", "Y");
          if (in == "Y" || in == "y" )
            {
              bool isLeaf = false;

              in = getAnswer("Is this a leaf certificate (Y/N)", "Y");
              if (in == "Y" || in == "y")
                {
                  isLeaf = true;
                }

              childCert = createCASigned(&caCert, isLeaf);
              writeCertToFile(childCert, "child.der");
            }
        }
      else
        {
          std::cout << "TODO: implement CA-signed cert generation\n";
          return EXIT_FAILURE;
        }
    }

  return 0;
}
