#ifndef UTILITY_H
#define UTILITY_H
#include "passManager.h"

void exitProgram();
void toLowerCase(char *str);
void resetString(char *str);
void clearInputBuffer();
int getValidatedInt(const char *prompt);
void removeFgetsNewLine(char *str);
void freeOldData(info *oldData);


#endif //UTILITY_H
