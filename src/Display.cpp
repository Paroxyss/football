#include "Display.h"
#include "config.h"

#include <cstdio>
#include <iostream>
#include <math.h>
#include <ostream>
#include <sys/ioctl.h>
#include <unistd.h>

void Display::moveTo(int col, int ligne) { printf("\033[%d;%df", ligne + 1, col + 1); }

void Display::clear() { std::cout << "\033[2J" << std::endl; };

void Display::setColor(displayColor color) { printf("\033[%dm", color); }

void Display::moveToVirtualPos(int x, int y) { moveTo(x * lengthFactor, y * heightFactor); };

Display::Display(int length, int height) {
    this->fakeLen = length;
    this->fakeHeight = height;

	// on récupère les dimensions du terminal
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// on initialise les facteurs
    double scaleFactor;
    double horizontaFactor = (double)w.ws_xpixel / length;
    double verticalFactor = (double)w.ws_ypixel / height;
	
	// On utilisera le plus petit facteur pour afficher, de manière à s'assurer que tout rentre
    if (verticalFactor < horizontaFactor) { 
        scaleFactor = verticalFactor;
    } else {
        scaleFactor = horizontaFactor;
    }

	// on définir les facteurs que l'on utilisera
    this->lengthFactor = scaleFactor / ((double)w.ws_xpixel / (w.ws_col + 2));
    this->heightFactor = scaleFactor / ((double)w.ws_ypixel / (w.ws_row + 2));
};

// met un caractère à une ceratine position
void Display::setChar(char c, int x, int y, displayColor color) {
    moveToVirtualPos(x, y);
    setColor(color);
    std::cout << c;
};

void Display::drawCircle(char c, int x, int y, int radius, displayColor color) {
    for (int i = 0; i < 360; i += 12) {
        setChar(c, x + radius * cos(i * PI / 180), y + radius * sin(i * PI / 180), color);
    }
};

// mets le curseur en bas à gauche, notamment pour que le terminal ecrive le prompt après
void Display::cursorToBottom() { moveToVirtualPos(fakeLen, fakeHeight); }
void Display::drawBorders(displayColor color) {
    for (int i = 0; i < fakeHeight; i += 1 / heightFactor) {
        setChar('#', 0, i, color);
    }
    for (int i = 0; i < fakeHeight; i += 1 / heightFactor) {
        setChar('#', fakeLen - 1 / heightFactor, i, color);
    }
    for (int i = 0; i < fakeLen; i += 1 / lengthFactor) {
        setChar('#', i, 0, color);
    }
    for (int i = 0; i < fakeLen; i += 1 / lengthFactor) {
        setChar('#', i, fakeHeight, color);
    }
}
