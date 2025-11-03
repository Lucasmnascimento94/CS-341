import pygame
import random

pygame.init()

# Grid setup
GRID_WIDTH = 65
GRID_HEIGHT = 65
GRID_SIZE = 10
SCREEN_WIDTH = GRID_WIDTH * GRID_SIZE
SCREEN_HEIGHT = GRID_HEIGHT * GRID_SIZE
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Pacman Simulator")

clock = pygame.time.Clock()
FPS = 60

font = pygame.font.Font('freesansbold.ttf', 20)

# Pacman movement delay
delay = 15  

# Convert grid position to pixel position
def grid_to_pixel(grid_x, grid_y):
    return grid_x * GRID_SIZE, grid_y * GRID_SIZE

# Draw grid
def draw_grid(screen, GRID_SIZE):
    for x in range(0, SCREEN_WIDTH, GRID_SIZE):
        pygame.draw.line(screen, 'gray20', (x, 0), (x, SCREEN_HEIGHT))
    for y in range(0, SCREEN_HEIGHT, GRID_SIZE):
        pygame.draw.line(screen, 'gray20', (0, y), (SCREEN_WIDTH, y))

# Pacman shape (5x6)
pacman_shape = [
    [0, 1, 1, 1, 0, 0],
    [1, 1, 1, 1, 1, 0],
    [1, 1, 1, 1, 1, 0],
    [1, 1, 1, 1, 1, 0],
    [0, 1, 1, 1, 0, 0],
]

# Maze layout (1 = wall)
maze = [[0 for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]
for x in range(GRID_WIDTH):
    maze[0][x] = 1
    maze[GRID_HEIGHT - 1][x] = 1
for y in range(GRID_HEIGHT):
    maze[y][0] = 1
    maze[y][GRID_WIDTH - 1] = 1

# Add some random internal walls
for i in range(300):
    wx = random.randint(1, GRID_WIDTH - 2)
    wy = random.randint(1, GRID_HEIGHT - 2)
    maze[wy][wx] = 1

# Generate food positions
food_positions = []
for _ in range(200):
    fx, fy = random.randint(1, GRID_WIDTH - 2), random.randint(1, GRID_HEIGHT - 2)
    if maze[fy][fx] == 0:
        food_positions.append((fx, fy))

# Pacman characters
pacmen = []
for _ in range(3):  # 3 Pacman characters
    gx, gy = random.randint(1, GRID_WIDTH - 2), random.randint(1, GRID_HEIGHT - 2)
    pacmen.append({'x': gx, 'y': gy, 'counter': 0})

# Move pacmen randomly, avoiding walls
def move_pacman_randomly(pac):
    pac['counter'] += 1
    if pac['counter'] >= delay:
        pac['counter'] = 0
        direction = random.choice(['up', 'down', 'left', 'right'])
        new_x, new_y = pac['x'], pac['y']
        if direction == 'up':
            new_y -= 1
        elif direction == 'down':
            new_y += 1
        elif direction == 'left':
            new_x -= 1
        elif direction == 'right':
            new_x += 1
        if 0 <= new_x < GRID_WIDTH and 0 <= new_y < GRID_HEIGHT and maze[new_y][new_x] == 0:
            pac['x'], pac['y'] = new_x, new_y

# Draw pacman (pixel art)
def draw_pacman_px(screen, grid_x, grid_y):
    px_x, px_y = grid_to_pixel(grid_x, grid_y)
    for row in range(5):
        for col in range(6):
            if pacman_shape[row][col] == 1:
                cell_x = px_x + (col * GRID_SIZE // 2)
                cell_y = px_y + (row * GRID_SIZE // 2)
                pygame.draw.rect(screen, 'yellow', (cell_x, cell_y, GRID_SIZE // 2, GRID_SIZE // 2))

# Draw maze walls
def draw_maze(screen):
    for y in range(GRID_HEIGHT):
        for x in range(GRID_WIDTH):
            if maze[y][x] == 1:
                px, py = grid_to_pixel(x, y)
                pygame.draw.rect(screen, 'blue', (px, py, GRID_SIZE, GRID_SIZE))

# Draw food
def draw_food(screen):
    for fx, fy in food_positions:
        px, py = grid_to_pixel(fx, fy)
        pygame.draw.circle(screen, 'white', (px + GRID_SIZE // 2, py + GRID_SIZE // 2), 2)

# Game loop
run = True
while run:
    clock.tick(FPS)
    screen.fill('black')

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False

    # Move and draw pacmen
    for pac in pacmen:
        move_pacman_randomly(pac)

    draw_maze(screen)
    draw_grid(screen, GRID_SIZE)
    draw_food(screen)
    for pac in pacmen:
        draw_pacman_px(screen, pac['x'], pac['y'])

    pygame.display.flip()

pygame.quit()
