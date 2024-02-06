from Apps.BaseApplication import BaseApplication
from pygame import Surface
from typing import Tuple

class SeparatorApp(BaseApplication[Tuple[int,int,int]]):
    def render(self) -> Surface:
        s = Surface((self.maxX, self.maxY))
        s.fill(self.data)
        return s
    
            
