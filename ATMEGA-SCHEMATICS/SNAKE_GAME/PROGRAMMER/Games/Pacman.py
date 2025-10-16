import pygame
import random

pygame.init()

# The Grid is 65x65 cells
GRID_WIDTH = 65
GRID_HEIGHT = 65
# Each Grid Cell is 10 pixels x 10 pixels
GRID_SIZE = 10
# Each Grid Cell is 1 pixels x 1 pixels
# GRID_SIZE = 1
# The Screen is the size of the Grid
SCREEN_WIDTH = GRID_WIDTH * GRID_SIZE
SCREEN_HEIGHT = GRID_HEIGHT * GRID_SIZE

# Set the Clock to help track time and control frame rate
# Times in pygame are in milliseconds so 1000ms = 1s
clock = pygame.time.Clock()

# Set the FPS
FPS = 60  

# Set the font
font = pygame.font.Font('freesansbold.ttf', 20)

# Create the Screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Pacman")

# Center in x-axis and y-axis
grid_x = GRID_WIDTH // 2
grid_y = GRID_HEIGHT // 2

# Movement speed
counter = 0
delay = 30 # Move every 30 frames,

# Convert grid position to pixel position
def grid_to_pixel(grid_x, grid_y):
    pixel_x = grid_x * GRID_SIZE
    pixel_y = grid_y * GRID_SIZE
    return pixel_x, pixel_y

# Draw the Grid for the game
def draw_grid(screen, GRID_SIZE):
    # Draw grid lines every GRID_SIZE pixels
    for x in range(0, SCREEN_WIDTH, GRID_SIZE):
        # Draw the horizontal grid lines
        pygame.draw.line(screen, ('white'), (x, 0), (x, 650))
    for y in range(0, SCREEN_HEIGHT, GRID_SIZE):
        # Draw the vertical grid lines
        pygame.draw.line(screen, ('white'), (0, y), (650, y))
    pass
        
# Move Pacman randomly within grid (up, down, left, right)
def move_pacman_randomly():
    # Changing global variables
    global grid_x, grid_y, counter
    
    # Only move when counter reaches delay
    counter += 1
    if counter >= delay:
        counter = 0  # Reset counter
        
        # Choose one of four directions: up, down, left, right
        direction = random.choice(['up', 'down', 'left', 'right'])
        
        # Apply movement based on random direction (one grid cell at a time)
        # Computer understands vertical vector differently than humans do
        # up -> down, down -> up
        if direction == 'up':
            new_grid_y = grid_y - 1
            if new_grid_y >= 0:  # Check top boundary
                grid_y = new_grid_y
        elif direction == 'down':
            new_grid_y = grid_y + 1
            if new_grid_y < GRID_HEIGHT:  # Check bottom boundary
                grid_y = new_grid_y
        elif direction == 'left':
            new_grid_x = grid_x - 1
            if new_grid_x >= 0:  # Check left boundary
                grid_x = new_grid_x
        elif direction == 'right':
            new_grid_x = grid_x + 1
            if new_grid_x < GRID_WIDTH:  # Check right boundary
                grid_x = new_grid_x

#2D array of pixels (5x6)
# 1 = Yellow, 0 = Black
pacman_shape = [
    [0, 1, 1, 1, 0, 0],  # Row 0
    [1, 1, 1, 1, 1, 0],  # Row 1
    [1, 1, 1, 1, 1, 0],  # Row 2
    [1, 1, 1, 1, 1, 0],  # Row 3
    [0, 1, 1, 1, 0, 0],  # Row 4
]

# Draw the Pacman for the game
# def draw_pacman(screen):
    # pixel_x, pixel_y = grid_to_pixel(grid_x, grid_y)
    # center_pixel_x = pixel_x + GRID_SIZE // 2
    # center_pixel_y = pixel_y + GRID_SIZE // 2
    # pygame.draw.circle(screen, 'yellow', (center_pixel_x, center_pixel_y), 20)

def draw_pacman_px(screen):
    px_x, px_y = grid_to_pixel(grid_x, grid_y)
    # px_x = 320px, px_y = 320px
    
    # Loop through each (i,j) or (x,y) in pacman_shape
    for row in range(5):      # 5 rows
        for col in range(6):  # 6 columns
            if pacman_shape[row][col] == 1:
                # Calculate pixel position for this cell
                # Example:
                # Row 0, Col 0: (320, 320) = (px_x, px_y)
                # Row 0, Col 1: (330, 320) = (px_x + 10, px_y) = (320 + 10, 320)
                cell_pixel_x = px_x + (col * GRID_SIZE)
                cell_pixel_y = px_y + (row * GRID_SIZE)
                
                # Fill a rectangle for this grid cell
                pygame.draw.rect(screen, 'yellow', 
                               (cell_pixel_x, cell_pixel_y, GRID_SIZE, GRID_SIZE))

#Game Loop
run = True
while run:
    # Called once per frame to compute how much time has passed since the last frame
    clock.tick(FPS)

    # Draw the Screen
    screen.fill('black')

    # Exit the while loop if the user quits the game
    for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

    # Move Pacman randomly
    move_pacman_randomly()
    
    # Draw the game elements
    draw_grid(screen, GRID_SIZE)
    # draw_pacman(screen)
    draw_pacman_px(screen)

    # Update the full display Surface to the screen
    # Update the contents of the entire display
    # Swap the front and back buffers 
    pygame.display.flip()
pygame.quit()