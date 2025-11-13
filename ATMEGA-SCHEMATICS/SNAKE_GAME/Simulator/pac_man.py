import pygame
import random
import math

pygame.init()

# Grid and screen setup
GRID_WIDTH = 65
GRID_HEIGHT = 65
GRID_SIZE = 10
SCREEN_WIDTH = GRID_WIDTH * GRID_SIZE
SCREEN_HEIGHT = GRID_HEIGHT * GRID_SIZE

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Pac-Man Simulator")

# Colors
BLACK = (0, 0, 0)
BLUE = (0, 0, 200)
YELLOW = (255, 255, 0)
WHITE = (255, 255, 255)
GRAY = (50, 50, 50)

# Font setup
font = pygame.font.SysFont("arial", 28, bold=True)

# Directions
UP = (0, -1)
DOWN = (0, 1)
LEFT = (-1, 0)
RIGHT = (1, 0)

# Create the Pac-Man maze pattern
def create_maze():
    maze = [[0 for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

    # Outer border (2 cells thick)
    for i in range(GRID_HEIGHT):
        for j in range(GRID_WIDTH):
            if i < 2 or i >= GRID_HEIGHT - 2 or j < 2 or j >= GRID_WIDTH - 2:
                maze[i][j] = 1

    # Clear tunnel openings on left and right
    for i in range(GRID_HEIGHT // 2 - 1, GRID_HEIGHT // 2 + 1):
        maze[i][0] = 0
        maze[i][1] = 0
        maze[i][GRID_WIDTH - 1] = 0
        maze[i][GRID_WIDTH - 2] = 0

    # Internal maze pattern (simplified Pac-Man layout)
    for i in range(5, GRID_HEIGHT - 5, 6):
        for j in range(4, GRID_WIDTH - 4):
            if j % 8 != 0:
                maze[i][j] = 1
    for j in range(5, GRID_WIDTH - 5, 6):
        for i in range(4, GRID_HEIGHT - 4):
            if i % 8 != 0:
                maze[i][j] = 1

    return maze

# Draw the maze walls
def draw_maze(maze):
    for i in range(GRID_HEIGHT):
        for j in range(GRID_WIDTH):
            rect = pygame.Rect(j * GRID_SIZE, i * GRID_SIZE, GRID_SIZE, GRID_SIZE)
            if maze[i][j] == 1:
                pygame.draw.rect(screen, BLUE, rect)
            # Draw grid lines
            pygame.draw.rect(screen, GRAY, rect, 1)

# Function to draw Pac-Man (circle with mouth)
def draw_pacman(x, y):
    center = (x * GRID_SIZE + GRID_SIZE // 2, y * GRID_SIZE + GRID_SIZE // 2)
    radius = GRID_SIZE // 2
    mouth_angle = math.pi / 6
    pygame.draw.circle(screen, YELLOW, center, radius)
    mouth_points = [
        center,
        (center[0] + radius * math.cos(mouth_angle), center[1] - radius * math.sin(mouth_angle)),
        (center[0] + radius * math.cos(-mouth_angle), center[1] - radius * math.sin(-mouth_angle)),
    ]
    pygame.draw.polygon(screen, BLACK, mouth_points)

# Function to create food
def create_food(maze):
    food = []
    for i in range(2, GRID_HEIGHT - 2):
        for j in range(2, GRID_WIDTH - 2):
            if maze[i][j] == 0 and random.random() < 0.03:
                food.append((i, j))
    return food

# Choose a safe spawn point (not trapped)
def choose_safe_spawn(maze):
    open_cells = []
    for i in range(2, GRID_HEIGHT - 2):
        for j in range(2, GRID_WIDTH - 2):
            if maze[i][j] == 0:
                # Check if there is at least 1 free cell in each direction
                neighbors = [(i-1,j),(i+1,j),(i,j-1),(i,j+1)]
                if all(0 <= ni < GRID_HEIGHT and 0 <= nj < GRID_WIDTH and maze[ni][nj] == 0 for ni,nj in neighbors):
                    open_cells.append((i,j))
    return random.choice(open_cells) if open_cells else (GRID_HEIGHT//2, GRID_WIDTH//2)

# Main function
def main():
    clock = pygame.time.Clock()
    maze = create_maze()

    pacman_y, pacman_x = choose_safe_spawn(maze)
    direction = None
    score = 0
    food = create_food(maze)

    running = True
    while running:
        screen.fill(BLACK)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP:
                    direction = UP
                elif event.key == pygame.K_DOWN:
                    direction = DOWN
                elif event.key == pygame.K_LEFT:
                    direction = LEFT
                elif event.key == pygame.K_RIGHT:
                    direction = RIGHT

        # Move Pac-Man
        if direction:
            new_x = pacman_x + direction[0]
            new_y = pacman_y + direction[1]

            # Wrap horizontally through tunnel
            if new_x < 0:
                new_x = GRID_WIDTH - 1
            elif new_x >= GRID_WIDTH:
                new_x = 0

            # Check wall collision
            if maze[new_y][new_x] == 0:
                pacman_x, pacman_y = new_x, new_y

        # Eat food
        if (pacman_y, pacman_x) in food:
            food.remove((pacman_y, pacman_x))
            score += 10

        # Draw elements
        draw_maze(maze)
        for f in food:
            pygame.draw.circle(screen, WHITE, (f[1]*GRID_SIZE + GRID_SIZE//2, f[0]*GRID_SIZE + GRID_SIZE//2), 2)
        draw_pacman(pacman_x, pacman_y)

        # Draw score
        score_text = font.render(f"SCORE: {score}", True, YELLOW)
        text_rect = score_text.get_rect(center=(SCREEN_WIDTH // 2, 20))
        screen.blit(score_text, text_rect)

        pygame.display.flip()
        clock.tick(10)

    pygame.quit()

if __name__ == "__main__":
    main()
