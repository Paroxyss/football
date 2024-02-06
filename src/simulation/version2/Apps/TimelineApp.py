from Apps.BaseApplication import BaseApplication
from pygame import Surface, draw, locals

class TimelineApp(BaseApplication[int]):
    def render(self) -> Surface:
        t = self.globals.time / self.data
        t = min(1, t)
        
        s = self.getSurface()
        s.fill((20,20,20))
        draw.rect(s, (255,0,0), (0, 0, self.maxX*t, self.maxY))
        return s
    def onClick(self, x: int, y: int, kind: int):
        if(kind == locals.BUTTON_LEFT):
            t = x / self.maxX * self.data
            self.globals.time = int(t)
        # scroll up
        elif(kind == 4):
            self.globals.time += 3
        #scroll down
        elif(kind == 5):
            self.globals.time -= 3
            self.globals.time = max(0, self.globals.time)
        
    def onKeyPress(self, key: int):
        if(key == locals.K_RIGHT):
            self.globals.dt = 2
        elif(key == locals.K_LEFT):
            self.globals.dt = -2
        elif(key == locals.K_SPACE):
            if self.globals.dt == 0:
                self.globals.dt = 1
            else: 
                self.globals.dt = 0
    def onKeyRelease(self, key: int):
        if(key == locals.K_RIGHT):
            self.globals.dt = 1
        if(key == locals.K_LEFT):
            self.globals.dt = 1
