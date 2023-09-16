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
                "x": dnmm(line[i * n + 1], PLAYER_SIZE, SCREEN_WIDTH - PLAYER_SIZE),
                "y": dnmm(line[i * n + 2], PLAYER_SIZE, SCREEN_HEIGHT - PLAYER_SIZE),
                "v": dnmm(line[i * n + 3], 0, 8),
                "v_angle": dnmm_a(line[i * n + 4]),
                "ball_dist": dnmm(
                    line[i * n + 5],
                    BALL_SIZE + PLAYER_SIZE,
                    d_map - BALL_SIZE - PLAYER_SIZE,
                ),
                "ball_angle": dnmm_a(line[i * n + 6]),
                "cage_dist": dnmm(line[i * n + 7], PLAYER_SIZE, d_cage - PLAYER_SIZE),
                "cage_angle": dnmm_a(line[i * n + 8]),
                "nearest_dist": dnmm(
                    line[i * n + 9], 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE
                ),
                "nearest_angle": dnmm_a(line[i * n + 10]),
            }
        )

    return data
