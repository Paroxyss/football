import pygame as pg
import json
import random
import colorsys
from math import fmod
import random



def read_json():
    name = "arbreDeVie.json"
    with open(name) as json_file:
        data = json.load(json_file)
        return data


def random_color():
    return random.random(), 0.5 + 0*random.random(), 0.5 #random.random()


def mutate(color):
    while(color[0] < 0):
        color[0]+=1
    return color
    return (
        fmod(color[0] + random.random() * 0.05 - 0.025, 1),
        min(color[1] + random.random() * 0.05 - 0.025, 1),
        min(color[2] + random.random() * 0.05 - 0.025, 1),
    )


def draw_couche_genealogique(data, j):
    l = data#random.sample(data, len(data))

    colors = []
    for i in range(len(l)):
        if l[i][1] == 0 and l[i][2] == 0:
            color = random_color() #(j/7, 0.5, 0.5) 
        elif l[i][2] == 0:
            color = mutate(ids[l[i][1]])
        else:
            color = mutate(average_color(ids[l[i][1]], ids[l[i][2]]))

        ids[l[i][0]] = color
        colors.append([color, i])

    def s(a): return fmod(a[0][0] + 1, 1)
    colors.sort(key=s)
    i = 0
    for a in range(len(colors)):
        color = colors[a][0]
        converted = colorsys.hls_to_rgb(color[0], color[1], color[2])
        converted = (converted[0] * 255, converted[1] * 255, converted[2] * 255)
        #print(color, converted)
        pg.draw.rect(screen, converted, (j * r, r * i, r+1, r+1))
        pg.draw.rect(screen, converted, (j * r, r * (i + len(colors)), r+1, r+1))
        i += 1

def circularDistance(a, b):
    return abs(fmod(abs(b-a), 1))
# moyenne circulaire pour un cercle entre 0 et 1
def circlularMean(a, b):
    m1 = (a+b)/2
    m2 = m1+0.5
    if(circularDistance(a, m1) < circularDistance(a, m2)):
        return m1
    return m2

def average_color(color1, color2):
    return (
        circlularMean(color1[0], color2[0]),
        (color1[1] + color2[1]) / 2,
        (color1[2] + color2[2]) / 2
    )


file_content = read_json()

pg.init()
running = True
population_size = 700 #2*len(file_content)
screen_width = 1400
screen_height = 850
r = screen_width / population_size
red = (255, 255, 255)
screen = pg.display.set_mode((screen_width, screen_height))
ids = {}
for i in range(len(file_content)):
    draw_couche_genealogique(file_content[i], i)


pg.display.flip()

while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
