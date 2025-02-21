//
// Created by Yuriko on 2/19/2025.
//

#include "shell.h"

#include <termios.h>
#include <stdio.h>
#include <unistd.h>

struct termios original_settings;

void set_terminal_raw_mode() {
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, &original_settings);
    new_settings = original_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}


void restore_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);
}