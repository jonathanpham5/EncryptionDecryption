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
#include <Qtime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "encryptionGenerator.h"

#define KEY_LENGTH  2048
#define PUB_EXP     3
#define PRIFILE     "private.pem"
#define PUBFILE     "public.pem"

bool generated = false;

void delay(int secs)
{
    QTime dieTime= QTime::currentTime().addSecs(secs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return (check_file.exists() && check_file.isFile());
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->detailsBox->append("CSCI 4800 Cybersecurity project");
    ui->detailsBox->append("A new pair of public/private keys were generated for you at:");
    ui->detailsBox->append("public.pem and private.pem");
    ui->detailsBox->append("Encrypting your message using the public key to generate cipher text.");
    ui->detailsBox->append("Encryption process:");
    ui->detailsBox->append("C = E(PBLK, P)");
    ui->detailsBox->append("C = Cipher Text, E = Encryption method, PBLK = receiver's Public Key and P = Plain Text");
    ui->detailsBox->append("...\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_pushButton_clicked()
{
    QString text = ui->encryptText->toPlainText();
    std::string message = text.toStdString();
    char *msg = new char[message.length() + 1];	// Message to encrypt
    strcpy(msg, message.c_str());

    char *encrypt = NULL;		// Encrypted message
    char *decrypt = NULL;		// Generate Key Pair
    char *err = NULL;			// Buffer for any error messages
    RSA *priKey = NULL;
    RSA *pubKey = NULL;

    OpenSSL_add_all_algorithms();

    if(fileExists("public.pem") && fileExists("private.pem"))
    {
        BIO *priBio;
        BIO *pubBio;

        QString privateKey = "";
        QString publicKey = "";
        std::string key = "";

        QString priFileName = "private.pem";
        QString pubFileName = "public.pem";

        QFile priFile(priFileName);
        if(priFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&priFile);
            privateKey = stream.readAll();
        }

        QFile pubFile(pubFileName);
        if(pubFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&pubFile);
            publicKey = stream.readAll();
        }


        key = privateKey.toStdString();
        char *prKey = new char[key.length() + 1];
        strcpy(prKey, key.c_str());

        key = publicKey.toStdString();
        char *puKey = new char[key.length() + 1];
        strcpy(puKey, key.c_str());


        priBio = BIO_new_mem_buf(prKey, -1);
        if (priBio==NULL)
        {
            printf( "Failed to create key BIO");
            QApplication::quit();
        }
        priKey = PEM_read_bio_RSAPrivateKey(priBio, &priKey,NULL, NULL);
        if(priKey == NULL)
        {
            printf( "Failed to create RSA");
        }

        pubBio = BIO_new_mem_buf(puKey, -1);
        if (pubBio==NULL)
        {
            printf( "Failed to create key BIO");
            QApplication::quit();
        }
        pubKey = PEM_read_bio_RSAPublicKey(pubBio, &pubKey,NULL, NULL);
        if(pubKey == NULL)
        {
            printf( "Failed to create RSA");
        }

        ui->detailsBox->append("*** Starting Encryption of Text ***\n");
        delay(1);

        ui->detailsBox->append("Setting up encryption...\n");
        encrypt = (char *)malloc(RSA_size(pubKey));
        int encrypt_len;
        err = (char*)malloc(130);

        ui->detailsBox->append("Executing encryption...\n");
        delay(1);
        if ((encrypt_len = RSA_public_encrypt((int)strlen(msg) + 1, (unsigned char*)msg, (unsigned char*)encrypt, pubKey, RSA_PKCS1_OAEP_PADDING)) == -1)
        {
              ERR_load_crypto_strings();
              ERR_error_string(ERR_get_error(), err);
              fprintf(stderr, "Error encrypting message: %s\n", err);

        }

        ui->detailsBox->append("Encryption Process finished!\n");
        delay(1);
        ui->detailsBox->append("*** Printing out Encrypted Message ***\n");
        delay(1);
        ui->detailsBox->append(encrypt);

        QFile outFile("encrypted.txt");
        if(outFile.open(QIODevice::WriteOnly)) {
            qint64 bytesWritten = outFile.write(reinterpret_cast<const char*>(encrypt), encrypt_len);
            if(bytesWritten < encrypt_len) {
                ui->detailsBox->append("File Write Failed!");
            }
        }
        outFile.close();

        std::ifstream in("encrypted.txt");
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        // Decrypt the message
        decrypt = (char *)malloc(encrypt_len);
        if (RSA_private_decrypt(encrypt_len, (unsigned char*)contents.c_str(), (unsigned char*)decrypt, priKey, RSA_PKCS1_OAEP_PADDING) == -1) {
            ERR_load_crypto_strings();
            ERR_error_string(ERR_get_error(), err);
            fprintf(stderr, "Error decrypting message: %s\n", err);
        }

        ui->detailsBox->append("\nPrinting out Original Message:\n");
        delay(1);
        ui->detailsBox->append(decrypt);
        ui->detailsBox->append("\n");

    }
    else
    {
        QMessageBox::information(0, "KEY FILES DO NOT EXIST!", "Please click 'Generate Keys' to generate the key files!");
    }

}

void MainWindow::on_fileDialog_clicked()
{
    QString filename=QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "C://",
                "All Files (*.*);;Text File (*.txt)"
                );
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "File Information", file.errorString());

    QTextStream in(&file);

    ui->encryptText->setText(in.readAll());
}

void MainWindow::on_generateKeys_clicked()
{
    size_t pri_len;             // Length of private key
    size_t pub_len;             // Length of public key

    char *pri_key;              // Private Key
    char *pub_key;              // Public Key

    // Generate key pair
    RSA *keypair = RSA_generate_key(KEY_LENGTH, PUB_EXP, NULL, NULL);

    // To get the C-string PEM form:
    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    // extracting the Public/Private Key into a BIO variable called pub/pri
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

    generated = true;

    QMessageBox::information(0, "Keys Generated", "Your keys have been generated successfully!");
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->detailsBox->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->encryptText->clear();
}
