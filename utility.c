#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "colors.h"
#include "utility.h"


void exitProgram() {
    print_colored(COLOR_BOLD, "Program Closed");
    FILE *logfp = fopen("log.txt", "ab");
    if (logfp == NULL) {
        print_colored(COLOR_RED, "Usage Logging Failed\n");
        return;
    }
    log entry;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(entry.time, 9, "%H:%M:%S", tm_info);
    strftime(entry.date, 12, "%Y-%m-%d", tm_info);
    entry.attempts = 0;
    strcpy(entry.notes, "Manager Closed");
    strcpy(entry.tags, "Exit");
    fwrite(&entry, sizeof(entry), 1, logfp);
    fclose(logfp);
    exit(0);
}

void toLowerCase(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str += 32;
        }
        str++;
    }
}

void resetString(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str -= 32;
        }
        str++;
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getValidatedInt(const char *prompt) {
    int number;
    char input[100];

    while (1) {
        print_coloredf(COLOR_GREEN, "%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';

            toLowerCase(input);
            if (strcmp(input, "exit") == 0) {
                exitProgram();
            }

            if (sscanf(input, "%d", &number) == 1) {
                return number;
            } else {
                print_colored(COLOR_RED, "Please enter a valid choice!\n");
            }
        } else {
            print_colored(COLOR_RED, "Error reading input!\n");
        }
        clearInputBuffer();
    }
}

void removeFgetsNewLine(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

void freeOldData(info *oldData) {
    if (oldData != NULL) {
        free(oldData);
        oldData = NULL;
    }
}