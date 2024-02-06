from typing import List, Tuple
from Apps.BaseApplication import BaseApplication
from pygame import Surface

from BoardRenderer import BoardRenderer
from GameData import GameState

class ZoomedFootApp(BaseApplication[Tuple[int,int,List[GameState]]]):
    def render(self) -> Surface:
        s = self.getSurface()
        t = self.globals.time
        if(t >= len(self.data[2])):
            t = len(self.data[2])-1
        renderer = BoardRenderer(self.data[2][t])
        
        terrain = renderer.globalRender()
        
        team = self.data[0]
        id = self.data[1]
        playerPos = self.data[2][t].players[team][id].pos.xy()
        
        minX = playerPos[0]-self.maxX//2
        minY = playerPos[1]-self.maxY//2
        maxX = playerPos[0]+self.maxX//2
        maxY = playerPos[1]+self.maxY//2

        if(minX < 0):
            minX = 0
            # le scope??
            maxX+=0
            maxX = self.maxX
        if(minY < 0):
            minY = 0
            # le scope??
            maxY+=0
            maxY = self.maxY
        if(maxX > self.data[2][0].conf.mapWidth):
            maxX = self.data[2][0].conf.mapWidth
            minX = maxX - self.maxY
        if(maxY > self.data[2][0].conf.mapHeight):
            maxY = self.data[2][0].conf.mapHeight
            minY = maxY - self.maxY
        
        s.blit(terrain, (0,0), (
            minX, minY,
            maxX, maxY
        ))
        
        return s
