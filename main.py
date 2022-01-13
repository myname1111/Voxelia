from kivy.app import App
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty
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
        with self.canvas:
            Color(0, .9, 0)
            self.rec = Rectangle(pos=(300, 300), size=(300, 400))

    def move_rec_right(self):
        x, y = self.rec.pos
        x += 1
        self.rec.pos = (x, y)




class GravApp(App):
    pass


if __name__ == "__main__":
    GravApp().run()
