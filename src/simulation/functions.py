import pygame as pg
from math import cos, sin
from config import *


def draw_player(t, x, y, d):
    if t == "droite":
        x = x
        y = y
        color = RED
        d = d
    else:
        color = BLUE

    pg.draw.circle(
        screen,
        color,
        (x, y),
        PLAYER_SIZE,
    )

    pg.draw.line(
        screen,
        WHITE,
        (x, y),
        (
            x + cos(-d) * PLAYER_SIZE,
            y - sin(-d) * PLAYER_SIZE,
        ),
        2,
    )


def calcx(infos, k, nb_joueurs):
    if k <= nb_joueurs / 2:
        return infos["x"]
    return SCREEN_WIDTH - infos["x"]
    
def calcy(infos, k, nb_joueurs):
    if k <= nb_joueurs / 2:
        return infos["y"]
    return SCREEN_WIDTH - infos["y"]


def progression_bar(p, max):
    x = 10
    y = 10

    for _ in range(4):
        pg.draw.rect(screen, (255, 255, 255), (x, y, 200, 15), 1)

    pg.draw.rect(screen, (255, 255, 255), (x, y, 2 * (p / max * 100), 15), 15)


def show_data(infos, k, nb_joueurs):
    x = infos["x"]
    y = infos["y"]

    # balle
    epx = x + cos(infos["ball_angle_relat"] + infos["orientation"]) * infos["ball_dist"]
    epy = y + sin(infos["ball_angle_relat"] + infos["orientation"]) * infos["ball_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(screen, WHITE, (x, y), ep, 4)

    # numéro

    font = pg.font.Font(None, 24)

    screen.blit(font.render(str(k), True, WHITE), (x + 30, y - 30))

    # cage adverse

    # pour epy je ne suis pas sur, peut être que je corrige un problème
    # qui provient en fait du cpp mais je ne sais pas...
    epx = x + cos(infos["cage_haut_angle"] + infos["orientation"]) * infos["cage_haut_dist"]
    epy = y + sin(infos["cage_haut_angle"] + infos["orientation"]) * infos["cage_haut_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        BLUE,
        (x, y),
        ep,
        3,
    )

    epx = x + cos(infos["cage_bas_angle"] + infos["orientation"]) * infos["cage_bas_dist"]
    epy = y + sin(infos["cage_bas_angle"] + infos["orientation"]) * infos["cage_bas_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        BLUE,
        (x, y),
        ep,
        3,
    )

    # joueur le plus proche

    epx = x + cos(infos["nearest_copain_angle"] + infos["orientation"]) * infos["nearest_copain_dist"]
    epy = y + sin(infos["nearest_copain_angle"] + infos["orientation"]) * infos["nearest_copain_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        PINK,
        (x, y),
        ep,
        2,
    )

    epx = x + cos(infos["nearest_adv_angle"] + infos["orientation"]) * infos["nearest_adv_dist"]
    epy = y + sin(infos["nearest_adv_angle"] + infos["orientation"]) * infos["nearest_adv_dist"]

    ep = (int(epx), int(epy))

    pg.draw.line(
        screen,
        PURPLE,
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
