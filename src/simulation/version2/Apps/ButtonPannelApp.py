from types import FunctionType
from typing import Tuple, List
from Apps.BaseApplication import BaseApplication
from pygame import Surface

from Apps.AppManager import AppManager
from Apps.ButtonApp import ButtonApp 

class ButtonPannelApp(BaseApplication[
                      List[Tuple[str, Tuple[int,int,int],FunctionType]
                      ]]):
    def onInit(self):
        self.manager = AppManager(self.maxX, self.maxY, None, self.globals)
        largeurParBouton = self.maxX/len(self.data)
        if(largeurParBouton > 2*self.maxY):
            largeurParBouton = 2*self.maxY
        for i in range(len(self.data)):
            self.manager.addApp(ButtonApp, int(largeurParBouton*i), 0, int(largeurParBouton*(i+1)), self.maxY, 
                                self.data[i])
    def render(self) -> Surface:
        return self.manager.render()
    def onClick(self, x: int, y: int, kind: int):
        self.manager.onClick(x, y, kind)
