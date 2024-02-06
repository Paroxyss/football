from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Generic, TypeVar

from pygame import Surface
from pygame.event import Event

@dataclass
class GlobalInfos:
    time = 0
    dt = 1
    team = 0

T = TypeVar("T")

class BaseApplication(ABC, Generic[T]):
    maxX: int
    maxY: int
    data: T
    globals: GlobalInfos
    def __init__(self, maxX: int, maxY: int, data: T, globals: GlobalInfos) -> None:
        self.maxX= maxX
        self.maxY= maxY
        self.data= data
        self.globals= globals
        self.onInit()

    def onInit(self):
        pass
    def onClick(self, x: int, y:int, kind: int):
        pass
    def onRelease(self, x: int, y:int):
        pass

    def onKeyPress(self, key: int):
        pass
    def onKeyRelease(self, key: int):
        pass
    def onEvent(self, event: Event):
        pass
        
    # renvoie la taille de la surface qui sera renvoyée par l'appli, pour interpoler les clicks
    def getTrueSize(self):
        return (self.maxX, self.maxY)
    
    def getSurface(self):
        return Surface(self.getTrueSize())
        
    @abstractmethod
    def render(self) -> Surface: pass
    
    
