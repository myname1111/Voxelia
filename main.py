from kivy.app import App
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle, Ellipse
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty, Clock
import os
import keyboard
import random
from kivy.uix.widget import Widget
from kivy.lang.builder import Builder
os.environ['KIVY_GL_BACKEND'] = 'angle_sdl2'


class MainWidget(Widget):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        with self.canvas:
            pass

class VoxelApp(App):
    def build(self):
        Builder.load_file("voxel.kv")


if __name__ == "__main__":
    VoxelApp().run()
