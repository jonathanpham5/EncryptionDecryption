/*
Author: Jonathan Pham
File: main.cpp
Key Encryption
*/
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "encryptionGenerator.h"

#define KEY_LENGTH  2048
#define PUB_EXP     3

// generate and save private key at private.pem
void savePrivateKey(char *keypair){
  FILE* pFile;

  if (pFile = fopen("private.pem", "wb"))
  {
    fprintf(pFile, "%s", keypair);
  }
  else{
    printf("PEM_write_PrivateKey failed.\n");
  }

  if (pFile)
  {
    fclose(pFile);
    pFile = NULL;
  }
}
 // generate and save public key at public.pem
void savePublicKey(char *keypair){
  FILE* pFile;

  if (pFile = fopen("public.pem", "wb"))
  {
    fprintf(pFile, "%s", keypair);
  }
  else{
    printf("PEM_write_PublicKey failed.\n");
  }

  if (pFile)
  {
    fclose(pFile);
    pFile = NULL;
  }
}


int main(int argc, char* argv[])
{
  bool generatorHolder;
  generatorHolder = generator::generate_key();
  std::string fileName = "";
  //std::string msg = "";

  size_t pri_len;								// Length of private key
  size_t pub_len;								// Length of public key
  char   *pri_key;							// Private key
  char   *pub_key;           		// Public key
  char   msg[KEY_LENGTH / 8];		// Message to encrypt
  char   *encrypt = NULL;				// Encrypted message
  char   *decrypt = NULL;				// Decrypted message
  char   *err = NULL;						// Buffer for any error messages
  char   name[256];
  char   rand_buff[16];
  char   *private_key = NULL;		// Private key
  char   *public_key = NULL;		// Public key

  OpenSSL_add_all_algorithms();

  // Generate key pair
  RSA *keypair = RSA_generate_key(KEY_LENGTH, PUB_EXP, NULL, NULL);

  // To get the C-string PEM form:
  BIO *pri = BIO_new(BIO_s_mem());
  BIO *pub = BIO_new(BIO_s_mem());

  // extracting the Public/Private key into a BIO variable called pub/pri
  PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
  PEM_write_bio_RSAPublicKey(pub, keypair);

  pri_len = BIO_pending(pri);
  pub_len = BIO_pending(pub);

  pri_key = (char *)malloc(pri_len + 1);
  pub_key = (char *)malloc(pub_len + 1);

  BIO_read(pri, pri_key, pri_len);
  BIO_read(pub, pub_key, pub_len);

  pri_key[pri_len] = '\0';
  pub_key[pub_len] = '\0';

  savePrivateKey(pri_key);
  savePublicKey(pub_key);

  std::cout << "\n\033[1mCSCI 4800 Cybersecurity peoject\033[0m\n\n";
  std::cout << "This is a Public Key Encryption software example. A new pair of public/private keys were generated for you at:\n";
  std::cout << "\tpublic.pem\n";
  std::cout << "\tprivate.pem\n\n";

  // Command Line Arguments are 2: ./a.out <insert file name here>
  if (argc == 2) {
    std::string cmdline(argv[1]);
    std::cout << "File name you entered to encryprt:\n";
    std::cout << cmdline << std::endl;

    // Encrypt the file here

    // Decrypt the file here

    // Encrypt buffer.str() here

  } else {
    // Encrypt text from command line
    std::cout << " >> Enter a message to be encrypted: ";
    fgets(msg, KEY_LENGTH - 1, stdin);
    msg[strlen(msg) - 1] = '\0';

    std::cout << "\n\n Encrypting your message using the public key to generate a cipher text.\n ";
    std::cout << "\t Encryption process\n";
    std::cout << "\t C = E(PBLK,P)\n";
    std::cout << "\t C : cipher text\n";
    std::cout << "\t E : Encryption method\n";
    std::cout << "\t PBLK : reciever's Public key\n";
    std::cout << "\t P : Plain text\n";
    std::cout << "\n ********* Starting encrypted message ****************** \n\n";

    // Encrypt the message
    encrypt = (char *)malloc(RSA_size(keypair));
    int encrypt_len;
    err = (char *)malloc(130);
    if ((encrypt_len = RSA_public_encrypt(strlen(msg) + 1, (unsigned char*)msg, (unsigned char*)encrypt, keypair, RSA_PKCS1_OAEP_PADDING)) == -1) {
      ERR_load_crypto_strings();
      ERR_error_string(ERR_get_error(), err);
      fprintf(stderr, "Error encrypting message: %s\n", err);

    }
    // display encrypted text
    std::cout << encrypt;
    std::cout << "\n ********* End of encrypted message ****************** \n";
    std::cout << "\n Now we decrypt the cipher text using the private key\n";
    std::cout << "\t P = D(PRVK,E(PBLK,P))\n";
    std::cout << "\t D : decryption method\n";
    std::cout << "\t PRVK : reciever's Private key\n\n";

    // Decrypt the message
    decrypt = (char *)malloc(encrypt_len);
    if (RSA_private_decrypt(encrypt_len, (unsigned char*)encrypt, (unsigned char*)decrypt,
    keypair, RSA_PKCS1_OAEP_PADDING) == -1) {
      ERR_load_crypto_strings();
      ERR_error_string(ERR_get_error(), err);
      fprintf(stderr, "Error decrypting message: %s\n", err);

    }

    std::cout << "Decrypted message: ";
    std::cout << decrypt << std::endl;

    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
    free(pri_key);
    free(pub_key);
    free(encrypt);
    free(decrypt);
    free(err);

  }

  return 0;
}
