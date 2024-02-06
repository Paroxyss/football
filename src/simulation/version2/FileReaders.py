import csv
from GameData import LineReader, Vector, GameConfig, GameState
from typing import List
from math import sqrt

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
            f.close()
    except Exception as e:
        print(str(e))

    return lines

def getFrames(filename = "game.csv") -> List[GameState]:
    data = read(filename)

    walls = []

    headerReader = LineReader(data[0])
    headerReader.step()
    teamSize = int(headerReader.readValue())
    mapHeight = headerReader.readValue()
    mapWidth = headerReader.readValue()
    ballSize = headerReader.readValue()
    playerSize = headerReader.readValue()

    for _ in range((len(data[0])-6)//4):
        walls.append((Vector.deserializePositionnal(headerReader), Vector.deserializePositionnal(headerReader)))

    PLAYER_ACCELERATION = 0.3
    PLAYER_FROTTEMENT = 1 / 20

    MASS_PLAYER = 100
    MASS_BALL = 40

    GOAL_SIZE = 15 * 12
    GOAL_THICKNESS = 5

    vmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT
    vball_max = vmax * sqrt(MASS_PLAYER / MASS_BALL)

    conf = GameConfig(
        mapHeight= mapHeight,
        mapWidth= mapWidth,
        vmax= vmax,
        vballmax= vball_max,
        teamsize= teamSize,

        playerNumber=teamSize*2,
        playerRenderLines=[False for _ in range(teamSize*2)],

        playerSize= playerSize,
        ballSize= ballSize,
        goalSize= GOAL_SIZE,
        goalThickness= GOAL_THICKNESS,
        walls= walls
    )

    data = data[1:]

    frames : List[GameState]= []
    for i in range(len(data)):
        frameReader = LineReader(data[i])
        frames.append(GameState.deserialize(frameReader, conf))
    
    return frames
