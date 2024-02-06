import dataclasses
from typing import List, Tuple, Any
from math import cos, sin, sqrt, atan2
from config import COM_SIZE, PLAYER_SIZE

@dataclasses.dataclass
class LineReader:
    index = 0
    line: list[Any]
    def step(self):
        self.index += 1
    def getSelectedValue(self):
        return self.line[self.index]
    def readValue(self):
        val = self.getSelectedValue()
        self.step()
        return val
    def readAngle(self):
        s = self.readValue()
        c = self.readValue()
        return atan2(s, c)
    def readValArray(self, size):
        tab = []
        for _ in range(size):
            tab.append(self.readValue())
        return tab
    def readArray(self, readFunc, size):
        tab = []
        for _ in range(size):
            tab.append(readFunc(self))
        return tab

@dataclasses.dataclass
class Vector:
    norme:float
    angle:float
    def get_x(self):
        return cos(self.angle)*self.norme
    def get_y(self):
        return sin(self.angle)*self.norme
    def xy(self):
        return (self.get_x(), self.get_y())
    def denormalize(self, max: float, min=0):
        self.norme = self.norme * (max+min) - min
        
    def __add__(self, other):
        return Vector.from_xy(
            self.get_x() + other.get_x(),
            self.get_y() + other.get_y(),
        )

    @staticmethod
    def deserialize(state: LineReader):
        return Vector(
            norme = state.readValue(),
            angle = state.readAngle()
        )
    @staticmethod
    def from_xy(x: int, y: int):
        return Vector(
            norme=sqrt(x*x + y*y),
            angle=atan2(y, x)
        )
    @staticmethod
    def deserializePositionnal(state: LineReader):
        return Vector.from_xy(state.readValue(), state.readValue())
        
@dataclasses.dataclass
class Ball:
    pos: Vector
    vitesse: Vector
    def denormalize(self, dmap:float, vmax:float):
        self.pos.denormalize(dmap)
        self.vitesse.denormalize(vmax)
    @staticmethod
    def deserialize(state: LineReader):
        return Ball(
            pos = Vector.deserialize(state),
            vitesse = Vector.deserialize(state)
        )

@dataclasses.dataclass
class Player():
    pos: Vector
    orientation: float
    incom : List[float]
    vrotation: float
    vitesse: Vector
    cage: Vector
    hauteur: float
    balle: Ball
    joueurAllie: Ball
    joueurAdverse: Ball
    outputRota: float
    outputAccel: float
    outcom: List[float]
    def denormalize(self, mapWidth: int, mapHeight: int, 
                    vmax: float, vballmax: float):
        dmap = sqrt(pow(mapWidth, 2) + pow(mapHeight, 2))
        d_cage = sqrt(pow(mapHeight / 2, 2) + pow(mapWidth, 2))
        
        self.hauteur *= mapHeight/2
        self.vitesse.denormalize(vmax)
        self.cage.denormalize(min = PLAYER_SIZE, max = d_cage)
        self.balle.denormalize(dmap, vmax = vballmax+vmax)
        self.joueurAllie.denormalize(dmap, vmax = 2*vmax)
        self.joueurAdverse.denormalize(dmap, vmax = 2*vmax)

    @staticmethod
    def deserialize(state: LineReader):
        return Player(
            orientation= state.readValue(),
            pos= Vector.deserializePositionnal(state),
            incom= state.readValArray(COM_SIZE),
            vrotation= state.readValue(),
            vitesse= Vector.deserialize(state),
            cage= Vector.deserialize(state),
            hauteur= state.readValue(),
            balle= Ball.deserialize(state),
            joueurAllie= Ball.deserialize(state),
            joueurAdverse= Ball.deserialize(state),
            outputRota= state.readValue(),
            outputAccel= state.readValue(),
            outcom= state.readValArray(COM_SIZE)
        )

@dataclasses.dataclass
class GameConfig:
    mapWidth: int
    mapHeight: int
    vmax: float
    vballmax: float
    teamsize: int
    
    playerNumber: int
    playerRenderLines: List[bool]
    
    playerSize: int 
    ballSize: int
    goalSize: int
    goalThickness: int
    walls: List[Tuple[Vector, Vector]]

@dataclasses.dataclass
class GameState:
    conf: GameConfig
    ball: Vector
    players: Tuple[List[Player], List[Player]]
    def denormalize(self):
        for i in range(len(self.players[0])):
            self.players[0][i].denormalize(
                self.conf.mapWidth, self.conf.mapHeight, self.conf.vmax, self.conf.vballmax)
            self.players[1][i].denormalize(
                self.conf.mapWidth, self.conf.mapHeight, self.conf.vmax, self.conf.vballmax)
    @staticmethod
    def deserialize(state: LineReader, conf: GameConfig):
        # premier paramètre??
        state.step()
        return GameState(
            conf= conf,
            ball= Vector.deserializePositionnal(state),
            players= (
                state.readArray(Player.deserialize, conf.teamsize),
                state.readArray(Player.deserialize, conf.teamsize)
            )
        )
        
            
