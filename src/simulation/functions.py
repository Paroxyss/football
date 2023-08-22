import pygame as pg
from math import cos, sin
from config import *


def draw_player(t, x, y, d):
    if t == "droite":
        x = SCREEN_WIDTH - x

    pg.draw.circle(
        screen,
        RED if t == "droite" else BLUE,
        (
            x if t == "droite" else x,
            SCREEN_HEIGHT - y,
        ),
        PLAYER_SIZE,
    )

    pg.draw.line(
        screen,
        WHITE,
        (
            x if t == "droite" else x,
            SCREEN_HEIGHT - y,
        ),
        (
            x + cos(d) * PLAYER_SIZE,
            SCREEN_HEIGHT - y - sin(d) * PLAYER_SIZE,
        ),
        2,
    )


def calcx(infos, k, nb_joueurs):
    if k <= nb_joueurs / 2:
        return infos["x"]
    return SCREEN_WIDTH - infos["x"]


def show_data(infos, k, nb_joueurs):
    y = infos["y"]
    x = calcx(infos, k, nb_joueurs)
    side = "droite" if k > nb_joueurs / 2 else "gauche"

    # balle

    epy = y + sin(infos["ball_angle"]) * infos["ball_dist"]
    if side == "gauche":
        epx = x + cos(infos["ball_angle"]) * infos["ball_dist"]
    else:
        epx = x - cos(infos["ball_angle"]) * infos["ball_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(screen, WHITE, (x, y), ep, 4)

    # numéro

    font = pg.font.Font(None, 24)

    screen.blit(font.render(str(k), True, WHITE), (x + 30, y - 30))

    # cage adverse

    epy = y + sin(infos["cage_angle"]) * infos["cage_dist"]
    if side == "gauche":
        epx = x + cos(infos["cage_angle"]) * infos["cage_dist"]
    else:
        epx = x - cos(infos["cage_angle"]) * infos["cage_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        BLUE,
        (x, y),
        ep,
        3,
    )

    # joueur le plus proche

    epy = y + sin(infos["nearest_angle"]) * infos["nearest_dist"]
    if side == "gauche":
        epx = x + cos(infos["nearest_angle"]) * infos["nearest_dist"]
    else:
        epx = x - cos(infos["nearest_angle"]) * infos["nearest_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        PINK,
        (x, y),
        ep,
        2,
    )


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
        YELLOW,
        (x, SCREEN_HEIGHT - y),
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
