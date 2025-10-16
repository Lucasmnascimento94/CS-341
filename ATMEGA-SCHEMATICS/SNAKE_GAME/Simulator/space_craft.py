import pygame
import sys

pygame.init()

GRID_SIZE = 65
CELL_SIZE = 10  # each grid cell is 10x10 pixels

WIDTH, HEIGHT = GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Spacecraft Simulator with Alien Enemies")

clock = pygame.time.Clock()

ship_pos = [GRID_SIZE // 2, GRID_SIZE // 2]  # start near center

projectiles = []

shoot_cooldown = 4
shoot_timer = 0

def grid_to_pixel(i, j):
    return i * CELL_SIZE, j * CELL_SIZE

def draw_cell(surface, i, j, color):
    x, y = grid_to_pixel(i, j)
    rect = pygame.Rect(x, y, CELL_SIZE, CELL_SIZE)
    pygame.draw.rect(surface, color, rect)

def draw_grid(surface):
    for x in range(0, WIDTH, CELL_SIZE):
        pygame.draw.line(surface, (40, 40, 40), (x, 0), (x, HEIGHT))
    for y in range(0, HEIGHT, CELL_SIZE):
        pygame.draw.line(surface, (40, 40, 40), (0, y), (WIDTH, y))

# Ship shape: 3 cells like a triangle
ship_shape = [
    (0, 0),    # center
    (-1, 1),   # bottom left
    (1, 1),    # bottom right
]

def draw_ship(surface, pos):
    i, j = pos
    for dx, dy in ship_shape:
        x, y = i + dx, j + dy
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            draw_cell(surface, x, y, (255, 255, 255))  # white ship

def draw_projectile(surface, pos):
    i, j = pos
    draw_cell(surface, i, j, (255, 255, 255))  # white projectile

# Enemy class: compact alien shape (4 cells)
class Enemy:
    def __init__(self, pos):
        self.pos = pos
        self.shape = [
            (0, 0),     # center
            (-1, 0),    # left
            (1, 0),     # right
            (0, 1),     # block below center
        ]

    def draw(self, surface):
        i, j = self.pos
        for dx, dy in self.shape:
            x, y = i + dx, j + dy
            if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
                draw_cell(surface, x, y, (0, 255, 0))  # green alien

# Create enemies grid: 3 rows x 10 columns
enemies = []
rows = 3
cols = 10
start_x = 10
start_y = 5
spacing_x = 5
spacing_y = 4

for row in range(rows):
    for col in range(cols):
        x = start_x + col * spacing_x
        y = start_y + row * spacing_y
        enemies.append(Enemy([x, y]))

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT] and ship_pos[0] > 1:
        ship_pos[0] -= 1
    if keys[pygame.K_RIGHT] and ship_pos[0] < GRID_SIZE - 2:
        ship_pos[0] += 1
    if keys[pygame.K_UP] and ship_pos[1] > 0:
        ship_pos[1] -= 1
    if keys[pygame.K_DOWN] and ship_pos[1] < GRID_SIZE - 2:
        ship_pos[1] += 1

    shoot_timer += 1
    if shoot_timer >= shoot_cooldown:
        shoot_timer = 0
        proj_pos = [ship_pos[0], ship_pos[1] - 1]
        if proj_pos[1] >= 0:
            projectiles.append(proj_pos)

    # Move projectiles upward
    for proj in projectiles:
        proj[1] -= 1

    # Remove off-screen projectiles
    projectiles = [p for p in projectiles if p[1] >= 0]

    screen.fill((0, 0, 0))
    draw_grid(screen)

    draw_ship(screen, ship_pos)

    for proj in projectiles:
        draw_projectile(screen, proj)

    for enemy in enemies:
        enemy.draw(screen)

    pygame.display.flip()
    clock.tick(10)

pygame.quit()
sys.exit()
