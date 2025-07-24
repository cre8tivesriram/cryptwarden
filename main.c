#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passManager.h"
#include "banner.h"
#include "colors.h"
#include "utility.h"

void storeWrapper() {
    info *details;
    int records;
    storePassword(&details, &records);

    free(details);
}

void searchWrapper() {
    char search[100];
    print_colored(COLOR_CYAN, "Enter name of website: ");
    scanf("%s", search);
    clearInputBuffer();
    searchPassword(search);
}

void deleteWrapper() {
    char delete[100];
    print_colored(COLOR_CYAN, "Enter name of website: ");
    scanf("%s", delete);
    clearInputBuffer();
    deletePassword(delete);
}

void updateWrapper() {
    char update[100];
    print_colored(COLOR_CYAN, "Enter name of website: ");
    scanf("%s", update);
    clearInputBuffer();
    updatePassword(update);
}

void (*menuOptions[6])() = {
    NULL,
    storeWrapper,
    readPassword,
    NULL,
    NULL,
    nuke
};

void (*manageOptions[3])() = { searchWrapper, updateWrapper, deleteWrapper };
void (*logOptions[2])() = { readLogs, clearLogs };

int main() {
    showBanner();
    for (int i = 0; i < 2; i++) {
        printf("\n");
    }
    int choice;
    int access = masterPassword();

    if (access != 0) {
        while (1) {
            print_colored(COLOR_MAGENTA, "\n--- Main Menu ---\n");
            print_colored(COLOR_CYAN, "1. Store passwords\n");
            print_colored(COLOR_CYAN, "2. Read all passwords\n");
            print_colored(COLOR_CYAN, "3. Manage by Website\n");
            print_colored(COLOR_CYAN, "4. Manage Logs\n");
            // print_colored(COLOR_CYAN, "5. Reset Master Password\n");
            print_colored(COLOR_CYAN, "5. Nuke all Data\n\n");

            choice = getValidatedInt("Enter your choice (Type ""exit"" to close): ");

            if (choice == 3) {
                while (1) {
                    int manage = 0;
                    print_colored(COLOR_MAGENTA, "\n--- Manage Menu ---\n");
                    print_colored(COLOR_CYAN, "1. Search\n");
                    print_colored(COLOR_CYAN, "2. Update\n");
                    print_colored(COLOR_CYAN, "3. Delete\n");
                    print_colored(COLOR_CYAN, "4. Main Menu\n\n");
                    print_colored(COLOR_CYAN, "\n");

                    manage = getValidatedInt("Enter your choice (Type ""exit"" to close): ");

                    if (manage >= 1 && manage <= 3) {
                        manageOptions[manage - 1]();
                    } else if (manage == 4) {
                        break;
                    } else {
                        print_colored(COLOR_RED, "Invalid choice, please try again.\n");
                    }
                }
                continue;
            }

            if (choice == 4) {
                while (1) {
                    int logChoice = 0;
                    print_colored(COLOR_MAGENTA, "\n--- Logs Menu ---\n");
                    print_colored(COLOR_CYAN, "1. Read Logs\n");
                    print_colored(COLOR_CYAN, "2. Clear Logs\n");
                    print_colored(COLOR_CYAN, "3. Back to Main Menu\n\n");


                    logChoice = getValidatedInt("Enter your choice (Type ""exit"" to close): ");

                    if (logChoice >= 1 && logChoice <= 2) {
                        logOptions[logChoice - 1]();
                    } else if (logChoice == 3) {
                        break;
                    } else {
                        print_colored(COLOR_RED, "Invalid choice, please try again.\n");
                    }
                }
                continue;
            }

            if (choice == 5) {
                char answer[4];
                print_colored(COLOR_RED, "\nNuking results in complete removal of all passwords, master password and logs stored.\n");
                print_colored(COLOR_RED, "Do you want to continue? (Y/N) THERE IS NO GOING BACK: ");
                scanf("%s", answer);
                toLowerCase(answer);
                clearInputBuffer();

                if (strcmp(answer, "y") == 0 || strcmp(answer, "yes") == 0) {
                    menuOptions[choice]();
                } else {
                    print_colored(COLOR_GREEN, "Nuke Aborted! Your Data is Safe.\n");
                }
                continue;
            }

            if (choice >= 1 && choice <= 2) {
                menuOptions[choice]();
            } else {
                print_colored(COLOR_RED, "Invalid choice, try again!\n");
            }
        }
    }
    return 0;
}

