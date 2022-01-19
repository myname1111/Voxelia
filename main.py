import os
import math
import numpy as np

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


class VertexRotation(Vertex):
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
            out = (
                (focal + z1) / z1 * x1 + win.size[0] / 2,
                (focal + z1) / z1 * y1 + win.size[1] / 2,
                (focal + z2) / z2 * x2 + win.size[0] / 2,
                (focal + z2) / z2 * y2 + win.size[1] / 2
            )
            if z1 <= 0 or z2 <= 0:
                out = (0, 0, 0, 0)
            return out
        except ZeroDivisionError:
            return 0, 0, 0, 0


class Voxel:
    def __init__(self, size, pos, camera):
        self.camera = camera
        size /= 2
        self.cords = np.array([
            [-size, -size, -size],
            [size, -size, -size],
            [size, -size, -size],
            [size, size, -size],
            [size, size, -size],
            [-size, size, -size],
            [-size, size, -size],
            [-size, -size, -size],
            [-size, -size, size],
            [size, -size, size],
            [size, -size, size],
            [size, size, size],
            [size, size, size],
            [-size, size, size],
            [-size, size, size],
            [-size, -size, size],
            [-size, -size, size],
            [-size, -size, -size],
            [size, -size, size],
            [size, -size, -size],
            [size, size, size],
            [size, size, -size],
            [-size, size, size],
            [-size, size, -size]
        ])
        rotation_x_pos = np.array([[1, 0, 0],
                                   [0, math.cos(math.radians(pos.xr)), -math.sin(math.radians(pos.xr))],
                                   [0, math.sin(math.radians(pos.xr)), math.cos(math.radians(pos.xr))]])

        rotation_y_pos = np.array([[math.cos(math.radians(pos.yr)), 0, -math.sin(math.radians(pos.yr))],
                                   [0, 1, 0],
                                   [math.sin(math.radians(pos.yr)), 0, math.cos(math.radians(pos.yr))]])

        rotation_z_pos = np.array([[math.cos(math.radians(pos.zr)), -math.sin(math.radians(pos.zr)), 0],
                                   [math.sin(math.radians(pos.zr)), math.cos(math.radians(pos.zr)), 0],
                                   [0, 0, 1]])

        rotation_x_cam = np.array([[1, 0, 0],
                                   [0, math.cos(math.radians(camera.xr)), -math.sin(math.radians(camera.xr))],
                                   [0, math.sin(math.radians(camera.xr)), math.cos(math.radians(camera.xr))]])

        rotation_y_cam = np.array([[math.cos(math.radians(camera.yr)), 0, -math.sin(math.radians(camera.yr))],
                                   [0, 1, 0],
                                   [math.sin(math.radians(camera.yr)), 0, math.cos(math.radians(camera.yr))]])

        rotation_z_cam = np.array([[math.cos(math.radians(camera.zr)), -math.sin(math.radians(camera.zr)), 0],
                                   [math.sin(math.radians(camera.zr)), math.cos(math.radians(camera.zr)), 0],
                                   [0, 0, 1]])

        for i in range(len(self.cords)):
            self.cords[i] = np.matmul(rotation_y_pos, self.cords[i])
            self.cords[i] = np.matmul(rotation_x_pos, self.cords[i])
            self.cords[i] = np.matmul(rotation_z_pos, self.cords[i])

        for i in range(len(self.cords)):
            self.cords[i] = np.matmul(rotation_y_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_x_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_z_cam, self.cords[i])
        self.cords += np.array([[pos.x, pos.y, pos.z]] * 24)
        self.lines = []
        for i in range(int(len(self.cords) / 2)):
            self.lines.append(Line3d(VertexRotation(*self.cords[i * 2].tolist(), pos.xr, pos.yr, pos.zr),
                                     VertexRotation(*self.cords[i * 2 + 1].tolist(),
                                                    pos.xr, pos.yr, pos.zr), self.camera))


class MainWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.fov = 90
        self.camera = VertexRotation(0, 0, 0, 0, 0, 0)
        self.voxels = [
            Voxel(10, VertexRotation(0, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-10, -10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-20, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-30, -30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-40, -40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-50, -50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-60, -40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-70, -30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-80, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-90, -10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-100, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-30, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-40, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-50, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-60, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-70, -20, 100, 0, 0, 0), self.camera),
        ]
        self.line_project = []
        with self.canvas:
            for voxel in self.voxels:
                for line in voxel.lines:
                    self.line_project.append(Line(points=list(map(dp, line.project(
                        self.width / 2 / math.tan(self.fov / 2), Window)))))

        Clock.schedule_interval(self.update, 1 / 60)

    def update(self, dt):
        if keyboard.is_pressed("w"):
            self.camera.z -= 1
        if keyboard.is_pressed("a"):
            self.camera.x += 1
        if keyboard.is_pressed("s"):
            self.camera.z += 1
        if keyboard.is_pressed("d"):
            self.camera.x -= 1
        if keyboard.is_pressed("q"):
            self.camera.y += 1
        if keyboard.is_pressed("e"):
            self.camera.y -= 1
        if keyboard.is_pressed("1"):
            self.camera.zr -= 1
        if keyboard.is_pressed("2"):
            self.camera.xr += 1
        if keyboard.is_pressed("3"):
            self.camera.zr += 1
        if keyboard.is_pressed("4"):
            self.camera.xr -= 1
        if keyboard.is_pressed("5"):
            self.camera.yr += 1
        if keyboard.is_pressed("6"):
            self.camera.yr -= 1
        i = 0
        self.voxels = [
            Voxel(10, VertexRotation(0, 0, 100, 45, 0, 0), self.camera),
            Voxel(10, VertexRotation(-10, -10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-20, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-30, -30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-40, -40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-50, -50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-60, -40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-70, -30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-80, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-90, -10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-100, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-30, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-40, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-50, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-60, -20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(-70, -20, 100, 0, 0, 0), self.camera),
        ]
        for voxel in self.voxels:
            for line in voxel.lines:
                self.line_project[i].points = list(map(dp, line.project(
                    self.width / 2 / math.tan(self.fov / 2), Window)))
                i += 1


class VoxelApp(App):
    def build(self):
        Builder.load_file("voxel.kv")


if __name__ == "__main__":
    VoxelApp().run()
