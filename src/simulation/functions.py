import pygame as pg
from math import cos, sin
from config import *


def draw_player(t, x, y, d):
    pg.draw.circle(
        screen,
        RED if t == 1 else BLUE,
        (x, SCREEN_HEIGHT - y),
        PLAYER_SIZE,
    )

    pg.draw.line(
        screen,
        WHITE,
        (x, SCREEN_HEIGHT - y),
        (
            x + cos(d) * PLAYER_SIZE,
            SCREEN_HEIGHT - y - sin(d) * PLAYER_SIZE,
        ),
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
        pg.Rect(
            SCREEN_WIDTH // 2 - 2.5, 0, 5, SCREEN_HEIGHT
        ),
    )

    pg.draw.circle(
        screen,
        WHITE,
        (SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2),
        10,
    )
