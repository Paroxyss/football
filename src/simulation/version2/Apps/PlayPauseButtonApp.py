from typing import Tuple
from Apps.BaseApplication import BaseApplication
from pygame import Surface, draw, font 

class PlayPauseButtonApp(BaseApplication[Tuple[int,int,int]]):
    def render(self) -> Surface:
        s = self.getSurface()
        s.fill(self.data)
        draw.rect(s, (20,20,20), (0,0, s.get_width(), s.get_height()), 2)
        f = font.Font(None, 24)
        s.blit(f.render("||", True, (255,255,255)), (2,2))
        return s
    def onClick(self, x: int, y: int, kind: int):
        self.globals.dt = 1 - self.globals.dt
