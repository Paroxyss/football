from Apps.BaseApplication import BaseApplication
from pygame import Surface, draw

class PlaceholderApp(BaseApplication[None]):
    clicks = []
    def render(self) -> Surface:
        s = Surface((self.maxX, 2*self.maxY))
        s.fill((255,0,255))
        for i in range(len(self.clicks)):
            draw.circle(s, (255,255,0), self.clicks[i], 20)
        return s
    def getTrueSize(self):
        return (self.maxX, 2*self.maxY)
    def onClick(self, x: int, y: int, kind: int):
        self.clicks.append((x, y))
    
            
