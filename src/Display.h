
// représente un "écran" virtuel, qui s'affichera pour le moment dans le terminal.
// À terme, on pourrait faire une interface web qui se connecte en websocket,
// de manière à pouvoir lancer le programme sur un serveur et le monitorer depuis un site

enum displayColor {
    TERM_BLACK = 30,
    TERM_RED = 31,
    TERM_GREEN = 32,
    TERM_YELLOW = 33,
    TERM_BLUE = 34,
    TERM_MAGENTA = 35,
    TERM_CYAN = 36,
    TERM_WHITE = 37,
    TERM_DEFAULT = 39,
    TERM_RESET = 0
};

class Display {
  private:
    double heightFactor;
    double lengthFactor;
    // Hauteur et largeur exposées à l'exterieur:
    // Le programme aura l'impression que l'écran aura ces dimensions,
    // ce qui est faux mais pratique
    int fakeHeight;
    int fakeLen;

    void moveTo(int col, int ligne);
    void moveToVirtualPos(int x, int y);

    void setColor(displayColor color);

  public:
    Display(int length, int height);

    void setChar(char c, int x, int y, displayColor color = TERM_DEFAULT);
    void drawCircle(char c, int x, int y, int radius, displayColor color = TERM_DEFAULT);

    void clear();

    void cursorToBottom();
    void drawBorders(displayColor color = TERM_DEFAULT);
    void drawLine(char c,  int startx, int starty, int endx, int endy, displayColor color = TERM_DEFAULT, int resolution = 10);
};
