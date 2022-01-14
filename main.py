from kivy.app import App
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle, Ellipse
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty, Clock
from kivy.uix.widget import Widget
import os
import keyboard
os.environ['KIVY_GL_BACKEND'] = 'angle_sdl2'


class Menu(GridLayout):
    num = StringProperty("0")

    def button_pressed(self):
        print("Hello world!")
        self.num = str(int(self.num) + 1)

    def on_slider_val(self, widget):
        self.num = str(int(widget.value))


class MainWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.rebound = ["Right", "Up"]
        with self.canvas:
            self.ball = Ellipse(pos=(200, 100), size=(100, 100))
            self.player1 = Rectangle(pos=(700, 100), size=(20, 150))
            self.player2 = Rectangle(pos=(100, 100), size=(20, 150))
            self.ball_speed = [5, 5]
        Clock.schedule_interval(self.update, 1/60)

    def update(self, dt):
        x, y = self.ball.pos
        print(keyboard.is_pressed('up arrow'))
        x += self.ball_speed[0]
        y += self.ball_speed[1]
        if y > self.height - self.ball.size[1]:
            self.ball_speed[1] = -abs(self.ball_speed[1])
        if y < 0:
            self.ball_speed[1] = abs(self.ball_speed[1])
        self.ball.pos = (x, y)


class GravApp(App):
    pass


if __name__ == "__main__":
    GravApp().run()
