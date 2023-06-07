struct player {
    double x, y, orientation, vitesse, size;
};

struct ball{
	double x, y, vx, vy, size;
};

class Game {
  public:
	player* players;
	ball ball;
	
    Game();
    void print();
};
