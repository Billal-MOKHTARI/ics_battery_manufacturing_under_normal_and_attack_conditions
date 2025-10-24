import pygame
from pymodbus.client import ModbusTcpClient
import sys
import os
import threading

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from utils import io
from services.connection import client, physical_addr

class Label:
    def __init__(self, text, x, y, font_size=40, color=(1, 97, 86), align="left"):
        """
        Creates a label for displaying text in Pygame.

        :param text: The text to display
        :param x: X coordinate of the label
        :param y: Y coordinate of the label
        :param font_size: Font size of the text
        :param color: Text color (default: white)
        :param align: Text alignment ("left", "center", "right")
        """
        self.text = text
        self.x = x
        self.y = y
        self.color = color
        self.align = align
        self.font = pygame.font.Font(None, font_size)
        self.rendered_text = self.font.render(self.text, True, self.color)
        self.rect = self.rendered_text.get_rect()

        # Position alignment
        if self.align == "center":
            self.rect.center = (x, y)
        elif self.align == "right":
            self.rect.topright = (x, y)
        else:  # Default is left alignment
            self.rect.topleft = (x, y)

    def draw(self, screen):
        """Draws the label on the given screen."""
        screen.blit(self.rendered_text, self.rect)

    def set_text(self, new_text):
        """Updates the label text dynamically."""
        self.text = new_text
        self.rendered_text = self.font.render(self.text, True, self.color)

    def set_color(self, new_color):
        """Updates the text color."""
        self.color = new_color
        self.rendered_text = self.font.render(self.text, True, self.color)

class Button:
    def __init__(self, x, y, width, height, text, color, hover_color, font, x_translation = 0):
        self.rect = pygame.Rect(x, y, width, height)
        self.color = color
        self.hover_color = hover_color
        self.text = font.render(text, True, (255, 255, 255))
        x, y = self.rect.center
        self.text_rect = self.text.get_rect(center=(x-self.text.get_size()[0]+x_translation, y))
    

    def draw(self, screen):
        current_color = self.hover_color if self.rect.collidepoint(pygame.mouse.get_pos()) else self.color
        pygame.draw.rect(screen, current_color, self.rect, border_radius=10)
        screen.blit(self.text, self.text_rect)

    def is_clicked(self, event):
        return event.type == pygame.MOUSEBUTTONDOWN and self.rect.collidepoint(event.pos)

class IntegerInputField:
    def __init__(self, x, y, width, height, label, font_size=30):
        self.rect = pygame.Rect(x, y, width, height)
        self.label = label  # Unique identifier for input fields
        self.font = pygame.font.Font(None, font_size)
        self.text = ""
        self.is_active = False

        # Colors
        self.text_color = (255, 255, 255)  # White
        self.bg_color = (50, 50, 50)       # Dark gray
        self.border_color = (255, 255, 255)  # White (inactive)
        self.active_border_color = (0, 255, 0)  # Green (active)

        # Label
        self.label_text = self.font.render(label, True, (0, 0, 0))
        self.label_rect = self.label_text.get_rect(midbottom=(self.rect.x-100, self.rect.top + 40))

    def draw(self, screen):
        """Draw the input field on the screen."""
        screen.blit(self.label_text, self.label_rect)  # Draw label above input field
        pygame.draw.rect(screen, self.bg_color, self.rect, border_radius=5)
        border_color = self.active_border_color if self.is_active else self.border_color
        pygame.draw.rect(screen, border_color, self.rect, 2, border_radius=5)

        text_surface = self.font.render(self.text, True, self.text_color)
        text_rect = text_surface.get_rect(midleft=(self.rect.x + 10, self.rect.centery))
        screen.blit(text_surface, text_rect)

    def handle_event(self, event):
        """Handle user input."""
        if event.type == pygame.MOUSEBUTTONDOWN:
            # Activate if clicked inside, deactivate otherwise
            self.is_active = self.rect.collidepoint(event.pos)

        if event.type == pygame.KEYDOWN and self.is_active:
            if event.key == pygame.K_RETURN:
                print(f"Entered integer in {self.label}: {self.text}")
                self.is_active = False  # Deactivate after pressing enter
                return True
            elif event.key == pygame.K_BACKSPACE:
                self.text = self.text[:-1]
                return False
            elif event.unicode.isdigit():  # Allow only numbers
                self.text += event.unicode
                return False
        return False

class MixerSimulation:
    def __init__(self):
        # Load configurations
        self.configs = io.read_json("configs/vars.json")
        self.connection = self.configs["connection"]
        self.physical_addr = physical_addr
        
        # Set up ModbusTCP connection
        self.client = client
        self.connection_status = self.client.connect()
        
        # Load component configurations
        self.sensors = self.configs["sensors"]
        self.actuators = self.configs["actuators"]
        
        # Initialize pygame
        pygame.init()
        
        # Set up colors
        self.colors = {
            "normal": {
                "off": (148, 1, 18),
                "on": (6, 73, 153)
            },
            "hover": {
                "off": (208, 2, 27),
                "on": (74, 121, 226)
            },
            "text": {
                "on": "Disable",
                "off": "Enable"
            }
        }
        
        # Set up display
        info = pygame.display.Info()
        self.screen_width, self.screen_height = info.current_w, info.current_h
        self.screen = pygame.display.set_mode(
            (self.screen_width, self.screen_height), 
            pygame.FULLSCREEN
        )
        
        # Load assets
        self.load_assets()
        
        # Set up coordinates for all components
        self.setup_coordinates()
        
        # Create UI elements
        self.create_ui_elements()
        
        # Initialize state tracking
        self.states = {key: False for key in self.physical_addr.keys() if key != "timer_gauge_1" and 
                       key != "timer_gauge_2" and key != "PV" and key != "time_1" and 
                       key != "time_2" and key != "CV"}
        # self.states["auto_run"] = False
        # Update initial states from PLC
        self.update_states_from_plc()
        
        # Setup animation properties
        self.setup_animation_properties()
        
        # Initialize component positions
        self.initialize_component_positions()
        
        # Set up clock for frame rate control
        self.clock = pygame.time.Clock()
        self.running = True

    def run_script(self, script):
        exec(open(script).read())

    def load_assets(self):
        """Load and resize all image assets"""
        # Scale factors
        self.ratio = 0.65
        self.ratio_M1 = 0.3
        self.ratio_M2 = 0.8
        
        # Load main machine image
        self.image_mixing = self.load_and_scale_image(
            "assets/mixing machine.png", 
            self.ratio * 1324, 
            self.ratio * 1798
        )
        
        # Load indicator images
        self.indicator_images = {
            "red": {},
            "green": {}
        }
        
        # Load sensor indicators (red and green circles)
        for sensor in ["a", "b", "d1", "e1", "d2", "e2", "f", "g", "l"]:
            circle_width = self.ratio * pygame.image.load("assets/red_circle.png").get_width()
            circle_height = self.ratio * pygame.image.load("assets/red_circle.png").get_height()
            
            self.indicator_images["red"][sensor] = self.load_and_scale_image(
                "assets/red_circle.png", 
                circle_width, 
                circle_height
            )
            self.indicator_images["green"][sensor] = self.load_and_scale_image(
                "assets/green_circle.png", 
                circle_width, 
                circle_height
            )
        
        # Load cylinder images
        self.cylinder_images = {}
        verin_r_width = self.ratio * pygame.image.load("assets/verin_r.png").get_width()
        verin_r_height = self.ratio * pygame.image.load("assets/verin_r.png").get_height()
        verin_l_width = self.ratio * pygame.image.load("assets/verin_l.png").get_width()
        verin_l_height = self.ratio * pygame.image.load("assets/verin_l.png").get_height()
        verin_b_width = self.ratio * pygame.image.load("assets/verin_b.png").get_width()
        verin_b_height = self.ratio * pygame.image.load("assets/verin_b.png").get_height()
        
        self.cylinder_images["EA"] = self.load_and_scale_image("assets/verin_r.png", verin_r_width, verin_r_height)
        self.cylinder_images["EB"] = self.load_and_scale_image("assets/verin_r.png", verin_r_width, verin_r_height)
        self.cylinder_images["ED"] = self.load_and_scale_image("assets/verin_l.png", verin_l_width, verin_l_height)
        self.cylinder_images["EE"] = self.load_and_scale_image("assets/verin_l.png", verin_l_width, verin_l_height)
        self.cylinder_images["EF"] = self.load_and_scale_image("assets/verin_b.png", verin_b_width, verin_b_height)
        self.cylinder_images["EG"] = self.load_and_scale_image("assets/verin_r.png", verin_r_width, verin_r_height)
        
        # Load engine images
        self.engine_images = {
            "M1": {
                "off": self.load_and_scale_image(
                    "assets/engine_off.png", 
                    self.ratio_M1 * pygame.image.load("assets/engine_off.png").get_width(),
                    self.ratio_M1 * pygame.image.load("assets/engine_off.png").get_height()
                ),
                "on": self.load_and_scale_image(
                    "assets/engine_on.png", 
                    self.ratio_M1 * pygame.image.load("assets/engine_on.png").get_width(),
                    self.ratio_M1 * pygame.image.load("assets/engine_on.png").get_height()
                )
            },
            "M2": {
                "off": self.load_and_scale_image(
                    "assets/3D_mixer_off.png", 
                    self.ratio_M2 * pygame.image.load("assets/3D_mixer_off.png").get_width(),
                    self.ratio_M2 * pygame.image.load("assets/3D_mixer_off.png").get_height()
                ),
                "on": self.load_and_scale_image(
                    "assets/3D_mixer_on.png", 
                    self.ratio_M2 * pygame.image.load("assets/3D_mixer_on.png").get_width(),
                    self.ratio_M2 * pygame.image.load("assets/3D_mixer_on.png").get_height()
                )
            }
        }

    def load_and_scale_image(self, path, width, height):
        """Helper method to load and scale an image"""
        image = pygame.image.load(path)
        return pygame.transform.scale(image, (width, height))

    def setup_coordinates(self):
        """Set up normalized coordinates for all components"""
        self.coordinates = {
            "m": [0.8, 0.2],
            "a": [0.452, 0.448],
            "b": [0.357, 0.123],
            "d1": [0.416, 0.54],  
            "e1": [0.3215, 0.21],  
            "d2": [0.537, 0.586],  
            "e2": [0.447, 0.2627],  
            "f": [0.2343, 0.88],   
            "g": [0.223, 0.6263],   
            "l": [0.163, 0.611],
            "EA": [0.506, 0.451],
            "EB": [0.415, 0.124],
            "ED": [0.465, 0.592],
            "EE": [0.372, 0.266],
            "EF": [0.238, 0.97],
            "EG": [0.349, 0.618],
            "M1": [0.21, 0.915],
            "M2": [0.3, 0.535],
            "timer_gauge_1": [0.8, 0.25],
            "timer_gauge_2": [0.8, 0.3],
            "PV": [0.8, 0.35],
            "time_1": [0.22, 0.8],
            "time_2": [0.29, 0.45],
            "CV": [0.08, 0.5],
            # "auto_run": [0.8, 0.4]
        }
        
        # Store original coordinates for animation
        self.original_coordinates = {}
        for key, value in self.coordinates.items():
            self.original_coordinates[key] = value.copy()
        
        # Define translations for animations
        self.translations = {
            "EA": (0.028, 0),
            "EB": (0.028, 0),
            "ED": (-0.046, 0),
            "EE": (-0.046, 0),
            "EF": (0, 0.075),
            "EG": (-0.032, 0)
        }

    def create_ui_elements(self):
        """Create all UI elements"""
        font = pygame.font.Font(None, 36)
        
        # Create button, input fields, and labels
        self.start_button = Button(
            self.screen_width * self.coordinates["m"][0], 
            self.screen_height * self.coordinates["m"][1], 
            150, 50, "ON", (0, 200, 0), (0, 255, 0), font
        )

        # self.auto_run = Button(
        #     self.screen_width * self.coordinates["auto_run"][0], 
        #     self.screen_height * self.coordinates["auto_run"][1], 
        #     150, 50, "Auto Run", self.colors["normal"]["off"], self.colors["hover"]["off"], font, x_translation=105
        # )
        
        self.timer_1 = IntegerInputField(
            self.screen_width * self.coordinates["timer_gauge_1"][0], 
            self.screen_height * self.coordinates["timer_gauge_1"][1], 
            150, 50, "Timer (Inline Mixer)"
        )
        
        self.timer_2 = IntegerInputField(
            self.screen_width * self.coordinates["timer_gauge_2"][0], 
            self.screen_height * self.coordinates["timer_gauge_2"][1], 
            150, 50, "    Timer (3D Mixer)"
        )
        
        self.PV = IntegerInputField(
            self.screen_width * self.coordinates["PV"][0], 
            self.screen_height * self.coordinates["PV"][1], 
            150, 50, "        Preset Value"
        )
        
        self.time_1_label = Label(
            "Time 1", 
            self.screen_width * self.coordinates["time_1"][0], 
            self.screen_height * self.coordinates["time_1"][1]
        )
        
        self.time_2_label = Label(
            "Time 2", 
            self.screen_width * self.coordinates["time_2"][0], 
            self.screen_height * self.coordinates["time_2"][1]
        )
        
        self.CV_label = Label(
            "Counting Value", 
            self.screen_width * self.coordinates["CV"][0], 
            self.screen_height * self.coordinates["CV"][1], 
            color=(208, 67, 2)
        )
        
        # Initialize input fields with values from PLC
        self.timer_1.text = str(self.client.read_holding_registers(
            address=self.physical_addr["timer_gauge_1"], count=1).registers[0])
        self.timer_2.text = str(self.client.read_holding_registers(
            address=self.physical_addr["timer_gauge_2"], count=1).registers[0])
        self.PV.text = str(self.client.read_holding_registers(
            address=self.physical_addr["PV"], count=1).registers[0])

    def setup_animation_properties(self):
        """Set up properties for animations"""
        self.to_true = {actuator: False for actuator in self.actuators}
        self.animation_duration = 30  # Number of frames for the animation to complete
        self.animation_speed = 1  # Frames per update

    def initialize_component_positions(self):
        """Initialize all component positions and rects"""
        # Create image rect for the main machine
        self.image_rect = self.image_mixing.get_rect(center=(self.screen_width * 0.35, self.screen_height // 2))
        
        # Create dictionaries to store rects for components
        self.circle_rects = {}
        self.cylinders = {}
        self.engines = {}
        
        # Initialize sensor indicators
        for sensor in ["a", "b", "d1", "e1", "d2", "e2", "f", "g", "l"]:
            self.circle_rects[sensor] = self.indicator_images["red"][sensor].get_rect(
                center=(
                    self.screen_width * self.coordinates[sensor][0], 
                    self.screen_height * self.coordinates[sensor][1]
                )
            )
        
        # Initialize cylinders
        for cylinder in ["EA", "EB", "ED", "EE", "EF", "EG"]:
            self.cylinders[cylinder] = self.cylinder_images[cylinder].get_rect(
                center=(
                    self.screen_width * self.coordinates[cylinder][0], 
                    self.screen_height * self.coordinates[cylinder][1]
                )
            )
        
        # Initialize engines
        for engine in ["M1", "M2"]:
            self.engines[engine] = self.engine_images[engine]["off"].get_rect(
                center=(
                    self.screen_width * self.coordinates[engine][0], 
                    self.screen_height * self.coordinates[engine][1]
                )
            )

    def update_states_from_plc(self):
        """Update all component states from PLC"""
        for item in self.states:
            # if item not in ["auto_run"]:
            self.states[item] = self.client.read_coils(
                address=self.physical_addr[item], count=1).bits[0]

    def update_labels_from_plc(self):
        """Update all label values from PLC"""
        self.time_1_label.set_text(
            str(self.client.read_holding_registers(
                address=self.physical_addr["time_1"], count=1).registers[0]) + " s"
        )
        self.time_2_label.set_text(
            str(self.client.read_holding_registers(
                address=self.physical_addr["time_2"], count=1).registers[0]) + " s"
        )
        self.CV_label.set_text(
            str(self.client.read_holding_registers(
                address=self.physical_addr["CV"], count=1).registers[0]) + " times"
        )

    def handle_events(self):
        """Handle all pygame events"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                self.running = False  # Press ESC to exit fullscreen mode
            elif event.type == pygame.MOUSEBUTTONDOWN:
                self.handle_mouse_click(event)
            
            # Handle input field events
            if self.timer_1.handle_event(event):
                self.client.write_register(
                    self.physical_addr["timer_gauge_1"], 
                    int(self.timer_1.text)
                )
            if self.timer_2.handle_event(event):
                self.client.write_register(
                    self.physical_addr["timer_gauge_2"], 
                    int(self.timer_2.text)
                )
            if self.PV.handle_event(event):
                self.client.write_register(
                    self.physical_addr["PV"], 
                    int(self.PV.text)
                )

    def handle_mouse_click(self, event):
        """Handle mouse click events"""
        mouse_pos = pygame.mouse.get_pos()
        
        # Check if any sensor indicators were clicked
        for key, rect in self.circle_rects.items():
            if rect.collidepoint(mouse_pos):
                self.states[key] = not self.states[key]
                self.client.write_coil(self.physical_addr[key], self.states[key])
                print(f"Clicked {key}, new state: {self.states[key]}")
        
        # Check if start button was clicked
        if self.start_button.is_clicked(event):
            self.states["m"] = not self.states["m"]
            self.client.write_coil(self.physical_addr["m"], self.states["m"])
            print(f"Clicked m, new state: {self.states['m']}")
        
        # if self.auto_run.is_clicked(event):
            
        #     if not self.states["auto_run"]:
        #         auto_run_thread = threading.Thread(target=self.run_script("src/services/auto_start.py"))
        #         auto_run_thread.start()
        #         self.states["auto_run"] = True

        #     else:
        #         try:
        #             auto_run_thread.daemon = True
        #             self.states["auto_run"] = False
        #             del auto_run_thread
        #         except:
        #             print("The thread is not created yet.")
                
            

    def update_animations(self):
        """Update animations for all actuators"""
        for item in self.actuators:
            if self.states[item] and not self.to_true[item]:
                self.coordinates[item][0] += self.translations[item][0]
                self.coordinates[item][1] += self.translations[item][1]
                self.to_true[item] = True
            
            if not self.states[item] and self.to_true[item]:
                self.coordinates[item][0] -= self.translations[item][0]
                self.coordinates[item][1] -= self.translations[item][1]
                self.to_true[item] = False

    def update_component_positions(self):
        """Update positions of all components based on current coordinates"""
        # Update sensor indicators
        for key in self.circle_rects:
            self.circle_rects[key] = self.circle_rects[key].copy()
            self.circle_rects[key].center = (
                self.screen_width * self.coordinates[key][0], 
                self.screen_height * self.coordinates[key][1]
            )
        
        # Update cylinders
        for key in self.cylinders:
            self.cylinders[key] = self.cylinders[key].copy()
            self.cylinders[key].center = (
                self.screen_width * self.coordinates[key][0], 
                self.screen_height * self.coordinates[key][1]
            )
        
        # Update engines
        for key in self.engines:
            self.engines[key] = self.engines[key].copy()
            self.engines[key].center = (
                self.screen_width * self.coordinates[key][0], 
                self.screen_height * self.coordinates[key][1]
            )

    def draw(self):
        """Draw all components to the screen"""
        # Fill the screen with white
        self.screen.fill((255, 255, 255))
        
        # Draw the main machine image
        self.screen.blit(self.image_mixing, self.image_rect)
        
        # Draw start button
        if self.states["m"]:
            self.start_button.color = self.colors["normal"]["on"]
            self.start_button.hover_color = self.colors["hover"]["on"]
            self.start_button.text = pygame.font.Font(None, 36).render(
                self.colors["text"]["on"], True, (255, 255, 255)
            )
        else:
            self.start_button.color = self.colors["normal"]["off"]
            self.start_button.hover_color = self.colors["hover"]["off"]
            self.start_button.text = pygame.font.Font(None, 36).render(
                self.colors["text"]["off"], True, (255, 255, 255)
            )
        

        self.start_button.draw(self.screen)
        # self.auto_run.draw(self.screen)

        # Draw sensor indicators
        for sensor in ["a", "b", "d1", "e1", "d2", "e2", "f", "g", "l"]:
            if self.states[sensor]:
                self.screen.blit(
                    self.indicator_images["green"][sensor], 
                    self.circle_rects[sensor]
                )
            else:
                self.screen.blit(
                    self.indicator_images["red"][sensor], 
                    self.circle_rects[sensor]
                )
        
        # Draw cylinders
        for cylinder in ["EA", "EB", "ED", "EE", "EF", "EG"]:
            self.screen.blit(
                self.cylinder_images[cylinder], 
                self.cylinders[cylinder]
            )
        
        # Draw engines
        for engine in ["M1", "M2"]:
            if self.states[engine]:
                self.screen.blit(
                    self.engine_images[engine]["on"], 
                    self.engines[engine]
                )
            else:
                self.screen.blit(
                    self.engine_images[engine]["off"], 
                    self.engines[engine]
                )
        
        # Draw UI elements
        self.timer_1.draw(self.screen)
        self.timer_2.draw(self.screen)
        self.PV.draw(self.screen)
        self.time_1_label.draw(self.screen)
        self.time_2_label.draw(self.screen)
        self.CV_label.draw(self.screen)
        
        # Update the display
        pygame.display.flip()

    def run(self):
        """Main game loop"""
        while self.running:
            # Control frame rate
            dt = self.clock.tick(60) / 1000  # Get delta time in seconds (60 FPS target)
            
            # Handle events
            self.handle_events()
            
            # Update states from PLC
            self.update_states_from_plc()
            
            # Update label values from PLC
            self.update_labels_from_plc()
            
            # Update animations
            self.update_animations()
            
            # Update component positions
            self.update_component_positions()
            
            # Draw everything
            self.draw()
        
        # Clean up pygame
        pygame.quit()

# Main entry point
if __name__ == "__main__":
    simulation = MixerSimulation()
    simulation.run()