import pygame as pg
import json
import random
import colorsys
from math import fmod
import random

pg.init()
running = True
population_size = 20
screen_width = 1400
screen_height = 850
r = screen_width / population_size
red = (255, 255, 255)
screen = pg.display.set_mode((screen_width, screen_height))
ids = {}


def read_json():
    name = "arbreDeVie.json"
    with open(name) as json_file:
        data = json.load(json_file)
        return data


def random_color():
    return random.random(), random.random(), random.random()


def average_color(color1, color2):
    return (
        (color1[0] + color2[0]) / 2,
        (color1[1] + color2[1]) / 2,
        (color1[2] + color2[2]) / 2,
    )


def mutate(color):
    return (
        fmod(color[0] + random.random() * 0.05, 1),
        fmod(color[1] + random.random() * 0.05, 1),
        fmod(color[2] + random.random() * 0.05, 1),
    )


def draw_couche_genealogique(data, j):
    l = random.sample(data, len(data))

    for i in range(len(l)):
        if l[i][1] == 0 and l[i][2] == 0:
            color = random_color()
        elif l[i][2] == 0:
            color = mutate(ids[l[i][1]])
        else:
            color = mutate(average_color(ids[l[i][1]], ids[l[i][2]]))

        converted = colorsys.hls_to_rgb(color[0], color[1], color[2])
        converted = (converted[0] * 255, converted[1] * 255, converted[2] * 255)
        ids[l[i][0]] = color

        pg.draw.rect(screen, converted, (i * r, r * j, r, r))


def everage_color(color1, color2):
    return (
        (color1[0] + color2[0]) / 2,
        (color1[1] + color2[1]) / 2,
        (color1[2] + color2[2]) / 2,
    )


file_content = read_json()

for i in range(len(file_content)):
    draw_couche_genealogique(file_content[i], i)


pg.display.flip()

while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
