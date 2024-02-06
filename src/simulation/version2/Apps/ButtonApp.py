from types import FunctionType
from typing import Tuple
from Apps.BaseApplication import BaseApplication
from pygame import Surface, draw, font, BUTTON_LEFT 

class ButtonApp(BaseApplication[Tuple[str, Tuple[int,int,int],FunctionType]]):
    def render(self) -> Surface:
        s = self.getSurface()
        s.fill(self.data[1])
        draw.rect(s, (20,20,20), (0,0, s.get_width(), s.get_height()), 2)
        f = font.Font(None, 24)
        s.blit(f.render(self.data[0], True, (255,255,255)), (2,2))
        return s
    def onClick(self, x: int, y: int, kind: int):
        if(kind == BUTTON_LEFT):
            self.data[2]()
