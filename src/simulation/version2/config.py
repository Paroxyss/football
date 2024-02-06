import pygame as pg
import os

SCREEN_WIDTH, SCREEN_HEIGHT = 119 * 7, 75 * 7
PLAYER_SIZE = 2 * 10
COM_SIZE = 2
BALL_SIZE = 10
GOAL_SIZE = 15 * 12
GOAL_THICKNESS = 5

RED = (255, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
LIGHT_BLUE = (80, 80, 255)
BLACK = (0, 0, 0)
YELLOW = (255, 255, 0)
GREEN = (0, 255, 0)
PINK = (255, 192, 203)
PURPLE = (255, 0, 255)

FIELD_GREEN = (50, 168, 82)
FIELD_DARK_GREEN = (46, 153, 75)
N_BANDE = 10

PLAYER_ACCELERATION = 0.3
PLAYER_FROTTEMENT = 1 / 20

MASS_PLAYER = 100
MASS_BALL = 40

if not ("SCREEN_FACTOR" in os.environ.keys()):
    global SCREEN_FACTOR
    SCREEN_FACTOR = 1
else:
    SCREEN_FACTOR = float(os.environ["SCREEN_FACTOR"])

#screen = pg.display.set_mode((SCREEN_WIDTH*SCREEN_FACTOR, SCREEN_HEIGHT*SCREEN_FACTOR))
