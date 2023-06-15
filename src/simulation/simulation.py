import pygame as pg

from functions import *
from readcsv import *
from config import *


pg.init()
running = True
clock = pg.time.Clock()
data = read("game.csv")

nb_joueurs = int(data[0][1] * 2)

for i in range(1, len(data)):
    match data[i][0]:
        case 1.0:
            clock.tick(60)
        case 2.0:
            draw_field()
            draw_goal()

            draw_ball(data[i][1], data[i][2])
            for k in range(1, nb_joueurs + 1):
                draw_player(
                    1 if k > (nb_joueurs + 1) / 2 else 2,
                    data[i][3 * k],
                    data[i][3 * k + 1],
                    data[i][3 * k + 2],
                )
        case default:
            print("error", data[i][0])

    pg.display.flip()

pg.quit()
