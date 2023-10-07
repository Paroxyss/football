import pygame as pg
import sys
import json
from random import randint
from time import sleep

pg.init()
running = True
population_size = 100
screen_width = 1400
screen_height = 850
r = screen_width / population_size
red = (255, 255, 255)
screen = pg.display.set_mode((screen_width, screen_height))


def random_color():
    return (randint(0, 255), randint(0, 255), randint(0, 255))


def draw_couche_genealogique(j):
    global cur_sp

    for i in range(population_size):
        color = random_color() if j == 0 else red
        pg.draw.rect(screen, color, (i * r, r * j, r, r))


def everage_color(color1, color2):
    return (
        (color1[0] + color2[0]) / 2,
        (color1[1] + color2[1]) / 2,
        (color1[2] + color2[2]) / 2,
    )


for j in range(100):
    draw_couche_genealogique(j)

pg.display.flip()

while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
