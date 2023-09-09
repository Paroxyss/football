import pygame as pg

SIZE = 1.5


def meter(x):
    return x * 10


# dimensions du stade de France, j'ai augmenté la taille
# des cages pour que ce soit plus facile de marquer.
SCREEN_WIDTH, SCREEN_HEIGHT = meter(119 / 2) * SIZE, meter(75 / 2) * SIZE
PLAYER_SIZE = meter(2) * SIZE
BALL_SIZE = 10 * SIZE
GOAL_SIZE = meter(15) * SIZE
# GOAL_SIZE = meter(7.32)
GOAL_THICKNESS = 5 * SIZE

RED = (255, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)
YELLOW = (255, 255, 0)
GREEN = (0, 255, 0)
PINK = (255, 192, 203)

FIELD_GREEN = (50, 168, 82)
FIELD_DARK_GREEN = (46, 153, 75)
N_BANDE = 10

screen = pg.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
