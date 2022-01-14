from kivy.app import App
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle, Ellipse
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty, Clock
import os
import keyboard
import random

# let's add scoring
from kivy.uix.widget import Widget

os.environ['KIVY_GL_BACKEND'] = 'angle_sdl2'


class MainWidget(Widget):
    player1_score = StringProperty("0")
    player2_score = StringProperty("0")

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.ball_speed = [5, 5]
        self.robo_mode = [False, False]  # robo_mode[2] = is up
        with self.canvas:
            self.ball = Ellipse(pos=(300, 100), size=(20, 20))
            self.player1 = Rectangle(pos=(700, 300), size=(20, 150))
            self.player2 = Rectangle(pos=(100, 100), size=(20, 150))
        Clock.schedule_interval(self.update, 1 / 60)

    def update(self, dt):
        x, y = self.ball.pos
        x += self.ball_speed[0]
        y += self.ball_speed[1]
        if y > self.height - self.ball.size[1]:
            self.ball_speed[1] = -abs(self.ball_speed[1])
        if y < 0:
            self.ball_speed[1] = abs(self.ball_speed[1])

        if keyboard.is_pressed("r"):
            self.robo_mode = [True, False]
        if keyboard.is_pressed("up arrow"):
            self.player1.pos = self.player1.pos[0], self.player1.pos[1] + 5
        if keyboard.is_pressed("down arrow"):
            self.player1.pos = self.player1.pos[0], self.player1.pos[1] - 5
        if keyboard.is_pressed("w") and not self.robo_mode[0]:
            self.player2.pos = self.player2.pos[0], self.player2.pos[1] + 5
        if keyboard.is_pressed("s") and not self.robo_mode[0]:
            self.player2.pos = self.player2.pos[0], self.player2.pos[1] - 5

        if self.robo_mode and self.player2.pos[1] < 0:
            self.robo_mode[1] = True
        elif self.robo_mode and self.player2.pos[1] + self.player2.size[1] > self.height:
            self.robo_mode[1] = False

        if self.robo_mode[0] and self.robo_mode[1]:
            self.player2.pos = self.player2.pos[0], self.player2.pos[1] + 5
        if self.robo_mode[0] and not self.robo_mode[1]:
            self.player2.pos = self.player2.pos[0], self.player2.pos[1] - 5

        # more or '>' is == to more to right | up
        # less or '<' is == to more to left | down

        if ((self.player1.pos[0] < x < self.player1.pos[0] + self.player1.size[0] or
             self.player1.pos[0] < x + self.ball.size[0] < self.player1.pos[0] + self.player1.size[0]) and
                (self.player1.size[1] + self.player1.pos[1] > y > self.player1.pos[1] or
                 self.player1.size[1] + self.player1.pos[1] > y + self.ball.size[1] > self.player1.pos[1])):
            self.ball_speed[0] = (random.random() / 10 + .9) * -abs(self.ball_speed[0])

        if ((self.player2.pos[0] < x < self.player2.pos[0] + self.player2.size[0] or
             self.player2.pos[0] < x + self.ball.size[0] < self.player2.pos[0] + self.player2.size[0]) and
                (self.player2.size[1] + self.player2.pos[1] > y > self.player2.pos[1] or
                 self.player2.size[1] + self.player2.pos[1] > y + self.ball.size[1] > self.player2.pos[1])):
            self.ball_speed[0] = (random.random() / 10 + .9) * abs(self.ball_speed[0])

        if x + self.ball.size[0] > self.width:
            self.player2_score = str(int(self.player2_score) + 1)
            x = 300
            self.ball_speed = [random.uniform(4, 6), random.uniform(4, 6)]
        if x < 0:
            self.player1_score = str(int(self.player1_score) + 1)
            x = 300
            self.ball_speed = [random.uniform(4, 6), random.uniform(4, 6)]

        self.ball.pos = (x, y)


class Menu(BoxLayout):
    pass


class GravApp(App):
    pass


if __name__ == "__main__":
    GravApp().run()
