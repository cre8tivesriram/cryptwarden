#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define usleep(x) Sleep((x) / 1000)
#endif

// ---------- Utility Effects ----------
void typeEffect(const char *text, useconds_t delay) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        usleep(delay);
    }
}

void glitchEffect(const char *finalText, useconds_t delayPerChar, const char *color) {
    int len = strlen(finalText);
    char charset[] = "!@#$%^&*()_+=-{}[]|:;<>,.?/~";
    char buffer[len + 1];
    memset(buffer, ' ', len);
    buffer[len] = '\0';

    srand(time(NULL));
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 3; j++) { // reduced from 5 to 3 for speed
            buffer[i] = charset[rand() % (sizeof(charset) - 1)];
            printf("\r%81s%s%s\033[0m", "", color, buffer);
            fflush(stdout);
            usleep(delayPerChar / 3);
        }
        buffer[i] = finalText[i];
        printf("\r%81s%s%s\033[0m", "", color, buffer);
        fflush(stdout);
        usleep(delayPerChar);
    }
}

// ---------- Banner Drawing ----------
void showBanner() {
    const char *red   = "\033[1;91m";  // Bright Red (NO BLINK)
    const char *cyan  = "\033[1;96m";  // Bright Cyan
    const char *green = "\033[1;92m";  // Bright Green
    const char *reset = "\033[0m";

    const char *block[] = {
        "   ____    ____     __   __  ____    _____                   _       ____     ____  U _____ u _   _     ",
        "U /\"___|U |  _\"\\ u  \\ \\ / /U|  _\"\\ u|_ \" _| __        __ U  /\"\\  uU |  _\"\\ u |  _\"\\ \\| ___\"|/| \\ |\"|    ",
        "\\| | u   \\| |_) |/   \\ V / \\| |_) |/  | |   \\\"\\      /\"/  \\/ _ \\/  \\| |_) |//| | | | |  _|\" <|  \\| |>   ",
        " | |/__   |  _ <    U_|\"|_u |  __/   /| |\\  /\\ \\ /\\ / /\\  / ___ \\   |  _ <  U| |_| |\\| |___ U| |\\  |u   ",
        "  \\____|  |_| \\_\\     |_|   |_|     u |_|U U  \\ V  V /  U/_/   \\_\\  |_| \\_\\  |____/ u|_____| |_| \\_|    ",
        " _// \\\\   //   \\\\_.-,//|(_  ||>>_   _// \\\\_.-,_\\ /\\ /_,-. \\\\    >>  //   \\\\_  |||_   <<   >> ||   \\\\,_. ",
        "(__)(__) (__)  (__)(_) (__)(__)__) (__) (__)(_)'-'  '-(_/ (__)  (__)(__)  (__)(__)_) (__) (__)(_\")  (_/ "
    };

    printf("\033[2J\033[H"); // Clear screen

    // Just print the banner once (no blinking)
    for (int i = 0; i < 7; i++) {
        printf("%s%s%s\n", red, block[i], reset);
    }

    // Subtitle with spacing (typeEffect)
    printf("                                                                            ");
    printf("%s", cyan);
    typeEffect("-- the PasswordManager --", 20000);  // nice typing effect
    printf("%s\n", reset);

    usleep(150000); // slight pause

    // Subtitle with glitch effect and spacing
    printf("                                                                            ");
    glitchEffect("-- by sudosriram --", 30000, green);
    printf("%s\n", reset);
}
