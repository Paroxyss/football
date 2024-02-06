from typing import Any
import sys
import pygame as pg
from Apps.AppManager import AppManager
from Apps.BaseApplication import GlobalInfos

class RootApp:
    screen: pg.Surface
    manager: AppManager    
    globalInfos: GlobalInfos
    def __init__(self, screen: pg.Surface, globals: GlobalInfos = GlobalInfos()) -> None:
        self.screen = screen
        self.manager = AppManager(screen.get_width(), screen.get_height(), None, globals)
        self.globalInfos = globals
    def addApp(self, constructor : Any, x: int, y:int, endX: int, endY: int, data: Any, rotation = 0):
        self.manager.addApp(constructor, x, y, endX, endY, data, rotation)

    def clearApps(self):
        if self.manager.apps:
            del self.manager.apps
        self.manager.apps = []
        
    def tick(self):
        for event in pg.event.get():
            if event.type == pg.MOUSEBUTTONDOWN:
                pos = pg.mouse.get_pos()
                self.manager.onClick(pos[0], pos[1], event.button)
            if event.type == pg.QUIT:
                pg.quit()
                sys.exit(0)
            if event.type == pg.KEYDOWN:
                self.manager.onKeyPress(event.key)
            if event.type == pg.KEYUP:
                self.manager.onKeyRelease(event.key)
            self.manager.onEvent(event)
        self.screen.blit(self.manager.render(), (0,0))
        pg.display.flip()
