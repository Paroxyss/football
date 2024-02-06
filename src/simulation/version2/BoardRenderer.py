import pygame as pg
from dataclasses import dataclass
from math import sin, cos
import config
from GameData import GameState
from typing import Union
from GameData import Vector

@dataclass
class BoardRenderer:
    frame: GameState

    def idToArrayCoords(self, id: int):
        return (id//len(self.frame.players[0]),id%len(self.frame.players[0]))
    def idToPlayer(self, id: int):
        co = self.idToArrayCoords(id)
        return self.frame.players[co[0]][co[1]]

    def globalRender(self, destination: Union[pg.Surface, None] = None):
        if(destination == None):
            destination = pg.Surface((self.frame.conf.mapWidth, self.frame.conf.mapHeight))
        self.draw_field(destination)
        self.drawWalls(destination)
        self.draw_ball(destination)
        self.draw_goal(destination)
        for i in  range(self.frame.conf.playerNumber):
            self.draw_player(destination, i)
        return destination
    
    def draw_player(self, surface: pg.Surface, id: int):
        co = self.idToArrayCoords(id)
        player = self.idToPlayer(id)
        if co[0]:
            color = config.RED
        else:
            color = config.BLUE

        pg.draw.circle(surface, color, player.pos.xy(), self.frame.conf.playerSize)

        pg.draw.line(surface, config.WHITE, player.pos.xy(),
            (
                (player.pos.get_x() + cos(-player.orientation) * self.frame.conf.playerSize),
                (player.pos.get_y() - sin(-player.orientation) * self.frame.conf.playerSize),
            ), 3
        )
        
    def show_data(self, surface : pg.Surface, id: int):
        player = self.idToPlayer(id)
        x = player.pos.get_x()
        y = player.pos.get_y()
        def drawLineToObj(obj: Vector, color):
            # balle
            ep = player.pos + obj
            pg.draw.line(surface, color, player.pos.xy(), ep.xy(), 4)

        # balle
        drawLineToObj(player.balle.pos, config.WHITE)
        
        # numéro
        font = pg.font.Font(None, 24)
        surface.blit(font.render(str(id), True, config.WHITE), ((x + 30), (y - 30)))

        # cage adverse
        drawLineToObj(player.cage, config.LIGHT_BLUE)

        # ligne de décalage par rapport au centre
        pg.draw.line( surface, config.BLUE, 
                     (x, y), 
                     (x, (y + player.hauteur * (2*(self.idToArrayCoords(id)[0]<=len(self.frame.players[0])/2) - 1))), 
                     3)

        # joueur le plus proche
        drawLineToObj(player.joueurAllie.pos, config.PINK)
        drawLineToObj(player.joueurAdverse.pos, config.PINK)
        
    def drawWalls(self, surface: pg.Surface):
        for i in range(len(self.frame.conf.walls)):
            wall = self.frame.conf.walls[i]
            pg.draw.line(
                surface,
                config.WHITE,
                wall[0].xy(),
                ((wall[0].get_x() + wall[1].get_x()), (wall[0].get_y()+ wall[1].get_y())),
                2
            )
    def draw_goal(self, surface: pg.Surface):
        pg.draw.rect(surface, config.WHITE, (
                0,
                (surface.get_height() - self.frame.conf.goalSize) // 2,
                self.frame.conf.goalThickness,
                self.frame.conf.goalSize,
        ))

        pg.draw.rect( surface, config.WHITE, (
                surface.get_width() - self.frame.conf.goalThickness,
                (surface.get_height()- self.frame.conf.goalSize) // 2,
                10,
                self.frame.conf.goalSize,
        ))

    def draw_ball(self, surface: pg.Surface):
        pg.draw.circle(
            surface,
            config.BLACK,
            self.frame.ball.xy(),
            self.frame.conf.ballSize,
        )

    def draw_field(self, surface: pg.Surface):
        for i in range(config.N_BANDE):
            pg.draw.rect(
                surface,
                config.FIELD_DARK_GREEN if i % 2 == 0 else config.FIELD_GREEN,
                pg.Rect(
                    i * surface.get_width()//config.N_BANDE,
                    0,
                    surface.get_width()//config.N_BANDE + 10,
                    surface.get_height(),
                ),
            )

        pg.draw.circle(
            surface,
            config.WHITE,
            (surface.get_width()// 2, surface.get_height()// 2),
            100,
            width=5,
        )

        pg.draw.rect(
            surface,
            config.WHITE,
            pg.Rect(surface.get_width()// 2 - 2.5, 0, 5, surface.get_height()),
        )

        pg.draw.circle(
            surface,
            config.WHITE,
            (surface.get_width()// 2, surface.get_height()// 2),
            10,
        )

