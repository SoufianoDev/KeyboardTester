#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #pragma comment(lib, "winmm.lib")
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <termios.h>
    #include <unistd.h>
    #include <time.h>
#endif

/* ─── ANSI Colors ─────────────────────────────────────────────────── */
#define COLOR_VIOLET  "\033[35m"
#define COLOR_CRIMSON "\033[38;2;220;20;60m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RESET   "\033[0m"



/* ─── Struct definition ───────────────────────────────────────────── */
typedef struct KeyboardTester KeyboardTester;
typedef void (*OnClickListener)(KeyboardTester* self, int keyStroke);

struct KeyboardTester {
    bool isRunning;
    OnClickListener onClickListener;

    void (*init)(KeyboardTester* self);
    void (*playIntroSound)(KeyboardTester* self);
    void (*run)(KeyboardTester* self);
    void (*playSound)(KeyboardTester* self);
    void (*cleanup)(KeyboardTester* self);
    void (*setOnClickListener)(KeyboardTester* self, OnClickListener listener);
};

/* ─── POSIX raw mode ──────────────────────────────────────────────── */
#ifndef _WIN32
struct termios originalTermios;
void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios); }
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &originalTermios);
    atexit(disableRawMode);
    struct termios raw = originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#endif

/* ─── Typing animation ────────────────────────────────────────────── */
void typeText(const char* text, unsigned int milliseconds) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        #ifdef _WIN32
            Sleep(milliseconds);
        #else
            usleep(milliseconds * 1000);
        #endif
    }
}

/* ─── Audio ───────────────────────────────────────────────────────── */
void playSoundEffect(KeyboardTester* self) {
    const char* fileName = "res/audio/key_is_working.wav";
    #ifdef _WIN32
        PlaySound(fileName, NULL, SND_FILENAME | SND_ASYNC);
    #elif __APPLE__
        char command[512]; sprintf(command, "afplay \"%s\" &", fileName); system(command);
    #else
        char command[512]; sprintf(command, "aplay -q \"%s\" &", fileName); system(command);
    #endif
}

void playIntroSound(KeyboardTester* self) {
    const char* fileName = "res/audio/yokoso.wav";
    #ifdef _WIN32
        PlaySound(fileName, NULL, SND_FILENAME | SND_SYNC);
        while (_kbhit()) _getch();
    #elif __APPLE__
        char command[512]; sprintf(command, "afplay \"%s\"", fileName); system(command);
        tcflush(STDIN_FILENO, TCIFLUSH);
    #else
        char command[512]; sprintf(command, "aplay -q \"%s\"", fileName); system(command);
        tcflush(STDIN_FILENO, TCIFLUSH);
    #endif

    typeText(">> Initializing system peripherals...\n", 30);
    typeText(">> Welcome, User. Keyboard diagnostic mode is now ACTIVE.\n", 40);
    typeText(">> All inputs are being monitored. Press 'ESC' to terminate session.\n", 40);
    printf("--------------------------------------------------\n\n");
}

/* ─── Init ────────────────────────────────────────────────────────── */
void testerInit(KeyboardTester* self) {
    self->isRunning = true;

    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0; GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; SetConsoleMode(hOut, dwMode);
    #else
        enableRawMode();
    #endif

    printf("\n\n%s", COLOR_VIOLET);
    printf("\t███████╗ ██████╗ ██╗   ██╗███████╗██╗ █████╗ ███╗   ██╗ ██████╗ \n");
    printf("\t██╔════╝██╔═══██╗██║   ██║██╔════╝██║██╔══██╗████╗  ██║██╔═══██╗\n");
    printf("\t███████╗██║   ██║██║   ██║█████╗  ██║███████║██╔██╗ ██║██║   ██║\n");
    printf("\t╚════██║██║   ██║██║   ██║██╔══╝  ██║██╔══██║██║╚██╗██║██║   ██║\n");
    printf("\t███████║╚██████╔╝╚██████╔╝██║     ██║██║  ██║██║ ╚████║╚██████╔╝\n");
    printf("\t╚══════╝ ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ \n");
    printf("\t                    ██████╗ ███████╗██╗   ██╗                    \n");
    printf("\t                    ██╔══██╗██╔════╝██║   ██║                    \n");
    printf("\t                    ██║  ██║█████╗  ██║   ██║                    \n");
    printf("\t                    ██║  ██║██╔══╝  ╚██╗ ██╔╝                    \n");
    printf("\t                    ██████╔╝███████╗ ╚████╔╝                     \n");
    printf("\t                    ╚═════╝ ╚══════╝  ╚═══╝                      \n");
    printf("%s\n", COLOR_RESET);
}

/* ─── Cleanup ────────────────────────────────────────────────────── */
void testerCleanup(KeyboardTester* self) {
    printf("\n\n");
    printf("--------------------------------------------------\n");
    printf("%s>> SESSION TERMINATED%s\n", COLOR_CYAN, COLOR_RESET);
    printf("--------------------------------------------------\n");
    printf(">> Diagnostics complete. Goodbye!\n");
}

/* ─── Listener setter ─────────────────────────────────────────────── */
void setOnClickListener(KeyboardTester* self, OnClickListener listener) {
    self->onClickListener = listener;
}

/* ─── Main loop ───────────────────────────────────────────────────── */
void testerRun(KeyboardTester* self) {
    while (self->isRunning) {
        int keyCode;
        #ifdef _WIN32
            keyCode = _getch();
            if (keyCode == 0 || keyCode == 224) { _getch(); continue; }
        #else
            keyCode = getchar();
        #endif

        if (keyCode == 27) { self->isRunning = false; break; }
        if (self->onClickListener) self->onClickListener(self, keyCode);

        
    }
}

/* ─── Factory ─────────────────────────────────────────────────────── */
KeyboardTester createKeyboardTester() {
    KeyboardTester tester = {
        .isRunning = false, .onClickListener = NULL,
        .init = testerInit, .playIntroSound = playIntroSound,
        .run = testerRun, .playSound = playSoundEffect,
        .cleanup = testerCleanup, .setOnClickListener = setOnClickListener
    };
    return tester;
}

/* ─── Key handler ─────────────────────────────────────────────────── */
void handleKeyPress(KeyboardTester* tester, int keyStroke) {
    tester->playSound(tester);

    /* ── Key label ── */
    if (keyStroke == 32) {
        /* SPACE key */
        printf("[INPUT] Key: %sSPACE%s\n", COLOR_CRIMSON, COLOR_RESET);
    } else if (keyStroke >= 32 && keyStroke <= 126) {
        printf("[INPUT] Key: %s%c%s\n", COLOR_VIOLET, (char)keyStroke, COLOR_RESET);
    } else {
        printf("[INPUT] Key: %sControl/System (code %d)%s\n", COLOR_CRIMSON, keyStroke, COLOR_RESET);
    }
}


int main() {
    KeyboardTester app = createKeyboardTester();
    app.init(&app);
    app.playIntroSound(&app);
    app.setOnClickListener(&app, handleKeyPress);
    app.run(&app);
    app.cleanup(&app);
    return 0;
}
