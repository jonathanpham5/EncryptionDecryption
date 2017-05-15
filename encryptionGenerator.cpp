//Encryption generator for Cyber Security group project
//Group Members: David Andrews, Hani Alghamdi, Jonathon Pham
//

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

namespace generator
{
    bool generate_key()
    {
        int ret = 0;
        RSA *r = NULL;
        BIGNUM *bne = NULL;
        BIO *bp_public = NULL, *bp_private = NULL;

        int bits = 2048;
        unsigned long   e = RSA_F4;

        // 1. generate rsa key
        bne = BN_new();
        ret = BN_set_word(bne,e);
        if(ret != 1){
            BIO_free_all(bp_public);
            BIO_free_all(bp_private);
            RSA_free(r);
            BN_free(bne);
        }

        r = RSA_new();
        ret = RSA_generate_key_ex(r, bits, bne, NULL);
        if(ret != 1){
            BIO_free_all(bp_public);
            BIO_free_all(bp_private);
            RSA_free(r);
            BN_free(bne);
        }

        // 2. save public key
        bp_public = BIO_new_file("public.pem", "w+");
        ret = PEM_write_bio_RSAPublicKey(bp_public, r);
        if(ret != 1){
            BIO_free_all(bp_public);
            BIO_free_all(bp_private);
            RSA_free(r);
            BN_free(bne);
        }

        // 3. save private key
        bp_private = BIO_new_file("private.pem", "w+");
        ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

        return (ret == 1);
    }

}
