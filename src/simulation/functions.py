import pygame as pg
from math import cos, sin
from config import *


def draw_player(t, x, y, d):
    if t == "droite":
        color = RED
    else:
        color = BLUE

    pg.draw.circle( screen, color, (x, y), PLAYER_SIZE)

    pg.draw.line( screen, WHITE, (x, y),
        (
            x + cos(-d) * PLAYER_SIZE,
            y - sin(-d) * PLAYER_SIZE,
        ), 2
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
    x = 10
    y = 10

    for _ in range(4):
        pg.draw.rect(screen, (255, 255, 255), (x, y, 200, 15), 1)

    pg.draw.rect(screen, (255, 255, 255), (x, y, 2 * (p / max * 100), 15), 15)


def show_data(infos, k, nb_joueurs):
    x = infos["pos"]["x"]
    y = infos["pos"]["y"]
    ori = infos["orientation"]

    # balle
    ballePos = infos["balle"]["pos"]
    ep = (
        x + cos(ballePos["angle"] + ori) * ballePos["norme"],
        y + sin(ballePos["angle"] + ori) * ballePos["norme"]
    )
    pg.draw.line(screen, WHITE, (x, y), ep, 4)

    # numéro

    font = pg.font.Font(None, 24)

    screen.blit(font.render(str(k), True, WHITE), (x + 30, y - 30))

    # cage adverse
    cagePos = infos["cage"]
    ep = (
        x + cos(cagePos["angle"] + ori) * cagePos["norme"],
        y + sin(cagePos["angle"] + ori) * cagePos["norme"]
    )

    pg.draw.line(
        screen,
        LIGHT_BLUE,
        (x, y),
        ep,
        3,
    )


    pg.draw.line(
        screen,
        BLUE,
        (x, y),
        (x, y + infos["h"] * (2*(k<=nb_joueurs/2) - 1)),
        3,
    )

    # joueur le plus proche

    copainPos = infos["joueurAllie"]["pos"]
    ep = (
        x + cos(copainPos["angle"] + ori) * copainPos["norme"],
        y + sin(copainPos["angle"] + ori) * copainPos["norme"]
    )
    pg.draw.line( screen, PINK, (x, y), ep, 2)

    
    advPos = infos["joueurAdverse"]["pos"]
    ep = (
        x + cos(advPos["angle"] + ori) * advPos["norme"],
        y + sin(advPos["angle"] + ori) * advPos["norme"]
    )

    pg.draw.line( screen, PURPLE, (x, y), ep, 2)


def draw_goal():
    pg.draw.rect(
        screen,
        WHITE,
        (
            0,
            (SCREEN_HEIGHT - GOAL_SIZE) // 2,
            GOAL_THICKNESS,
            GOAL_SIZE,
        ),
    )

    pg.draw.rect(
        screen,
        WHITE,
        (
            SCREEN_WIDTH - GOAL_THICKNESS,
            (SCREEN_HEIGHT - GOAL_SIZE) // 2,
            10,
            GOAL_SIZE,
        ),
    )


def draw_ball(x, y):
    pg.draw.circle(
        screen,
        BLACK,
        (x, y),
        BALL_SIZE,
    )


def draw_field():
    for i in range(N_BANDE):
        pg.draw.rect(
            screen,
            FIELD_DARK_GREEN if i % 2 == 0 else FIELD_GREEN,
            pg.Rect(
                i * SCREEN_WIDTH // N_BANDE,
                0,
                SCREEN_WIDTH // N_BANDE,
                SCREEN_HEIGHT,
            ),
        )

    pg.draw.circle(
        screen,
        WHITE,
        (SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2),
        100,
        width=5,
    )

    pg.draw.rect(
        screen,
        WHITE,
        pg.Rect(SCREEN_WIDTH // 2 - 2.5, 0, 5, SCREEN_HEIGHT),
    )

    pg.draw.circle(
        screen,
        WHITE,
        (SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2),
        10,
    )
