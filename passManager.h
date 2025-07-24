#ifndef PASSMANAGER_H
#define PASSMANAGER_H
#include "security.h"

typedef struct {
    char website[100];
    char username[100];
    char password[100];
} info;

typedef struct {
    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];
    unsigned char recoveryHash[HASH_SIZE];
    unsigned char encryptedKey[HASH_SIZE];
} masterHash;

typedef struct {
    char pass[100];
} master;

typedef struct {
    char time[9];
    char date[12];
    int attempts;
    char notes[100];
    char tags[50];
} log;

void toLowerCase(char* str);
int getValidatedInt(const char* str);

void exitProgram();
int masterPassword();
void storePassword(info **details, int *records);
void readPassword();
void searchPassword(char *search);
void deletePassword(char *targetPassword);
void updatePassword(char *targetWebsite);
void readLogs();
void clearLogs();
void nuke();

#endif //PASSMANAGER_H
