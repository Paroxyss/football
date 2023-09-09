import pygame as pg


def meter(x):
    return x * 10


# dimensions du stade de France, j'ai augmenté la taille
# des cages pour que ce soit plus facile de marquer.
SCREEN_WIDTH, SCREEN_HEIGHT = meter(119 / 2), meter(75 / 2)
PLAYER_SIZE = meter(2)
BALL_SIZE = 10
GOAL_SIZE = meter(15)
# GOAL_SIZE = meter(7.32)
GOAL_THICKNESS = 5

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
