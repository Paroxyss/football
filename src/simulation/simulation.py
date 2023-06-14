import pygame as pg
from math import pi
from time import sleep

from functions import *
from config import *


pg.init()
running = True


while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False

    screen.fill(BLACK)
    draw_field()
    draw_player(1, 100, 50, pi / 3)
    draw_player(2, 250, 250, 0)
    draw_player(2, 300, 30, pi / 2)

    draw_ball(300, 300)

    draw_goal()

    sleep(1 / 60)
    pg.display.flip()

pg.quit()
