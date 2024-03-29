import pygame as pg

from functions import *
from readcsv import *
from config import *
from math import sqrt


pg.init()

running = True
clock = pg.time.Clock()
data = read("game.csv")
nb_joueurs = int(data[0][1] * 2)
running = True
parsed = []
show = [False for _ in range(nb_joueurs)]

for i in range(1, len(data)):
    if data[i][0] == 2.0:
        parsed.append(parse(data[i][3:], nb_joueurs))

while running:
    for i in range(1, len(parsed)):
        lineType = data[i][0]
        if(lineType == 1.0):
            clock.tick(60)
        elif(lineType == 2.0):
            draw_field()
            draw_goal()
            progression_bar(i, len(parsed))
            draw_ball(data[i][1], data[i][2])

            for k in range(1, nb_joueurs + 1):
                if show[k - 1]:
                    show_data(parsed[i][k - 1], k, nb_joueurs)

                draw_player(
                    "droite" if k > nb_joueurs / 2 else "gauche",
                    parsed[i][k - 1]["x"],
                    parsed[i][k - 1]["y"],
                    parsed[i][k - 1]["orientation"],
                )
        else:
            print("error", data[i][0])

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

        pg.display.flip()

pg.quit()
