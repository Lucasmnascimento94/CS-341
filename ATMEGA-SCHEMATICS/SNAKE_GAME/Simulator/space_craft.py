import pygame
import sys

pygame.init()

# --- Grid setup ---
GRID_SIZE = 65
CELL_SIZE = 10
WIDTH, HEIGHT = GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Spacecraft Demo Animated Enemies")

clock = pygame.time.Clock()

# --- Fonts & Scores ---
font = pygame.font.SysFont("Arial", 20)
victory_font = pygame.font.SysFont("Arial", 50, bold=True)
score = 0
high_score = 0  # dynamic

# --- Ship setup ---
ship_pos = [GRID_SIZE // 2, GRID_SIZE - 5]
projectiles = []

shoot_cooldown = 3
shoot_timer = 0

# --- Explosions ---
explosions = []

class Explosion:
    def __init__(self, pos, color):
        self.pos = pos[:]
        self.color = color
        self.timer = 8
        self.radius = 2

    def update(self):
        self.timer -= 1
        self.radius += 1

    def draw(self, surface):
        x_pixel, y_pixel = grid_to_pixel(self.pos[0], self.pos[1])
        pygame.draw.circle(surface, self.color,
                           (x_pixel + CELL_SIZE // 2, y_pixel + CELL_SIZE // 2),
                           self.radius, 1)

    def is_finished(self):
        return self.timer <= 0

# --- Helper functions ---
def grid_to_pixel(i, j):
    return i * CELL_SIZE, j * CELL_SIZE

def draw_cell(surface, i, j, color):
    x, y = grid_to_pixel(i, j)
    rect = pygame.Rect(x, y, CELL_SIZE, CELL_SIZE)
    pygame.draw.rect(surface, color, rect)

def draw_grid(surface):
    for x in range(0, WIDTH, CELL_SIZE):
        pygame.draw.line(surface, (30, 30, 30), (x, 0), (x, HEIGHT))
    for y in range(0, HEIGHT, CELL_SIZE):
        pygame.draw.line(surface, (30, 30, 30), (0, y), (WIDTH, y))

# --- Ship ---
ship_shape = [(0, 0), (-1, 1), (1, 1)]

def draw_ship(surface, pos):
    for dx, dy in ship_shape:
        x, y = pos[0] + dx, pos[1] + dy
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            draw_cell(surface, x, y, (255, 255, 255))

# --- Projectile ---
def draw_projectile(surface, pos):
    draw_cell(surface, pos[0], pos[1], (255, 255, 255))

# --- Enemy class ---
class Enemy:
    def __init__(self, pos, color):
        self.pos = pos
        self.color = color
        self.frames = [
            [(0, 0), (-1, 0), (1, 0), (0, 1)],
            [(0, 0), (-1, 0), (1, 0)]
        ]
        self.current_frame = 0
        self.timer = 0
        self.hit_timer = 0
        self.alive = True

    def update(self):
        self.timer += 1
        if self.timer % 10 == 0:
            self.current_frame = 1 - self.current_frame
        if self.hit_timer > 0:
            self.hit_timer -= 1

    def draw(self, surface):
        color = self.color
        if self.hit_timer > 0:
            color = (255, 255, 255)
        for dx, dy in self.frames[self.current_frame]:
            x, y = self.pos[0] + dx, self.pos[1] + dy
            if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
                draw_cell(surface, x, y, color)

    def hit(self):
        self.hit_timer = 3
        self.alive = False

# --- Create enemies ---
enemies = []
rows = [
    {"color": (255, 255, 0), "y": 5},
    {"color": (255, 105, 180), "y": 10},
    {"color": (0, 191, 255), "y": 15},
]
cols = 10
start_x = 10
spacing_x = 5

for row in rows:
    for col in range(cols):
        x = start_x + col * spacing_x
        y = row["y"]
        enemies.append(Enemy([x, y], row["color"]))

# --- Auto movement pattern for enemies ---
auto_timer = 0
move_delay = 4
pattern = [
    ("left", 3), ("right", 5),
    ("left", 2), ("pause", 1),
    ("right", 4), ("up", 1),
    ("left", 1), ("down", 1),
]
pattern_index = 0
steps_done = 0

# --- Main loop ---
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # --- Auto-move enemies ---
    auto_timer += 1
    if auto_timer >= move_delay:
        auto_timer = 0
        direction, count = pattern[pattern_index]
        steps_done += 1

        dx, dy = 0, 0
        if direction == "left": dx = -1
        elif direction == "right": dx = 1
        elif direction == "up": dy = -1
        elif direction == "down": dy = 1

        # Prevent enemies from leaving screen
        can_move = True
        for enemy in enemies:
            new_x = enemy.pos[0] + dx
            new_y = enemy.pos[1] + dy
            if not (0 <= new_x < GRID_SIZE) or not (0 <= new_y < GRID_SIZE):
                can_move = False
                break
        if can_move:
            for enemy in enemies:
                enemy.pos[0] += dx
                enemy.pos[1] += dy

        if steps_done >= count:
            steps_done = 0
            pattern_index = (pattern_index + 1) % len(pattern)

    # --- Auto ship movement towards nearest enemy ---
    if enemies:
        target = min(enemies, key=lambda e: abs(e.pos[0] - ship_pos[0]) + abs(e.pos[1] - ship_pos[1]))
        if target.pos[0] < ship_pos[0] and ship_pos[0] > 2:
            ship_pos[0] -= 1
        elif target.pos[0] > ship_pos[0] and ship_pos[0] < GRID_SIZE - 3:
            ship_pos[0] += 1

    # --- Shooting ---
    shoot_timer += 1
    if shoot_timer >= shoot_cooldown:
        shoot_timer = 0
        proj_pos = [ship_pos[0], ship_pos[1] - 1]
        if proj_pos[1] >= 0:
            projectiles.append(proj_pos)

    # --- Move projectiles & check collisions ---
    for proj in projectiles[:]:
        proj[1] -= 1
        for enemy in enemies:
            if enemy.alive and proj[0] == enemy.pos[0] and proj[1] == enemy.pos[1]:
                enemy.hit()
                explosions.append(Explosion(enemy.pos, enemy.color))
                score += 10
                if proj in projectiles:
                    projectiles.remove(proj)

    # Remove dead enemies
    enemies = [e for e in enemies if e.alive or e.hit_timer > 0]
    projectiles = [p for p in projectiles if p[1] >= 0]

    # --- Update enemies and explosions ---
    for enemy in enemies:
        enemy.update()
    for exp in explosions[:]:
        exp.update()
        if exp.is_finished():
            explosions.remove(exp)

    # --- Draw everything ---
    screen.fill((0, 0, 10))
    draw_grid(screen)
    draw_ship(screen, ship_pos)
    for proj in projectiles:
        draw_projectile(screen, proj)
    for enemy in enemies:
        enemy.draw(screen)
    for exp in explosions:
        exp.draw(screen)

    # --- Update high score ---
    high_score = max(high_score, score)

    # --- Draw scores ---
    score_text = font.render(f"SCORE  {score}", True, (255, 255, 255))
    high_text = font.render(f"HIGH SCORE  {high_score}", True, (255, 255, 255))
    screen.blit(score_text, (20, 10))
    screen.blit(high_text, (WIDTH - 250, 10))

    # --- Victory message ---
    if not enemies:
        victory_text = victory_font.render("VICTORY!", True, (0, 255, 0))
        screen.blit(victory_text, (WIDTH // 2 - 120, HEIGHT // 2 - 30))
        score_text_victory = font.render(f"SCORE: {score}", True, (255, 255, 255))
        screen.blit(score_text_victory, (WIDTH // 2 - 60, HEIGHT // 2 + 30))

    pygame.display.flip()
    clock.tick(15)

pygame.quit()
sys.exit()
