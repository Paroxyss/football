import csv
from math import pi, sqrt, pow
from config import *


def convert(lst):
    for i in range(len(lst)):
        try:
            lst[i] = float(lst[i])
        except ValueError:
            pass

    return lst


def read(name):
    lines = []

    try:
        with open(name, "r") as f:
            reader = csv.reader(f)
            for l in reader:
                lines.append(convert(l))
    except Exception as e:
        print(str(e))

    return lines


d_map = sqrt(pow(SCREEN_WIDTH, 2) + pow(SCREEN_HEIGHT, 2))
d_cage = sqrt(pow(SCREEN_HEIGHT / 2, 2) + pow(SCREEN_WIDTH, 2))
vmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT
d_cage = sqrt(pow(SCREEN_WIDTH + GOAL_SIZE / 2, 2) + pow(SCREEN_WIDTH / 2, 2))
vball_max = vmax * sqrt(MASS_PLAYER / MASS_BALL)


def dnmm(x, min, max):
    return x * (max - min) + min


def dnmm_a(x):
    return x * pi


def parse(line, nb_joueurs):
    data = []
    # il y a un élément vide à la fin de la ligne

    n = int((len(line) - 1) / nb_joueurs)

    for i in range(nb_joueurs):
        data.append(
            {
                "orientation": line[i * n],
                "x": line[i * n + 1],
                "y": line[i * n + 2],
                "v_joueur": dnmm(line[i * n + 3], 0, vmax),
                "v_angle": dnmm_a(line[i * n + 4]),
                "cage_haut_dist": dnmm(line[i * n + 5], PLAYER_SIZE / 2, d_cage),
                "cage_haut_angle": dnmm_a(line[i * n + 6]),
                "cage_bas_dist": dnmm(line[i * n + 7], PLAYER_SIZE / 2, d_cage),
                "cage_bas_angle": dnmm_a(line[i * n + 8]),
                "ball_dist": dnmm(
                    line[i * n + 9],
                    BALL_SIZE + PLAYER_SIZE,
                    d_map - BALL_SIZE - PLAYER_SIZE,
                ),
                "ball_angle_relat": dnmm_a(line[i * n + 10]),
                "v_ball": dnmm(line[i * n + 11], 0, vball_max + vmax),
                "v_ball_angle": dnmm_a(line[i * n + 12]),
                "nearest_copain_dist": dnmm(
                    line[i * n + 13], 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE
                ),
                "nearest_copain_angle": dnmm_a(line[i * n + 14]),
                "nearest_copain_v": dnmm(line[i * n + 15], 0, 2 * vmax),
                "nearest_copain_v_angle": dnmm_a(line[i * n + 16]),
                "nearest_adv_dist": dnmm(
                    line[i * n + 17], 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE
                ),
                "nearest_adv_angle": dnmm_a(line[i * n + 18]),
                "nearest_adv_v": dnmm(line[i * n + 19], 0, 2 * vmax),
                "nearest_adv_v_angle": dnmm_a(line[i * n + 20]),
            }
        )

    return data
