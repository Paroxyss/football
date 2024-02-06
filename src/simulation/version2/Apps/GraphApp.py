from dataclasses import dataclass
from typing import List
from Apps.BaseApplication import BaseApplication
from pygame import Surface, draw, font, SRCALPHA, BUTTON_LEFT, BUTTON_RIGHT

graphColors = [
    (255, 0, 0),
    (0, 255, 0),
    (0, 0, 255),
    (255, 255, 0),
    (255, 0, 255),
    (0, 255, 255),
    (255, 127, 127),
    (127, 255, 127),
    (127, 127, 255),
    (255, 255, 127),
    (255, 127, 255),
    (127, 255, 255),
]

@dataclass
class GraphData:
    name: str
    values: List[float]

gzoom = 4
class GraphApp(BaseApplication[List[GraphData]]):
    def onInit(self):
        self.bigSurfaces = []
        self.display = [True]*len(self.data)
        
        for i in range(len(self.data)):
            self.bigSurfaces.append(Surface((len(self.data[0].values)*gzoom, self.maxY), SRCALPHA, 32))
        self.ma = 0
        self.mi = 0
        for i in range(len(self.data)):
            values = self.data[i].values
            self.ma = max(*values, self.ma)
            self.mi = min(*values, self.mi)
        for i in range(len(self.data)):
            values = self.data[i].values
            assert len(values) == len(self.data[0].values)
            line = []
            for x in range(len(self.data[0].values)):
                y = values[x]
                line.append((
                    gzoom*x,
                    self.maxY-((y-self.mi)/(self.ma-self.mi)*self.maxY) if y != 0 else 0
                ))
            draw.lines(self.bigSurfaces[i], graphColors[i], False, line, 3)
        
    def render(self) -> Surface:
        t = self.globals.time
        if(t >= len(self.data[0].values)):
            t=len(self.data[0].values)-1
        s = self.getSurface()

        for i in range(len(self.data)):
            if(not self.display[i]):
                continue
            s.blit(self.bigSurfaces[i], (0, 0), (
                gzoom*t - s.get_width()//2, 0, 
                gzoom*t + s.get_width()  , s.get_height()))
        
        draw.line(s, (255,0,0), (s.get_width()//2, 0), (s.get_width()//2, s.get_height()),2)


        # légendes min et max
        f = font.Font(None, 24)
        s.blit(f.render(str(round(self.mi, 2)), True, (255,255,255)), (s.get_width()//2,s.get_height()-24))
        s.blit(f.render(str(round(self.ma, 2)), True, (255,255,255)), (s.get_width()//2+4,0))
        
        for i in range(len(self.data)):
            s.blit(f.render(str(self.data[i].name), True, graphColors[i] if self.display[i] else (180,180,180), (40,40,40)), (0,i*24+12))
            
        absHeight = self.ma - self.mi
        ratioDescente = self.ma / absHeight
        zeroy = self.maxY*ratioDescente

        draw.line(s, (255,255,255), (s.get_width()//2-50, zeroy), (s.get_width()//2+50, zeroy),2)
        return s
    
    def onClick(self, x: int, y: int, kind: int):
        if(kind == BUTTON_LEFT):
            if(x > 80):
                return
            y -= 12
            id = y//24
            if(id > len(self.display)):
                return
            self.display[id] = not self.display[id]
        elif(kind == BUTTON_RIGHT):
            if(not all(self.display)):
                self.display = [True]*len(self.display)
            else:
                self.display = [False]*len(self.display)
        # scroll up
        elif(kind == 4):
            self.globals.time += 3
        #scroll down
        elif(kind == 5):
            self.globals.time -= 3
            self.globals.time = max(0, self.globals.time)
