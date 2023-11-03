import pygame as pg
from math import cos, sin
from config import *


def draw_player(t, x, y, d):
    if t == "droite":
        color = RED
    else:
        color = BLUE

    pg.draw.circle( screen, color, (x * SCREEN_FACTOR, y * SCREEN_FACTOR), PLAYER_SIZE * SCREEN_FACTOR)

    pg.draw.line( screen, WHITE, (x * SCREEN_FACTOR, y * SCREEN_FACTOR),
        (
            (x + cos(-d) * PLAYER_SIZE) * SCREEN_FACTOR,
            (y - sin(-d) * PLAYER_SIZE) * SCREEN_FACTOR,
        ), int(SCREEN_FACTOR) + 1
    )


def calcx(infos, k, nb_joueurs):
    if k <= nb_joueurs / 2:
        return infos["pos"]["x"]
    return SCREEN_WIDTH - infos["pos"]["x"]
    
def calcy(infos, k, nb_joueurs):
    if k <= nb_joueurs / 2:
        return infos["pos"]["y"]
    return SCREEN_WIDTH - infos["pos"]["y"]


def progression_bar(p, max):
    x = 10 * SCREEN_FACTOR
    y = 10 * SCREEN_FACTOR

    for _ in range(4):
        pg.draw.rect(screen, (255, 255, 255), (x, y, 200 * SCREEN_FACTOR, 10 * SCREEN_FACTOR + 5), 1)

    pg.draw.rect(screen, (255, 255, 255), (x, y, 2 * (p / max * 100) * SCREEN_FACTOR, 10 * SCREEN_FACTOR + 5), int(10 * SCREEN_FACTOR) + 5)


def show_data(infos, k, nb_joueurs):
    x = infos["pos"]["x"] * SCREEN_FACTOR
    y = infos["pos"]["y"] * SCREEN_FACTOR
    ori = infos["orientation"]

    # balle
    ballePos = infos["balle"]["pos"]
    ep = (
        (x + cos(ballePos["angle"] + ori) * ballePos["norme"]) * SCREEN_FACTOR,
        (y + sin(ballePos["angle"] + ori) * ballePos["norme"]) * SCREEN_FACTOR
    )
    pg.draw.line(screen, WHITE, (x * SCREEN_FACTOR, y * SCREEN_FACTOR), ep, 4 * SCREEN_FACTOR)

    # numéro

    font = pg.font.Font(None, 24)

    screen.blit(font.render(str(k), True, WHITE), ((x + 30) * SCREEN_FACTOR,(y - 30)*SCREEN_FACTOR))

    # cage adverse
    cagePos = infos["cage"]
    ep = (
        (x + cos(cagePos["angle"] + ori) * cagePos["norme"]) * SCREEN_FACTOR,
        (y + sin(cagePos["angle"] + ori) * cagePos["norme"]) * SCREEN_FACTOR
    )

    pg.draw.line(
        screen,
        LIGHT_BLUE,
        (x * SCREEN_FACTOR, y * SCREEN_FACTOR),
        ep * SCREEN_FACTOR,
        3 * SCREEN_FACTOR,
    )


    pg.draw.line(
        screen,
        BLUE,
        (x * SCREEN_FACTOR, y * SCREEN_FACTOR),
        (x * SCREEN_FACTOR, (y + infos["h"] * (2*(k<=nb_joueurs/2) - 1)) * SCREEN_FACTOR),
        3 * SCREEN_FACTOR,
    )

    # joueur le plus proche

    copainPos = infos["joueurAllie"]["pos"]
    ep = (
        (x + cos(copainPos["angle"] + ori) * copainPos["norme"]) * SCREEN_FACTOR,
        (y + sin(copainPos["angle"] + ori) * copainPos["norme"]) * SCREEN_FACTOR
    )
    pg.draw.line( screen, PINK, (x, y), ep, 2 * SCREEN_FACTOR)

    
    advPos = infos["joueurAdverse"]["pos"]
    ep = (
        (x + cos(advPos["angle"] + ori) * advPos["norme"]) * SCREEN_FACTOR,
        (y + sin(advPos["angle"] + ori) * advPos["norme"]) * SCREEN_FACTOR
    )

    pg.draw.line( screen, PURPLE, (x * SCREEN_FACTOR, y * SCREEN_FACTOR), ep, 2 * SCREEN_FACTOR)


def drawWalls(walls):
    for i in range(len(walls)):
        pg.draw.line(
            screen,
            WHITE,
            (walls[i][0]*SCREEN_FACTOR, walls[i][1]*SCREEN_FACTOR),
            ((walls[i][0]+ walls[i][2])*SCREEN_FACTOR, (walls[i][1]+ walls[i][3])*SCREEN_FACTOR),
            2*SCREEN_FACTOR
        )
def draw_goal():
    pg.draw.rect(
        screen,
        WHITE,
        (
            0,
            (SCREEN_HEIGHT * SCREEN_FACTOR - GOAL_SIZE * SCREEN_FACTOR) // 2,
            GOAL_THICKNESS * SCREEN_FACTOR,
            GOAL_SIZE * SCREEN_FACTOR,
        ),
    )

    pg.draw.rect(
        screen,
        WHITE,
        (
            SCREEN_WIDTH * SCREEN_FACTOR - GOAL_THICKNESS * SCREEN_FACTOR,
            (SCREEN_HEIGHT * SCREEN_FACTOR - GOAL_SIZE * SCREEN_FACTOR) // 2,
            10 * SCREEN_FACTOR,
            GOAL_SIZE * SCREEN_FACTOR,
        ),
    )


def draw_ball(x, y):
    pg.draw.circle(
        screen,
        BLACK,
        (x* SCREEN_FACTOR, y* SCREEN_FACTOR),
        BALL_SIZE * SCREEN_FACTOR,
    )


def draw_field():
    for i in range(N_BANDE):
        pg.draw.rect(
            screen,
            FIELD_DARK_GREEN if i % 2 == 0 else FIELD_GREEN,
            pg.Rect(
                int(i * SCREEN_WIDTH * SCREEN_FACTOR)// N_BANDE,
                0,
                int(SCREEN_WIDTH * SCREEN_FACTOR)// N_BANDE,
                SCREEN_HEIGHT * SCREEN_FACTOR,
            ),
        )

    pg.draw.circle(
        screen,
        WHITE,
        (int(SCREEN_WIDTH * SCREEN_FACTOR)// 2, int(SCREEN_HEIGHT * SCREEN_FACTOR)// 2),
        int(100* SCREEN_FACTOR),
        width=int(5 * SCREEN_FACTOR),
    )

    pg.draw.rect(
        screen,
        WHITE,
        pg.Rect(int(SCREEN_WIDTH * SCREEN_FACTOR)// 2 - 2.5, 0, int(5 * SCREEN_FACTOR), int(SCREEN_HEIGHT * SCREEN_FACTOR)),
    )

    pg.draw.circle(
        screen,
        WHITE,
        (int(SCREEN_WIDTH * SCREEN_FACTOR)// 2, int(SCREEN_HEIGHT* SCREEN_FACTOR)// 2),
        int(10* SCREEN_FACTOR),
    )
