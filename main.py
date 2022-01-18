import os

os.environ['KIVY_GL_BACKEND'] = 'angle_sdl2'

from kivy.app import App
from kivy.core.window import Window
from kivy.graphics.context_instructions import Color
from kivy.graphics.vertex_instructions import Rectangle, Ellipse, Line
from kivy.metrics import dp
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import StringProperty, Clock
import keyboard
import random
from kivy.uix.widget import Widget
from kivy.lang.builder import Builder


class Vertex:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z


class Line3d:
    def __init__(self, v1, v2, camera):
        self.vertexes = [v1, v2]
        self.camera = camera

    def project(self, focal, win):
        x1 = self.vertexes[0].x - self.camera.x
        x2 = self.vertexes[1].x - self.camera.x
        y1 = self.vertexes[0].y - self.camera.y
        y2 = self.vertexes[1].y - self.camera.y
        z1 = self.vertexes[0].z - self.camera.z
        z2 = self.vertexes[1].z - self.camera.z
        return (
            focal / z1 * x1 + win.size[0] / 2,
            focal / z1 * y1 + win.size[1] / 2,
            focal / z2 * x2 + win.size[0] / 2,
            focal / z2 * y2 + win.size[1] / 2
        )


class MainWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.camera = Vertex(-500, 0, 0)
        self.lines = [
            Line3d(Vertex(-100, -100, 100), Vertex(100, -100, 100), self.camera),
            Line3d(Vertex(100, -100, 100), Vertex(100, 100, 100), self.camera),
            Line3d(Vertex(100, 100, 100), Vertex(-100, 100, 100), self.camera),
            Line3d(Vertex(-100, 100, 100), Vertex(-100, -100, 100), self.camera),

            Line3d(Vertex(-100, -100, 200), Vertex(100, -100, 200), self.camera),
            Line3d(Vertex(100, -100, 200), Vertex(100, 100, 200), self.camera),
            Line3d(Vertex(100, 100, 200), Vertex(-100, 100, 200), self.camera),
            Line3d(Vertex(-100, 100, 200), Vertex(-100, -100, 200), self.camera),

            Line3d(Vertex(-100, -100, 100), Vertex(-100, -100, 200), self.camera),
            Line3d(Vertex(100, -100, 100), Vertex(100, -100, 200), self.camera),
            Line3d(Vertex(100, 100, 100), Vertex(100, 100, 200), self.camera),
            Line3d(Vertex(-100, 100, 100), Vertex(-100, 100, 200), self.camera),
        ]
        self.line_project = []
        with self.canvas:
            for line in self.lines:
                self.line_project.append(Line(points=list(map(dp, line.project(1, Window)))))

        Clock.schedule_interval(self.update, 1 / 60)

    def update(self, dt):
        self.camera.x += 5
        if self.camera.x == 500:
            self.camera.x = -500
        i = 0
        for line in self.lines:
            self.line_project[i].points = list(map(dp, line.project(50, Window)))
            i += 1


class VoxelApp(App):
    def build(self):
        Builder.load_file("voxel.kv")


if __name__ == "__main__":
    VoxelApp().run()
