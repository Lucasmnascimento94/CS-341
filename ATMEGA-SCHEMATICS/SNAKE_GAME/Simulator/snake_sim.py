import pygame
import random

# Grid setup
GRID_SIZE = 65
CELL = 10
WIDTH = GRID_SIZE * CELL
HEIGHT = GRID_SIZE * CELL

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, 30)
big_font = pygame.font.SysFont(None, 72)

# Colors
GREEN = (0, 255, 0)
RED = (255, 0, 0)
BLACK = (0, 0, 0)
GRAY = (40, 40, 40)
WHITE = (255, 255, 255)
OVERLAY = (0, 0, 0, 150)

def draw_cell(i, j, color):
    pygame.draw.rect(screen, color, (j*CELL, i*CELL, CELL, CELL))

def draw_grid():
    for i in range(GRID_SIZE):
        pygame.draw.line(screen, GRAY, (0, i*CELL), (WIDTH, i*CELL))
        pygame.draw.line(screen, GRAY, (i*CELL, 0), (i*CELL, HEIGHT))

def draw_score(score):
    text = font.render("Score: " + str(score), True, WHITE)
    screen.blit(text, (5, 5))

def spawn_foods(count, exclude):
    foods = set()
    while len(foods) < count:
        pos = (random.randint(0, GRID_SIZE-1), random.randint(0, GRID_SIZE-1))
        if pos not in exclude and pos not in foods:
            foods.add(pos)
    return list(foods)

def reset_game():
    snake = [(32, 32)]
    direction = (0, 1)
    score = 0
    foods = spawn_foods(3, set(snake))
    return snake, direction, score, foods

def show_defeat(score):
    overlay = pygame.Surface((WIDTH, HEIGHT), pygame.SRCALPHA)
    overlay.fill((0,0,0,160))
    screen.blit(overlay, (0,0))
    text = big_font.render("DEFEAT", True, WHITE)
    sub = font.render(f"Score: {score}   Press R to retry or ESC to quit", True, WHITE)
    tx = (WIDTH - text.get_width()) // 2
    ty = (HEIGHT - text.get_height()) // 2 - 20
    sx = (WIDTH - sub.get_width()) // 2
    sy = ty + text.get_height() + 10
    screen.blit(text, (tx, ty))
    screen.blit(sub, (sx, sy))
    pygame.display.flip()

def snake_game():
    snake, direction, score, foods = reset_game()
    running = True
    defeated = False

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.KEYDOWN:
                if not defeated:
                    if event.key == pygame.K_UP and direction != (1, 0):
                        direction = (-1, 0)
                    elif event.key == pygame.K_DOWN and direction != (-1, 0):
                        direction = (1, 0)
                    elif event.key == pygame.K_LEFT and direction != (0, 1):
                        direction = (0, -1)
                    elif event.key == pygame.K_RIGHT and direction != (0, -1):
                        direction = (0, 1)
                # global controls
                if event.key == pygame.K_ESCAPE:
                    running = False
                if event.key == pygame.K_r and defeated:
                    snake, direction, score, foods = reset_game()
                    defeated = False

        if not defeated:
            head = snake[0]
            new_head = (head[0] + direction[0], head[1] + direction[1])

            # collision detection with walls or self
            if (new_head[0] < 0 or new_head[0] >= GRID_SIZE or
                new_head[1] < 0 or new_head[1] >= GRID_SIZE or
                new_head in snake):
                defeated = True
            else:
                snake.insert(0, new_head)

                # check food collisions
                if new_head in foods:
                    score += 10
                    foods.remove(new_head)
                    # spawn one new food not on snake or existing foods
                    exclude = set(snake) | set(foods)
                    new = None
                    while new is None:
                        cand = (random.randint(0, GRID_SIZE-1), random.randint(0, GRID_SIZE-1))
                        if cand not in exclude:
                            new = cand
                    foods.append(new)
                else:
                    snake.pop()

        # draw
        screen.fill(BLACK)
        draw_grid()
        for f in foods:
            draw_cell(f[0], f[1], RED)
        for s in snake:
            draw_cell(s[0], s[1], GREEN)
        draw_score(score)

        if defeated:
            show_defeat(score)
            # skip clock tick to keep steady; still loop to handle retry/quit
            continue

        pygame.display.flip()
        clock.tick(10)

snake_game()
pygame.quit()
