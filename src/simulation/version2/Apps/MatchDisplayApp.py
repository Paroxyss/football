from typing import List
from Apps.BaseApplication import BaseApplication
from pygame import Surface, K_s

from BoardRenderer import BoardRenderer
from GameData import GameState

class MatchDisplayApp(BaseApplication[List[GameState]]):
    def getTrueSize(self):
        return (self.data[0].conf.mapWidth, self.data[0].conf.mapHeight)
    def render(self) -> Surface:
        t = self.globals.time
        if(t >= len(self.data)):
            t = len(self.data)-1
        renderer = BoardRenderer(self.data[t])
        return renderer.globalRender()
    def onClick(self, x: int, y: int, kind: int):
        # scroll up
        if(kind == 4):
            self.globals.time += 3
        #scroll down
        elif(kind == 5):
            self.globals.time -= 3
            self.globals.time = max(0, self.globals.time)
    def onKeyPress(self, key: int):
        if(key == K_s):
            self.globals.team = 1- self.globals.team
