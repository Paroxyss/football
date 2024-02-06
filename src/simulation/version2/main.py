import pygame as pg
import sys
from subprocess import run

from GameData import *
from BoardRenderer import *
from FileReaders import getFrames
from Apps.MatchDisplayApp import MatchDisplayApp
from Apps.TimelineApp import TimelineApp
from Apps.ButtonApp import ButtonApp
from Apps.GraphApp import GraphApp, GraphData
from Apps.PlayPauseButtonApp import PlayPauseButtonApp
from Apps.RootApp import RootApp
from Apps.ButtonPannelApp import ButtonPannelApp
from Apps.ZoomedFootApp import ZoomedFootApp
from Apps.SeparatorApp import SeparatorApp


running = True
clock = pg.time.Clock()

filename = "game.csv"
if len(sys.argv) >= 2:
    filename = sys.argv[1]

walls = []

screen = pg.display.set_mode((1400, 800))


pg.init()
rootApp = RootApp(screen)

reloadFunc = None
def commandFunc(command):
    def a():
        global reloadFunc
        run(command)
        reloadFunc = load
    return a
    
def genGetOutputs(team: int, id: int):
    def g(f: GameState):
        return f.players[team][id].outcom
    return g

def genGetInputs(team: int, id: int):
    def g(f: GameState):
        return f.players[team][id].incom
    return g

def gOut(f: List[GameState], team: int, player:int, property: str):
    return GraphData(property, list(map(lambda p: p.players[team][player].__getattribute__(property),f)))

def swapTeam():
    rootApp.globalInfos.team = 1 - rootApp.globalInfos.team
    load(False)
    
def load(restart = True):
    if(restart):
        rootApp.globalInfos.time = 0
    team = rootApp.globalInfos.team
    frames = getFrames("game.csv")
    conf = frames[0].conf
    largeurTerrain = int(conf.mapHeight/conf.mapWidth*800)
    rootApp.clearApps()
    rootApp.addApp(MatchDisplayApp, 0, 0, largeurTerrain, 800, frames, rotation=90 + 180*team)
    rootApp.addApp(TimelineApp, largeurTerrain+30, 800-30, 1400, 800, len(frames))
    rootApp.addApp(PlayPauseButtonApp, largeurTerrain, 800-30, largeurTerrain+30, 800, len(frames))

    rootApp.addApp(ButtonPannelApp, largeurTerrain, 800-30-60, 1400, 800-30,[
        ("SwapTeam", (255,30,255), swapTeam),
        ("FullRand", (255,127,0), commandFunc(["./a.out", "rm"])),
        ("Random", (0,255,255), commandFunc(["./a.out", "random", "pops/latest-backup"])),
        ("Seegoal", (127,127,255), commandFunc(["./a.out", "seegoal", "pops/latest-backup"])),
        ("Reload", (127,127,0), load),
        ("Reload", (255,127,255), load),
    ])
    
    rootApp.addApp(SeparatorApp, largeurTerrain, 0, 1400, 5, (255,255,255))
    for i in range(3):
        output = list(zip(*map(genGetOutputs(team,i),frames)))
        input = list(zip(*map(genGetInputs(team,i),frames)))
        output = [GraphData("Out1", output[0]), GraphData("Out2", output[1]),
                  GraphData("In1", input[0]), GraphData("In2", input[1]),
                  gOut(frames, team, i, "outputRota"),
                  gOut(frames, team, i, "outputAccel"),
                  ]
        rootApp.addApp(GraphApp, largeurTerrain, 205*i+5, 1400-200, 205*(i+1), output)
        rootApp.addApp(ZoomedFootApp, 1400-200, 205*i+5, 1400, 205*(i+1), (team, i, frames), 90+180*team)
        rootApp.addApp(SeparatorApp, largeurTerrain, 205*(i+1), 1400, 205*(i+1)+5, (255,255,255))

reloadFunc = load

clock = pg.time.Clock()
while(1):
    if reloadFunc:
        reloadFunc()
        reloadFunc = None
    clock.tick(60)
    rootApp.tick()
    rootApp.globalInfos.time += rootApp.globalInfos.dt
    keys = pg.key.get_pressed()
    if keys[pg.K_w]:
        clock.tick(20)


