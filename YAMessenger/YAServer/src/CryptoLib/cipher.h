#pragma once
#include "stdafx.h"
namespace Crypto{

//typedef  std::unique_ptr<RSA, decltype(&::RSA_free)> ptrRSA;
//typedef  std::unique_ptr<BIO, decltype(&::BIO_free)> ptrBIO;

class Cipher
{

    using ptrRSA = std::unique_ptr<RSA, decltype(&::RSA_free)>;
    using ptrBIO = std::unique_ptr<BIO, decltype(&::BIO_free)>;

public:
    Cipher();
    ~Cipher();

    /**
     * @brief GetPublicKey
     * @return PEM encoded Public key
     */
    QByteArray GetPublicKey();

    /**
     * @brief Encrypt data used external Public key
     * @param referance on data for encrypt
     * @return encrypted data
     */
    QByteArray EncryptData(QByteArray publicKey, QByteArray &data);

    /**
     * @brief DecryptData
     * @param referance on data for decrypt
     * @return decrypted data
     */
    QByteArray DecryptData(QByteArray &data);

private:

    // RSA key pair

    ptrRSA m_PublicKey;
    ptrRSA m_PrivateKey;

};

}
