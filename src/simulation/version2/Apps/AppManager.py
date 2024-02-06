from dataclasses import dataclass
from typing import List, Tuple, TypeVar, Any
from pygame import Surface, transform
from math import cos, sin

from pygame.event import Event

from Apps.BaseApplication import BaseApplication

@dataclass
class AppInstanceDescriptor:
    start:Tuple[int, int]
    end: Tuple[int, int]
    rotation: int
    instance:BaseApplication
    def size(self):
        return (self.end[0] -self.start[0], self.end[1] - self.start[1])
    
T = TypeVar("T")

class AppManager(BaseApplication[None]):
    apps : List[AppInstanceDescriptor] = []
    # au secours Maxence comment on passe le constructeur d'une classe dans un argument paramétrisé ?
    def addApp(self, constructor : Any, x: int, y:int, endX: int, endY: int, data: Any, rotation = 0):
        app = constructor(endX-x, endY-y, data, self.globals)
        self.apps.append(AppInstanceDescriptor(
                         start=(x,y),
                         end=(endX, endY),
                         rotation = rotation,
                         instance=app
        )) 
    
    def render(self):
        destination = Surface((self.maxX, self.maxY))
        for i in range(len(self.apps)):
            # On parcours dans le sens inverse pour que la première app 
            # enregistrée soit dessinée au dessus
            app = self.apps[len(self.apps) - i - 1]
            s = app.instance.render()
            s = transform.rotate(s, app.rotation)
            s = transform.smoothscale(s, app.size())
            destination.blit(s, app.start)
        return destination
    
    def getAppsAndTrueCoords(self, x:int, y:int):
        for i in range(len(self.apps)):
            app = self.apps[i]
            # on teste si le click est dans l'app
            if app.start[0] < x and app.end[0] > x and app.start[1] < y and app.end[1] > y:
                # on calcule les coordonnées, du point de vue l'app
                inCanvasX = x - app.start[0]
                inCanvasY = y - app.start[1]
                
                size = app.instance.getTrueSize()
                r = app.rotation
                
                trueSizeX = abs(size[0] * cos(r)) + abs(size[1] * sin(r))
                trueSizeY = abs(size[1] * cos(r)) + abs(size[0] * sin(r))

                virtualSizeX = app.instance.maxX
                virtualSizeY = app.instance.maxY

                inCanvasX *= trueSizeX / virtualSizeX
                inCanvasY *= trueSizeY / virtualSizeY
                
                # /!\ support incomplet des rotations
                return (app, inCanvasX, inCanvasY)
        return (None, 0, 0)
    
    def onClick(self, x: int, y: int, kind: int):
        app, tx, ty = self.getAppsAndTrueCoords(x, y)
        if(app):
            app.instance.onClick(int(tx), int(ty), kind)
            
    def onRelease(self, x: int, y: int):
        app, tx, ty = self.getAppsAndTrueCoords(x, y)
        if(app):
            app.instance.onRelease(int(tx), int(ty))

    def onKeyPress(self, key: int):
        for i in range(len(self.apps)):
            self.apps[i].instance.onKeyPress(key)
            
    def onKeyRelease(self, key: int):
        for i in range(len(self.apps)):
            self.apps[i].instance.onKeyRelease(key)
                
    def onEvent(self, event: Event):
        for i in range(len(self.apps)):
            self.apps[i].instance.onEvent(event)
                
                
            
            
