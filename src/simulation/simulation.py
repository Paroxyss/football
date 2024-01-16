import pygame as pg
import sys

from functions import *
from readcsv import *
from config import *
from math import sqrt


pg.init()

running = True
clock = pg.time.Clock()

filename = "game.csv"
if len(sys.argv) >= 2:
    filename = sys.argv[1]

data = read(filename)

nb_joueurs = int(data[0][1] * 2)
walls = []
wallData = data[0][6:]

for i in range(len(wallData)//4):
    walls.append(wallData[4*i:4*i+4])

running = True
parsed = []
show = [False for _ in range(nb_joueurs)]

data = data[1:]

for i in range(0, len(data)):
    parsed.append(parse(data[i][3:], nb_joueurs))

while running:
    i = 0
    while i < len(parsed):
        draw_field()
        drawWalls(walls)
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

        clock.tick(60)

        for event in pg.event.get():
            if event.type == pg.MOUSEBUTTONDOWN:
                mx, my = pg.mouse.get_pos()
                for k in range(nb_joueurs):
                    dist = sqrt(
                        (
                            mx
                            - calcx(
                                parsed[i][k],
                                k + 1,
                                nb_joueurs,
                            )
                        )
                        ** 2
                        + (my - calcy(parsed[i][k], k + 1, nb_joueurs)) ** 2
                    )

                    if dist <= PLAYER_SIZE:
                        show[k] = not show[k]
            elif event.type == pg.KEYDOWN:
                # Check if a number key is pressed (0-9)
                if event.key in [
                    pg.K_0,
                    pg.K_1,
                    pg.K_2,
                    pg.K_3,
                    pg.K_4,
                    pg.K_5,
                    pg.K_6,
                    pg.K_7,
                    pg.K_8,
                    pg.K_9,
                ]:
                    if event.key == pg.K_0:
                        show = [False for _ in range(nb_joueurs)]
                    elif event.key in [pg.K_1, pg.K_2, pg.K_3, pg.K_4, pg.K_5, pg.K_6]:
                        show[event.key - pg.K_1] = not show[event.key - pg.K_1]

        keys=pg.key.get_pressed()
        if(keys[pg.K_LEFT]):
            i-=3 + 5*keys[pg.K_LSHIFT]
        if(keys[pg.K_RIGHT]):
            i+=2 + 5*keys[pg.K_LSHIFT]
        pg.display.flip()
        screen.fill(FIELD_GREEN)
        i+=1
        
pg.quit()
