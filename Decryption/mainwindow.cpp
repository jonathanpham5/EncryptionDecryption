#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "encryptionGenerator.h"

#define KEY_LENGTH  2048
#define PUB_EXP     3

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
    ui->detailsBox->append("Decrypting your message using private key to decipher text.");
    ui->detailsBox->append("Decryption process:");
    ui->detailsBox->append("P = D(PRVK,E(PBLK,P))");
    ui->detailsBox->append("D = Decryption method, PBLK = receiver's Public Key, PRVK = receiver's Private Key and P = Plain Text");
    ui->detailsBox->append("...\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_decryptButton_clicked()
{
    char *decrypt = NULL;		// Decrypted message
    char *err = NULL;			// Buffer for any error messages
    RSA *priKey = NULL;
    bool doDecrypt;

    OpenSSL_add_all_algorithms();

    if(ui->fileNameBox->text().length() == 0)
        doDecrypt = false;
    else {
        if(fileExists(ui->fileNameBox->text()))
            doDecrypt = true;
        else
            QMessageBox::information(0, "FILE DOESN'T EXIST!", "Please re-enter a valid file!");
    }

    if(fileExists("private.pem") && doDecrypt)
    {
        BIO *priBio;

        QString privateKey = "";
        std::string key = "";

        QString priFileName = "private.pem";

        QFile priFile(priFileName);
        if(priFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&priFile);
            privateKey = stream.readAll();
        }

        key = privateKey.toStdString();
        char *prKey = new char[key.length() + 1];
        strcpy(prKey, key.c_str());

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

        ui->detailsBox->append("*** Starting Decryption of Text ***\n");
        delay(1);

        ui->detailsBox->append("Setting up decryption...\n");
        delay(1);

        err = (char*)malloc(130);

        std::ifstream in(ui->fileNameBox->text().toStdString());
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        ui->detailsBox->append("Executing Decryption...\n");
        delay(1);

        // Decrypt the message
        int encrypt_len = strlen(contents.c_str());
        decrypt = (char *)malloc(encrypt_len);
        if (RSA_private_decrypt(encrypt_len, (unsigned char*)contents.c_str(), (unsigned char*)decrypt, priKey, RSA_PKCS1_OAEP_PADDING) == -1) {
            ERR_load_crypto_strings();
            ERR_error_string(ERR_get_error(), err);
            fprintf(stderr, "Error decrypting message: %s\n", err);
        }

        ui->detailsBox->append("Decryption Process finished!\n");
        ui->detailsBox->append("*** Printing out Decrypted Message ***\n");
        delay(1);
        ui->detailsBox->append(decrypt);

    }
    else
    {
        QMessageBox::information(0, "KEY FILE DOES EXIST!", "Please make sure private.pem is in your directory!");
    }

}


void MainWindow::on_pushButton_clicked()
{
    ui->detailsBox->clear();
}
