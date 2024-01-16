import pygame as pg
import numpy as np
import sys
import json
import random
import colorsys
from math import fmod, pow
import random

def read_json():
    name = "stats.json"
    with open(name) as json_file:
        data = json.load(json_file)
        return data

def circularDistance(a, b):
    return min(fmod(abs(b-a)+1, 1), fmod(b+a, 1))
# moyenne circulaire pour un cercle entre 0 et 1
def circlularMean(a, b):
    m1 = (a+b)/2
    m2 = m1+0.5
    #print(a, m1, circularDistance(a, m1))
    #print(a, m2, circularDistance(a, m1))
    if(circularDistance(a, m1) < circularDistance(a, m2)):
        return m1
    return m2

def average_color(color1, color2):
    return (
        (color1[0] + color2[0]) / 2,
        min(max(color1[1], color2[1]) + 0.0075, 1),
        (color1[2] + color2[2])/2
    )

def random_color():
    return 0.1,0,1#random.random(), 0.5 + 0*random.random(), 0.5 #random.random()

def mutate_color(color):
    #return color
    return (
        fmod(color[0]+pow(1-color[0], 2)/15, 1),#fmod(color[0] + random.random() * 0.05 - 0.025 + 1, 1),
        color[1],#min(color[1]+0.005, 0.9),#min(color[2] + random.random() * 0.05 - 0.025, 1),
        color[2],#min(color[1] + random.random() * 0.05 - 0.025, 1),
    )

def average_pos(p1, p2):
    return (
        circlularMean(p1[0], p2[0]),
        circlularMean(p1[1], p2[1])
    )

def random_position():
    return random.random(), random.random()
 
def mutate_pos(pos):
    return (
        fmod(pos[0] + random.random() * 0.02 - 0.01 + 1, 1),
        fmod(pos[1] + random.random() * 0.02 - 0.01 + 1, 1),
    )

def draw_couche_genealogique(surface, data):
    l = data
    idToPrint = []
    for i in range(len(l)):
        id = l[i][0]
        p1 = l[i][1]
        p2 = l[i][2]
        
        if p1 == 0 and p2 == 0:
            color = random_color() #(j/7, 0.5, 0.5) 
            pos = random_position()
        elif p2 == 0:
            color = mutate_color(colors[p1])
            pos = mutate_pos(coords[p1])
        else:
            color = mutate_color(average_color(colors[p1], colors[p1]))
            pos = mutate_pos(average_pos(coords[p1], coords[p2]))

        colors[id] = color
        coords[id] = pos

        if p1 == 0 and p2 == 0:
            pass
        
        idToPrint.append(id)

    def s(a): return colors[a][1]
    idToPrint.sort(key=s)
    for i in range(len(idToPrint)):
        id = idToPrint[i]
        color = colors[id]
        pos = coords[id]
        
        converted = colorsys.hls_to_rgb(color[0], color[1], color[2])
        converted = (converted[0] * 255, converted[1] * 255, converted[2] * 255)
        pg.draw.rect(surface, converted, (pos[0]*surface.get_width(), pos[1]*surface.get_height(), 5, 5))

for i in range(0, 90):
    p2 = (i/100, 0)
    p1 = (i/100+0.1, 0)
    print(average_pos(p1, p2))
#sys.exit()

file_content = read_json()

genes=file_content["genealogy"]
stats = file_content["stats"]

screen = pg.display.set_mode((0, 0), pg.FULLSCREEN)
opacityMask = pg.Surface((screen.get_width(), screen.get_height()))
opacityMask.fill((0,0,0))
opacityMask.set_alpha(1)

colors = {}
coords = {}

pg.init()

clock = pg.time.Clock()


dx = 20
dy = 5 
npersquare = 1
sqSize = screen.get_width()/dx

def drawAll():
    screen.fill(0)
    colors = {}
    coords = {}
    for y in range(dy):
        for x in range(dx):
            genR = pg.Surface((100,100))
            for i in range(npersquare):
                draw_couche_genealogique(genR, genes[dx*npersquare*y + npersquare*x + i])
            genR = pg.transform.scale(genR, (sqSize, sqSize))
            screen.blit(genR, (x*sqSize, y*sqSize))
        pg.display.flip()

running = True
animationI = 0;
animation = False
drawAll()
while running:
    clock.tick(50)
    if(animation):
        draw_couche_genealogique(screen, genes[animationI])
        pg.display.flip()
        screen.blit(opacityMask, (0,0))
        animationI+=1
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
        elif event.type == pg.KEYDOWN and event.unicode == 'r':
            animation = False
            drawAll()
        elif event.type == pg.KEYDOWN and event.unicode == 'p':
            animationI=0
            animation = True
            screen.fill((0,0,0))
