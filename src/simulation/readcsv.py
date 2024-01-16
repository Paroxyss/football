import csv
from math import pi, sqrt, pow, atan2
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


dmap = sqrt(pow(SCREEN_WIDTH, 2) + pow(SCREEN_HEIGHT, 2))
d_cage = sqrt(pow(SCREEN_HEIGHT / 2, 2) + pow(SCREEN_WIDTH, 2))
vmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT
vball_max = vmax * sqrt(MASS_PLAYER / MASS_BALL)


def dnmm(x, min, max):
    return x * (max - min) + min


def dnmm_a(x):
    return x * pi

def readValue(state):
    val = state["line"][state["index"]]
    state["index"] += 1
    return val

def readAngle(state):
    s = readValue(state)
    c = readValue(state)
    return atan2(s, c)

def readAngularVector(state):
    return {
        "norme": readValue(state), 
        "angle": readAngle(state)
    }

def readObject(state):
    return {
        "pos": readAngularVector(state),
        "vitesse": readAngularVector(state)
    }

def readPositionnalVector(state):
    return {
        "x": readValue(state),
        "y": readValue(state)
    }

def readArray(readFunc, size, state):
    tab = []
    for i in range(size):
        tab.append(readFunc(state))
    return tab

def readPlayer(state):
    # NE PAS CHANGER L'ORDRE, c'est l'ordre de lecture
    obj = {
                "orientation": readValue(state),
                "pos": readPositionnalVector(state),
                "vrotation": readValue(state),
                "vitesse": readAngularVector(state),
                "cage": readAngularVector(state),
                "h": readValue(state),
                "balle": readObject(state),
                "joueurAllie": readObject(state),
                "joueurAdverse": readObject(state),
                "incom": readArray(readValue, COM_SIZE, state),
                "outputs": {
                    "rota": readValue(state),
                    "accel": readValue(state),
                },
                "outcom": readArray(readValue, COM_SIZE, state)
            }
    return obj
    
def parse_normalized(line, nb_joueurs):
    data = []
    # il y a un élément vide à la fin de la ligne

    n = int((len(line) - 1) / nb_joueurs)

    state = {"line": line, "index": 0}
    for i in range(nb_joueurs):
        data.append(readPlayer(state))

    return data

def denormalize_player(player):
    player["vitesse"]["norme"] = dnmm(player["vitesse"]["norme"], 0, vmax)
    player["cage"]["norme"] = dnmm(player["cage"]["norme"], PLAYER_SIZE, d_cage)
    player["h"] *= (SCREEN_HEIGHT/2) 
    player["balle"]["pos"]["norme"] = dnmm(player["balle"]["pos"]["norme"], 0, dmap)
    player["balle"]["vitesse"]["norme"] = dnmm(player["balle"]["vitesse"]["norme"], 0, vball_max + vmax)
    player["joueurAllie"]["pos"]["norme"] = dnmm(player["joueurAllie"]["pos"]["norme"], 0, dmap)
    player["joueurAllie"]["vitesse"]["norme"] = dnmm(player["joueurAllie"]["vitesse"]["norme"], 0, 2 * vmax)
    player["joueurAdverse"]["pos"]["norme"] = dnmm(player["joueurAdverse"]["pos"]["norme"], 0, dmap)
    player["joueurAdverse"]["vitesse"]["norme"] = dnmm(player["joueurAdverse"]["vitesse"]["norme"], 0, 2 * vmax)
    
def parse(line, nb_joueurs):
    players = parse_normalized(line, nb_joueurs)
    for player in players: denormalize_player(player)
    #print(players[3]["pos"])
    return players 
