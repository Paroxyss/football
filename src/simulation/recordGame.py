import pygame as pg
import sys
import os

os.environ["SCREEN_FACTOR"] = "0.1"
from functions import *
from readcsv import *
from config import *
from math import sqrt

from pygameRecorder import ScreenRecorder

pg.init()

running = True
clock = pg.time.Clock()

filename = "game.csv"
if len(sys.argv) >= 2:
    filename = sys.argv[1]

data = read(filename)

nb_joueurs = int(data[0][1] * 2)
running = True
parsed = []
show = [False for _ in range(nb_joueurs)]

data = data[1:]

recorder = ScreenRecorder(int(SCREEN_WIDTH*0.1), int(SCREEN_HEIGHT*0.1), 60)

for i in range(0, len(data)):
    parsed.append(parse(data[i][3:], nb_joueurs))

    
def clear_line(n=1):
    LINE_UP = '\033[1A'
    LINE_CLEAR = '\x1b[2K'
    for i in range(n):
        print(LINE_UP, end=LINE_CLEAR)

print()
for i in range(0, len(parsed), 3):
    draw_field()
    draw_goal()
    progression_bar(i, len(parsed))

    font = pg.font.Font(None, 24)
    screen.blit(font.render(str(i), True, WHITE), (SCREEN_WIDTH - 50, 30))

    draw_ball(data[i][1], data[i][2])

    for k in range(1, nb_joueurs + 1):
        if show[k - 1]:
            show_data(parsed[i][k - 1], k, nb_joueurs)
        draw_player(
            "droite" if k > nb_joueurs / 2 else "gauche",
            parsed[i][k - 1]["pos"]["x"],
            parsed[i][k - 1]["pos"]["y"],
            parsed[i][k - 1]["orientation"],
        )
    clear_line()
    print("simulation : "+str(i/len(parsed)*100)+"%")
    
    pg.display.flip()
    recorder.capture_frame(screen)
    screen.fill(FIELD_GREEN)

recorder.end_recording()
pg.quit()
