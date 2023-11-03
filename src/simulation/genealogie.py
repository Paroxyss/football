import pygame as pg
import numpy as np
import json
import random
import colorsys
from math import fmod
import random

def read_json():
    name = "stats.json"
    with open(name) as json_file:
        data = json.load(json_file)
        return data

def random_color():
    return random.random(), 0.5 + 0*random.random(), 0.5 #random.random()

def mutate(color):
    while(color[0] < 0):
        color = (color[0]+1, color[1], color[2])
    return color
    return (
        fmod(color[0] + random.random() * 0.05 - 0.025, 1),
        color[1],#min(color[1] + random.random() * 0.05 - 0.025, 1),
        color[2],#min(color[2] + random.random() * 0.05 - 0.025, 1),
    )


def draw_couche_genealogique(surface, data, x):
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
        pg.draw.rect(surface, converted, (x, i, 1, 1))
        i += 1

def make_arbre_genealogique(data):
    surface = pg.Surface((len(data), len(data[0])))
    for i in range(len(data)):
        draw_couche_genealogique(surface, data[i], i)
    return surface

def getCol(i):
    return lambda x: x[i]

def verticalGet(d,i):
    return np.fromiter(map(getCol(i), d), float)

def make_stats(data):
    res = 1000
    wres = 3
    surface = pg.Surface((len(data)*wres, res))
    lineWidth = int(len(data)/1000)
    
    colors = [(0,0,0),(0,255,0),(255,0,0),(255,255,0),(0,255,255)]
    
    for i in range(len(data)-1):
        for d in range(1,len(data[i])):
            def getDataAt(i):
                return (1-data[i][d]/maxes[d])*res
            pg.draw.line(surface, colors[d], (i*wres, getDataAt(i)), ((i+1)*wres, getDataAt(i+1)), lineWidth)
    return surface
    
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

genes=file_content["genealogy"]
stats = file_content["stats"]

screen_width = 1400
screen_height = 650


running = True
n_gen = len(genes)
population_size = len(genes[0])
hauteur = screen_height/population_size/2
largeur = screen_width/n_gen
red = (255, 255, 255)
screen = pg.display.set_mode((screen_width, screen_height))
ids = {}
maxes = [
    np.max(verticalGet(stats, 0)),
    np.max(verticalGet(stats, 1)),
    np.max(verticalGet(stats, 2)),
    np.max(verticalGet(stats, 3)),
    np.max(verticalGet(stats, 4)),
]

pg.init()

arbre = make_arbre_genealogique(genes)
stats = make_stats(stats)

displaySegment = (0, len(genes))
displayRatio = 1/2
def displayD(startG, endG):
    global displaySegment
    trajet = (startG, endG) if (startG < endG) else (endG, startG)
    displaySegment = (trajet[0], trajet[1])
    print(displaySegment)
    
    subArbre = pg.Surface((trajet[1] - trajet[0], arbre.get_height()))
    subArbre.blit(arbre, (-trajet[0], 0))
    subArbre = pg.transform.scale(subArbre, (screen_width, screen_height*displayRatio))
    
    subStats = pg.Surface(((trajet[1] - trajet[0]) * stats.get_width() / arbre.get_width(), stats.get_height()))
    subStats.blit(stats, (-trajet[0] * stats.get_width() / arbre.get_width(), 0))
    subStats = pg.transform.smoothscale(subStats, (screen_width, screen_height*(1-displayRatio)))
    
    screen.fill((0,0,0))
    screen.blit(subArbre, (0,0))
    screen.blit(subStats, (0,screen_height*displayRatio))

startPos = (0,0)
endPos = (len(genes),0)


displayD(0, len(genes))
zoomPile = []

def regenaAll():
    global ids
    global arbre
    ids = {}
    arbre = make_arbre_genealogique(genes)
    displayD(displaySegment[0], displaySegment[1])

mvspeed = 0.03
clock = pg.time.Clock()
while running:
    pg.display.flip()
    clock.tick(20)
  
    pressed = pg.key.get_pressed()
    zFactor = displaySegment[1]-displaySegment[0]
    if pressed[pg.K_h]:
        displaySegment = (displaySegment[0] - mvspeed*zFactor, displaySegment[1] - mvspeed*zFactor)
        displayD(displaySegment[0], displaySegment[1])
    if pressed[pg.K_l]:
        displaySegment = (displaySegment[0] + mvspeed*zFactor, displaySegment[1] + mvspeed*zFactor)
        displayD(displaySegment[0], displaySegment[1])
    if pressed[pg.K_j]:
        displaySegment = (displaySegment[0] - mvspeed*zFactor, displaySegment[1] + mvspeed*zFactor)
        displayD(displaySegment[0], displaySegment[1])
    if pressed[pg.K_k]:
        displaySegment = (displaySegment[0] + mvspeed*zFactor, displaySegment[1] - mvspeed*zFactor)
        displayD(displaySegment[0], displaySegment[1])
        
    for event in pg.event.get():
        print(event)
        if event.type == pg.QUIT:
            running = False
        elif event.type == pg.MOUSEBUTTONDOWN:
            startPos = event.pos
        elif event.type == pg.MOUSEBUTTONUP:
            endPos = event.pos
            lengthFactor = (displaySegment[1] - displaySegment[0])/screen_width
            zoom = (
                startPos[0]*lengthFactor+displaySegment[0], 
                endPos[0]*lengthFactor+displaySegment[0]
            )
            zoomPile.append(displaySegment)
            displaySegment = zoom
            displayD(zoom[0], zoom[1])
        elif event.type == pg.KEYDOWN and event.key == 8:
            if len(zoomPile) == 0:
                displayD(0, len(genes))
            else:
                zoom = zoomPile.pop()
                displayD(zoom[0], zoom[1])
        elif event.type == pg.KEYDOWN and event.unicode == 'r':
            regenaAll()
        # 81: flèche bas, 82: flèche haut
        elif event.type == pg.KEYDOWN and event.scancode == 81:
            displayRatio += 0.1
            displayRatio = min(1, displayRatio)
            displayD(displaySegment[0], displaySegment[1])
        elif event.type == pg.KEYDOWN and event.scancode == 82:
            displayRatio -= 0.1
            displayRatio = max(0, displayRatio)
            displayD(displaySegment[0], displaySegment[1])
