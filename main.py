from kivy.app import App
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle, Ellipse
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty, Clock
from kivy.uix.widget import Widget


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
            Color(0, .9, 0)
            self.ball = Ellipse(pos=(200, 100), size=(100, 100))
        Clock.schedule_interval(self.update, 1/60)

    def update(self, dt):
        x, y = self.ball.pos
        speed = (2, 2)
        if x >= self.width - self.ball.size[0]:
            self.rebound[0] = "Left"
        elif x <= 0:
            self.rebound[0] = "Right"
        if y >= self.height - self.ball.size[1]:
            self.rebound[1] = "Down"
        elif y <= 0:
            self.rebound[1] = "Up"
        if self.rebound[0] == "Right":
            x += speed[0]
        elif self.rebound[0] == "Left":
            x -= speed[0]
        if self.rebound[1] == "Up":
            y += speed[1]
        elif self.rebound[1] == "Down":
            y -= speed[1]
        self.ball.pos = (x, y)


class GravApp(App):
    pass


if __name__ == "__main__":
    GravApp().run()
