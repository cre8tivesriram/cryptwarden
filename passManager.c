#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>

#include "colors.h"
#include "security.h"
#include "utility.h"
#include "passManager.h"
#include "seed.h"

unsigned char key[HASH_SIZE];
unsigned char iv[IV_SIZE] = {0};
char loginInput[300];

int masterPassword() {
    char temp[100];
    FILE *fp = fopen("master.txt", "rb");

    if (fp == NULL) {
        master mp;
        masterHash hashValue;
        if (!generateSalt(hashValue.salt)) {
            print_colored(COLOR_RED, "Salt generation failed\n");
            return 1;
        }

        printf("\n");
        print_colored(COLOR_CYAN, "Create your master password (Remember it!): \n");
        print_colored(COLOR_RED, "SPACE NOT ALLOWED, TYPE \"EXIT\" TO CLOSE THE MANAGER\n");

        while (1) {
            int spaceFound = 0;
            fgets(temp, 50, stdin);
            temp[strcspn(temp, "\n")] = '\0';
            toLowerCase(temp);
            if (strcmp(temp, "exit") == 0) {
                exitProgram();
            }

            for (int i = 0; temp[i] != '\0'; i++) {
                if (temp[i] == ' ') {
                    spaceFound = 1;
                    break;
                }
            }

            if (!spaceFound) {
                char confirm[2];
                strcpy(mp.pass, temp);
                print_coloredf(COLOR_RED, "Your master password: %s\n", mp.pass);
                print_colored(COLOR_YELLOW, "Confirm (Y/N): ");
                scanf("%c", confirm);
                clearInputBuffer();
                toLowerCase(confirm);

                if (strcmp(confirm, "y") == 0) {
                    if (!hashPasswordPBKDF2(mp.pass, hashValue.salt, hashValue.hash)) {
                        print_colored(COLOR_RED, "Password hashing failed\n");
                        return 1;
                    }
                    memset(temp, 0, sizeof(temp));

                    fp = fopen("master.txt", "wb");
                    fwrite(&hashValue, sizeof(hashValue), 1, fp);
                    print_colored(COLOR_GREEN, "Your master password has been set. You can relaunch the manager\n");
                    fclose(fp);

                    FILE *logfp = fopen("log.txt", "wb");
                    log genesisEntry;
                    time_t t = time(NULL);
                    struct tm *tm_info = localtime(&t);
                    strftime(genesisEntry.time, 9, "%H:%M:%S", tm_info);
                    strftime(genesisEntry.date, 12, "%Y-%m-%d", tm_info);
                    genesisEntry.attempts = 0;
                    strcpy(genesisEntry.tags, "Initialized");
                    strcpy(genesisEntry.notes, "Master Password Created");
                    fwrite(&genesisEntry, sizeof(genesisEntry), 1, logfp);

                    fclose(logfp);
                    exitProgram();
                    break;
                } else if (strcmp(confirm, "n") == 0) {
                    break;
                }
            } else {
                print_colored(COLOR_RED, "Your password contains a space. Re-enter your password: \n");
            }
        }
    } else {
        int totalAttempts = 3;
        int attempts = 3;
        masterHash stored;
        fread(&stored, sizeof(stored), 1, fp);
        while (1) {
            if (attempts > 0) {
                print_coloredf(COLOR_YELLOW, "%d attempts left\n", attempts);
                print_colored(COLOR_CYAN, "Enter your master password: ");
                fgets(temp, sizeof(temp), stdin);
                removeFgetsNewLine(temp);

                unsigned char verifyHash[HASH_SIZE];
                hashPasswordPBKDF2(temp, stored.salt, verifyHash);
                if (memcmp(verifyHash, stored.hash, HASH_SIZE) == 0) {
                    print_colored(COLOR_GREEN, "Access Granted\n");
                    PKCS5_PBKDF2_HMAC(temp, (int)strlen(temp), stored.salt, SALT_SIZE, ITERATIONS, EVP_sha256(), HASH_SIZE, key);
                    strcpy(loginInput, temp);
                    memset(temp, 0, sizeof(temp));
                    fclose(fp);

                    FILE *logfp = fopen("log.txt", "ab");
                    log entry;
                    time_t t = time(NULL);
                    struct tm *tm_info = localtime(&t);
                    strftime(entry.time, 9, "%H:%M:%S", tm_info);
                    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
                    int tried = totalAttempts - attempts + 1;
                    if (tried == 1) {
                        strcpy(entry.notes, "Logged In");
                    } else if (tried > 1 && tried <= 3) {
                        sprintf(entry.notes, "Logged In @ attempt %d", tried);
                    }
                    entry.attempts = tried;
                    strcpy(entry.tags, "Login");
                    fwrite(&entry, sizeof(entry), 1, logfp);
                    fclose(logfp);
                    return 1;
                }
            }

            attempts--;
            if (attempts <= 0) {
                FILE *logfp = fopen("log.txt", "ab");
                log failEntry;
                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);
                strftime(failEntry.time, 9, "%H:%M:%S", tm_info);
                strftime(failEntry.date, 12, "%Y-%m-%d", tm_info);
                failEntry.attempts = totalAttempts;
                strcpy(failEntry.notes, "FAILED Login Attempt");
                strcpy(failEntry.tags, "Failed Login");
                fwrite(&failEntry, sizeof(failEntry), 1, logfp);
                fclose(logfp);
                fclose(fp);
                return 0;
            }
        }
    }
    return 0;
}


void storePassword(info **details, int *records) {
    int count = 0;
    int logging = 0;
    *details = malloc(sizeof(info));
    if (*details == NULL) {
        print_colored(COLOR_RED,"Memory allocation failed\n");
        return;
    }

    while (1) {
        char check[100];
        print_colored(COLOR_CYAN,"Enter website name (say ""done"" to stop): ");
        scanf("%s", check);
        clearInputBuffer();


        if (strcmp(check, "done") == 0 && count == 0) {
            exitProgram();
        }

        if (strcmp(check, "done") == 0 && count != 0) {
            print_colored(COLOR_GREEN,"Entries Loaded Successfully\n");
            break;
        }

        *details = realloc(*details, (count+1) * sizeof(info));
        if (*details == NULL) {
            print_colored(COLOR_RED,"Memory allocation error");
            break;
        }

        toLowerCase(check);
        strcpy((*details)[count].website, check);
        print_colored(COLOR_CYAN,"Enter username: ");
        fgets((*details)[count].username, sizeof((*details)[count].username), stdin);
        removeFgetsNewLine((*details)[count].username);

        print_colored(COLOR_CYAN,"Enter password: ");
        fgets((*details)[count].password, sizeof((*details)[count].password), stdin);
        removeFgetsNewLine((*details)[count].password);

        count++;
        logging++;
    }
    *records = count;


    FILE *fp = fopen("passwords.txt", "ab");
    if (fp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }
    for (int i = 0; i < *records; i++) {
        unsigned char encrypted[512];

        int encrypted_len = encryptData((unsigned char *)&(*details)[i], sizeof(info), key, iv, encrypted);

        fwrite(&encrypted_len, sizeof(int), 1, fp);
        fwrite(encrypted, 1, encrypted_len, fp);
    }

    print_colored(COLOR_GREEN,"Passwords Stored Successfully\n");
    fclose(fp);


    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    if (logging >= 1) {
        sprintf(entry.notes, "%d password(s) saved", logging);
    }
    strcpy(entry.tags, "Store");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
}

void readPassword() {
    FILE *fp = fopen("passwords.txt", "rb");
    if (fp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        print_colored(COLOR_RED,"File is empty\n");
        fclose(fp);
        return;
    }
    rewind(fp);
    size_t const totalRecords = size / sizeof(info);

    info *totalStructs = malloc(totalRecords * sizeof(info));
    if (totalStructs == NULL) {
        print_colored(COLOR_RED,"Memory allocation error");
        fclose(fp);
        return;
    }

    FILE *masfp = fopen("master.txt", "rb");
    if (masfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
    }
    masterHash stored;
    fread(&stored, sizeof(masterHash), 1, masfp);
    for (size_t i = 0; i < totalRecords; i++) {
        int encrypted_len;
        fread(&encrypted_len, sizeof(int), 1, fp);

        unsigned char encrypted[512];
        fread(encrypted, 1, encrypted_len, fp);

        decryptData(encrypted, encrypted_len, key, iv, (unsigned char *)&totalStructs[i]);
    }

    fclose(masfp);
    fclose(fp);

    print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
    print_coloredf(COLOR_MAGENTA,"%-30s | %-25s | %-25s\n", "WEBSITE", "USERNAME", "PASSWORD");
    print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < totalRecords; i++) {
        print_coloredf(COLOR_BOLD,"%-30s | %-25s | %-25s\n",
            totalStructs[i].website, totalStructs[i].username, totalStructs[i].password);
    }
    free(totalStructs);


    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    strcpy(entry.notes, "Read Passwords");
    strcpy(entry.tags, "Read");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
}


void searchPassword(char *search) {
    info *store;
    FILE *fp = fopen("passwords.txt", "rb");
    if (fp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        print_colored(COLOR_RED,"File is empty\n");
        fclose(fp);
        return;
    }
    rewind(fp);

    size_t const totalRecords = size / sizeof(info);
    store = malloc(totalRecords * sizeof(info));
    if (store == NULL) {
        print_colored(COLOR_RED,"Memory allocation error");
        fclose(fp);
        return;
    }
    FILE *masfp = fopen("master.txt", "rb");
    if (masfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
    }
    masterHash stored;
    fread(&stored, sizeof(masterHash), 1, masfp);
    for (size_t i = 0; i < totalRecords; i++) {
        int encrypted_len;
        fread(&encrypted_len, sizeof(int), 1, fp);

        unsigned char encrypted[512];
        fread(encrypted, 1, encrypted_len, fp);

        decryptData(encrypted, encrypted_len, key, iv, (unsigned char *)&store[i]);
    }

    fclose(masfp);
    fclose(fp);

    int totalFound = 0;
    toLowerCase(search);
    for (size_t i = 0; i < totalRecords; i++) {
        if (strstr(store[i].website, search) != NULL) {
            totalFound++;
        }
    }

    if (totalFound >= 1) {
        print_coloredf(COLOR_CYAN,"%d credential(s) found:\n", totalFound);
        print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
        print_coloredf(COLOR_MAGENTA,"%-30s | %-25s | %-25s\n", "WEBSITE", "USERNAME", "PASSWORD");
        print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
        for (size_t i = 0; i < totalRecords; i++) {
            if (strstr(store[i].website, search) != NULL) {
                print_coloredf(COLOR_BOLD,"%-30s | %-25s | %-25s\n", store[i].website, store[i].username, store[i].password);
            }
        }
    }

    else {
        print_colored(COLOR_RED,"Website not found in the manager. Try adding.\n");
        free(store);
        return;
    }
    free(store);


    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    strcpy(entry.notes, "Search Password");
    strcpy(entry.tags, "Search");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
}

void deletePassword(char *targetWebsite) {
    info *store;
    FILE *fp = fopen("passwords.txt", "rb");
    if (fp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        print_colored(COLOR_RED, "File is empty\n");
        fclose(fp);
        return;
    }
    rewind(fp);
    size_t totalRecords = size / sizeof(info);
    store = malloc(totalRecords * sizeof(info));


    FILE *masfp = fopen("master.txt", "rb");
    if (masfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
    }
    masterHash stored;
    fread(&stored, sizeof(masterHash), 1, masfp);
    for (size_t i = 0; i < totalRecords; i++) {
        int encrypted_len;
        fread(&encrypted_len, sizeof(int), 1, fp);

        unsigned char encrypted[512];
        fread(encrypted, 1, encrypted_len, fp);

        decryptData(encrypted, encrypted_len, key, iv, (unsigned char *)&store[i]);
    }
    fclose(masfp);
    fclose(fp);

    int found = 0;
    size_t newCount = 0;
    toLowerCase(targetWebsite);


    int totalFound = 0;
    for (size_t i = 0; i < totalRecords; i++) {
        if (strstr(store[i].website, targetWebsite) != NULL) {
            totalFound++;
        }
    }

    if (totalFound == 0) {
        print_colored(COLOR_RED,"Website not found in the manager. Try adding.\n");
        free(store);
        return;
    }

    print_coloredf(COLOR_CYAN,"%d credential(s) found:\n", totalFound);
    print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
    print_coloredf(COLOR_MAGENTA,"%-30s | %-25s | %-25s\n", "WEBSITE", "USERNAME", "PASSWORD");
    printf("---------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < totalRecords; i++) {
        if (strstr(store[i].website, targetWebsite) != NULL) {
            print_coloredf(COLOR_BOLD,"%-30s | %-25s | %-25s\n", store[i].website, store[i].username, store[i].password);
        }
    }
    printf("\n");

    while (!found) {
        char selected[100];
        print_colored(COLOR_CYAN,"Enter the full website name to select (type ""exit"" to close update): ");
        scanf("%s", selected);
        clearInputBuffer();


        toLowerCase(selected);
        if (strcmp(selected, "exit") == 0) {
            print_colored(COLOR_GREEN,"Delete Aborted\n");
            free(store);
            return;
        }

        int exactTotal = 0;
        for (size_t i = 0; i < totalRecords; i++) {
            if (strcmp(store[i].website, selected) == 0) {
                exactTotal++;
            }
        }

        if (exactTotal == 0) {
            print_colored(COLOR_RED,"No exact match found for that website name. Try again.\n");
            continue;
        }

        if (exactTotal > 1) {
            print_coloredf(COLOR_MAGENTA, "%d credential(s) found:\n", exactTotal);
            print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
            print_coloredf(COLOR_MAGENTA,"%-30s | %-25s | %-25s\n", "WEBSITE", "USERNAME", "PASSWORD");
            print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
            for (size_t i = 0; i < totalRecords; i++) {
                if (strcmp(store[i].website, selected) == 0) {
                    print_coloredf(COLOR_BOLD,"%-30s | %-25s | %-25s\n", store[i].website, store[i].username, store[i].password);
                }
            }
            printf("\n");
            print_colored(COLOR_CYAN,"Enter which username to delete (type ""exit"" to cancel): ");
            char toDelete[100];
            fgets(toDelete, sizeof(toDelete), stdin);
            removeFgetsNewLine(toDelete);


            toLowerCase(toDelete);
            if (strcmp(toDelete, "exit") == 0) {
                print_colored(COLOR_GREEN,"Deleting Aborted\n");
                free(store);
                return;
            }


            newCount = 0;
            found = 0;
            for (size_t i = 0; i < totalRecords; i++) {
                if (strcmp(store[i].website, selected) == 0 && strcmp(store[i].username, toDelete) == 0) {
                    print_coloredf(COLOR_GREEN,"Deleted username & password for %s on website %s\n", store[i].username, selected);
                    found = 1;

                } else {
                    store[newCount++] = store[i];
                }
            }
            if (!found) {
                print_colored(COLOR_RED,"Username not found. Try again or type ""exit""\n");
            }
        } else {
            newCount = 0;
            found = 0;
            for (size_t i = 0; i < totalRecords; i++) {
                if (strcmp(store[i].website, selected) == 0) {
                    print_coloredf(COLOR_GREEN,"Deleted username & password for %s\n", selected);
                    found = 1;

                } else {
                    store[newCount++] = store[i];
                }
            }
        }
    }


    fp = fopen("passwords.txt", "wb");
    if (fp == NULL) {
        print_colored(COLOR_RED,"File could not be opened for writing\n");
        free(store);
        return;
    }
    for (int i = 0; i < newCount; i++) {
        unsigned char encrypted[512];

        int encrypted_len = encryptData((unsigned char *)&store[i], sizeof(info), key, iv, encrypted);

        fwrite(&encrypted_len, sizeof(int), 1, fp);
        fwrite(encrypted, 1, encrypted_len, fp);
    }

    fclose(fp);
    free(store);


    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED,"File could not be opened for logging\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    strcpy(entry.notes, "Delete Password");
    strcpy(entry.tags, "Delete");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
}


void updatePassword(char *targetWebsite) {
    info *store;
    FILE *fp = fopen("passwords.txt", "rb");
    if (fp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        print_colored(COLOR_RED, "File is empty\n");
        fclose(fp);
        return;
    }
    rewind(fp);

    size_t totalRecords = size / sizeof(info);
    store = malloc(totalRecords * sizeof(info));

    FILE *masfp = fopen("master.txt", "rb");
    if (masfp == NULL) {
        print_colored(COLOR_RED, "File could not be opened\n");
    }
    masterHash stored;
    fread(&stored, sizeof(masterHash), 1, masfp);
    for (size_t i = 0; i < totalRecords; i++) {
        int encrypted_len;
        fread(&encrypted_len, sizeof(int), 1, fp);

        unsigned char encrypted[512];
        fread(encrypted, 1, encrypted_len, fp);

        decryptData(encrypted, encrypted_len, key, iv, (unsigned char *)&store[i]);
    }

    fclose(masfp);
    fclose(fp);

    toLowerCase(targetWebsite);
    int totalFound = 0;

    for (size_t i = 0; i < totalRecords; i++) {
        if (strstr(store[i].website, targetWebsite) != NULL) {
            totalFound++;
        }
    }

    if (totalFound == 0) {
        print_colored(COLOR_RED,"Website not found. Try adding it.\n");
        free(store);
        return;
    }

    print_coloredf(COLOR_MAGENTA,"%d credential(s) found:\n", totalFound);
    print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
    print_coloredf(COLOR_MAGENTA,"%-30s | %-25s | %-25s\n", "WEBSITE", "USERNAME", "PASSWORD");
    print_colored(COLOR_MAGENTA,"---------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < totalRecords; i++) {
        if (strstr(store[i].website, targetWebsite) != NULL) {
            print_coloredf(COLOR_BOLD,"%-30s | %-25s | %-25s\n", store[i].website, store[i].username, store[i].password);
        }
    }

    printf("\n");
    char selected[100];
    int exactTotal = 0;
    while (1) {
        print_colored(COLOR_CYAN,"Enter the full website name to select (type ""exit"" to close update): ");
        scanf("%s", selected);
        clearInputBuffer();


        toLowerCase(selected);
        if (strcmp(selected, "exit") == 0) {
            print_colored(COLOR_GREEN,"Delete Aborted");
            return;
        }

        exactTotal = 0;
        for (size_t i = 0; i < totalRecords; i++) {
            if (strcmp(store[i].website, selected) == 0) {
                exactTotal++;
            }
        }

        if (exactTotal == 0) {
            print_colored(COLOR_RED,"No exact match for website. Try again.\n");
        } else {
            break;
        }
    }

    int found = 0;
    while (!found) {
        char targetUser[100];
        print_colored(COLOR_CYAN,"Select an existing username to update (or type 'exit' to cancel): ");
        fgets(targetUser, sizeof(targetUser), stdin);
        removeFgetsNewLine(targetUser);

        toLowerCase(targetUser);

        if (strcmp(targetUser, "exit") == 0) {
            print_colored(COLOR_GREEN,"Update Aborted\n");
            free(store);
            return;
        }

        resetString(targetUser);

        for (size_t i = 0; i < totalRecords; i++) {
            if (strcmp(store[i].website, selected) == 0 && strcmp(store[i].username, targetUser) == 0) {

                char temp[100];

                print_colored(COLOR_CYAN,"Enter new username (or type ""skip"" to keep the same): ");
                fgets(temp, sizeof(temp), stdin);
                removeFgetsNewLine(temp);

                toLowerCase(temp);
                if (strcmp(temp, "skip") != 0) {
                    resetString(temp);
                    strcpy(store[i].username, temp);
                }

                print_colored(COLOR_CYAN,"Enter new password (or type ""skip"" to keep the same): ");
                fgets(temp, sizeof(temp), stdin);
                removeFgetsNewLine(temp);

                toLowerCase(temp);
                if (strcmp(temp, "skip") != 0) {
                    resetString(temp);
                    strcpy(store[i].password, temp);
                }

                print_coloredf(COLOR_GREEN,"Credentials updated for %s\n", selected);
                found = 1;
                break;
                }
        }

        if (!found) {
            print_colored(COLOR_RED,"Username not found. Try again or type ""exit"".\n");
        }
    }

    fp = fopen("passwords.txt", "wb");
    if (fp == NULL) {
        print_colored(COLOR_RED,"File could not be opened for writing.\n");
        free(store);
        return;
    }

    for (int i = 0; i < totalRecords; i++) {
        unsigned char encrypted[512];

        int encrypted_len = encryptData((unsigned char *)&store[i], sizeof(info), key, iv, encrypted);

        fwrite(&encrypted_len, sizeof(int), 1, fp);
        fwrite(encrypted, 1, encrypted_len, fp);
    }

    fclose(fp);
    free(store);
    print_colored(COLOR_GREEN,"Changes saved successfully.\n");

    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED,"Usage Logging Failed\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    strcpy(entry.notes, "Update Password");
    strcpy(entry.tags, "Update");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
}


void readLogs() {
    FILE *fp = fopen("log.txt", "rb");
    if (fp == NULL) {
        print_colored(COLOR_RED,"File could not be opened for reading.\n");
    }
    log *data;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) {
        print_colored(COLOR_RED, "File is empty\n");
        fclose(fp);
        return;
    }
    rewind(fp);
    size_t const totalRecords = size / sizeof(log);

    data = malloc(totalRecords * sizeof(log));
    if (data == NULL) {
        print_colored(COLOR_RED, "Memory allocation error");
        fclose(fp);
        return;
    }
    fread(data, sizeof(log), totalRecords, fp);
    fclose(fp);
    print_colored(COLOR_MAGENTA,"-----------------------------------------------------------------------------------------------------------------------------------------\n");
    print_coloredf(COLOR_MAGENTA,"%-10s | %-10s | %-10s | %-50s | %-30s\n", "DATE", "TIME", "ATTEMPTS", "NOTES", "TAGS");
    print_colored(COLOR_MAGENTA,"-----------------------------------------------------------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < totalRecords; i++) {
        print_coloredf(COLOR_BOLD,"%-10s | %-10s | %-10d | %-50s | %-30s\n",
            data[i].date, data[i].time, data[i].attempts, data[i].notes, data[i].tags);
    }
    free(data);
}

void clearLogs() {
    FILE *fp = fopen("log.txt", "wb");
    if (fp == NULL) {
        print_colored(COLOR_RED, "Failed to open log.txt for clearing.\n");
        return;
    }
    fclose(fp);
    print_colored(COLOR_GREEN, "Logs cleared successfully.\n");
}

void nuke() {
    if (remove("passwords.txt") != 0) {
        print_colored(COLOR_RED,"No passwords stored or already erased.\n");
    }
    else {
        print_colored(COLOR_GREEN,"Stored passwords deleted\n");
    }
    if (remove("master.txt") != 0) {
        print_colored(COLOR_RED,"Master password file doesn't exist or already erased.\n");
    }
    else {
        print_colored(COLOR_GREEN,"Master password deleted\n");
    }

    if (remove("log.txt") != 0) {
        print_colored(COLOR_RED,"Log file doesn't exist or already erased.\n");
    }
    else {
        print_colored(COLOR_GREEN,"Log file deleted\n");
    }
    exit(0);
}
