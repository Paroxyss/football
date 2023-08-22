import csv
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
    except FileNotFoundError:
        print("mskn")
    except Exception as e:
        print(str(e))

    return lines


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
                "vx": line[i * n + 3],
                "vy": line[i * n + 4],
                "ball_dist": line[i * n + 5],
                "ball_angle": line[i * n + 6],
                "cage_dist": line[i * n + 7],
                "cage_angle": line[i * n + 8],
                "nearest_dist": line[i * n + 9],
                "nearest_angle": line[i * n + 10],
                "com1": line[i * n + 11],
                "com2": line[i * n + 12],
            }
        )

    return data
