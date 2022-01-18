import os
import math

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


def floor_zero(x):
    return x if x >= 0 else 0


class Vertex:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z


class Camera(Vertex):
    def __init__(self, x, y, z, xr, yr, zr):
        super().__init__(x, y, z)
        self.xr = xr
        self.yr = yr
        self.zr = zr


class Line3d:
    def __init__(self, v1, v2, camera):
        self.vertexes = [v1, v2]
        self.camera = camera

    def project(self, focal, win):
        x1 = self.vertexes[0].x + self.camera.x
        x2 = self.vertexes[1].x + self.camera.x
        y1 = self.vertexes[0].y + self.camera.y
        y2 = self.vertexes[1].y + self.camera.y
        z1 = self.vertexes[0].z + self.camera.z
        z2 = self.vertexes[1].z + self.camera.z
        try:
            return (
                (focal + z1) / z1 * x1 + win.size[0] / 2,
                (focal + z1) / z1 * y1 + win.size[1] / 2,
                (focal + z2) / z2 * x2 + win.size[0] / 2,
                (focal + z2) / z2 * y2 + win.size[1] / 2
            )
        except ZeroDivisionError:
            return 0, 0, 0, 0


class Voxel:
    def __init__(self, size, pos, camera):
        self.camera = camera
        self.lines = [
            Line3d(Vertex(-size - pos.x, -size - pos.y, -size + pos.z),
                   Vertex(size - pos.x, -size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, -size - pos.y, -size + pos.z),
                   Vertex(size - pos.x, size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, size - pos.y, -size + pos.z),
                   Vertex(-size - pos.x, size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(-size - pos.x, size - pos.y, -size + pos.z),
                   Vertex(-size - pos.x, -size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(-size - pos.x, -size - pos.y, size + pos.z),
                   Vertex(size - pos.x, -size - pos.y, size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, -size - pos.y, size + pos.z),
                   Vertex(size - pos.x, size - pos.y, size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, size - pos.y, size + pos.z),
                   Vertex(-size - pos.x, size - pos.y, size + pos.z), self.camera),
            Line3d(Vertex(-size - pos.x, size - pos.y, size + pos.z),
                   Vertex(-size - pos.x, -size - pos.y, size + pos.z), self.camera),
            Line3d(Vertex(-size - pos.x, -size - pos.y, size + pos.z),
                   Vertex(-size - pos.x, -size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, -size - pos.y, size + pos.z),
                   Vertex(size - pos.x, -size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(size - pos.x, size - pos.y, size + pos.z),
                   Vertex(size - pos.x, size - pos.y, -size + pos.z), self.camera),
            Line3d(Vertex(-size - pos.x, size - pos.y, size + pos.z),
                   Vertex(-size - pos.x, size - pos.y, -size + pos.z), self.camera),
        ]


class MainWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        fov = 90
        self.camera = Vertex(0, 0, self.width / 2 / math.tan(fov / 2))  # TODO: rotation
        self.lines = Voxel(50, Vertex(0, 0, 100), self.camera).lines
        self.line_project = []
        with self.canvas:
            for line in self.lines:
                self.line_project.append(Line(points=list(map(dp, line.project(50, Window)))))

        Clock.schedule_interval(self.update, 1 / 60)

    def update(self, dt):
        self.camera.z -= 1
        i = 0
        for line in self.lines:
            self.line_project[i].points = list(map(dp, line.project(50, Window)))
            i += 1


class VoxelApp(App):
    def build(self):
        Builder.load_file("voxel.kv")


if __name__ == "__main__":
    VoxelApp().run()
