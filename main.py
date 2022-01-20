import math
import os
from kivy.app import App
from kivy.core.window import Window
from kivy.graphics.vertex_instructions import Line
from kivy.metrics import dp
from kivy.properties import Clock
import keyboard
from kivy.uix.widget import Widget
import numpy as np
from kivy.lang.builder import Builder

os.environ['KIVY_GL_BACKEND'] = 'angle_sdl2'


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
        if z1 <= 0 or z2 <= 0:
            out = (0, 0, 0, 0)
        else:
            out = (
                (focal + z1) / z1 * x1 + win.size[0] / 2,
                (focal + z1) / z1 * y1 + win.size[1] / 2,
                (focal + z2) / z2 * x2 + win.size[0] / 2,
                (focal + z2) / z2 * y2 + win.size[1] / 2
            )
        return out


class Voxel:
    def __init__(self, size, pos, camera):
        self.camera = camera
        self.size = size
        self.pos = pos
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
        self.cords_before_rotation = self.cords.copy()
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

        self.cords += np.array([[camera.x, camera.y, camera.z]] * 24)
        self.cords += np.array([[pos.x, pos.y, pos.z]] * 24)
        for i in range(len(self.cords)):
            self.cords[i] = np.matmul(rotation_y_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_x_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_z_cam, self.cords[i])
        self.cords -= np.array([[camera.x, camera.y, camera.z]] * 24)
        self.lines = []
        for i in range(int(len(self.cords) / 2)):
            self.lines.append(Line3d(Vertex(*self.cords[i * 2].tolist()),
                                     Vertex(*self.cords[i * 2 + 1].tolist()), self.camera))

    def update(self, camera):
        pos = self.pos
        self.cords = self.cords_before_rotation.copy()
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

        self.cords += np.array([[camera.x, camera.y, camera.z]] * 24)
        self.cords += np.array([[pos.x, pos.y, pos.z]] * 24)
        for i in range(len(self.cords)):
            self.cords[i] = np.matmul(rotation_y_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_x_cam, self.cords[i])
            self.cords[i] = np.matmul(rotation_z_cam, self.cords[i])
        self.cords -= np.array([[camera.x, camera.y, camera.z]] * 24)
        self.lines = []
        for i in range(int(len(self.cords) / 2)):
            self.lines.append(Line3d(Vertex(*self.cords[i * 2].tolist()),
                                     Vertex(*self.cords[i * 2 + 1].tolist()), self.camera))


class MainWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.fov = 90
        self.camera = VertexRotation(0, 0, 0, 0, 0, 0)
        self.voxels = [
            Voxel(160, VertexRotation(0, 0, 0, 0, 0, 0), self.camera)
        ]
        self.line_project = [
            Voxel(10, VertexRotation(0, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(10, 10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(20, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(30, 30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(40, 40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(50, 50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(60, 40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(70, 30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(80, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(90, 10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(100, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(30, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(40, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(50, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(60, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(70, 20, 100, 0, 0, 0), self.camera),

            Voxel(10, VertexRotation(120, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 40, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(160, 10, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(170, 0, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(130, 30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(140, 30, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(150, 20, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(120, 50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(130, 50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(140, 50, 100, 0, 0, 0), self.camera),
            Voxel(10, VertexRotation(150, 40, 100, 0, 0, 0), self.camera),

            Voxel(10, VertexRotation(120, 0, 100, 0, 0, 0), self.camera)
        ]
        with self.canvas:
            for voxel in self.voxels:
                for line in voxel.lines:
                    self.line_project.append(Line(points=list(map(dp, line.project(
                        self.width / 2 / math.tan(self.fov / 2), Window)))))

        Clock.schedule_interval(self.update, 1 / 60)

    def update(self, dt):
        if keyboard.is_pressed("w"):
            self.camera.z -= math.cos(math.radians(self.camera.xr)) * math.cos(math.radians(self.camera.yr))
            self.camera.y -= math.sin(math.radians(self.camera.xr))
            self.camera.x -= math.sin(math.radians(self.camera.yr))
        if keyboard.is_pressed("a"):
            self.camera.x += math.cos(math.radians(self.camera.zr)) * math.cos(math.radians(self.camera.yr))
            self.camera.y += math.sin(math.radians(self.camera.zr))
            self.camera.z -= math.sin(math.radians(self.camera.yr))
        if keyboard.is_pressed("s"):
            self.camera.z += math.cos(math.radians(self.camera.xr)) * math.cos(math.radians(self.camera.yr))
            self.camera.y += math.sin(math.radians(self.camera.xr))
            self.camera.x += math.sin(math.radians(self.camera.yr))
        if keyboard.is_pressed("d"):
            self.camera.x -= math.cos(math.radians(self.camera.zr)) * math.cos(math.radians(self.camera.yr))
            self.camera.y -= math.sin(math.radians(self.camera.zr))
            self.camera.z += math.sin(math.radians(self.camera.yr))
        if keyboard.is_pressed("q"):
            self.camera.y += math.cos(math.radians(self.camera.zr)) * math.cos(math.radians(self.camera.xr))
            self.camera.x += math.sin(math.radians(self.camera.zr))
            self.camera.z += math.sin(math.radians(self.camera.xr))
        if keyboard.is_pressed("e"):
            self.camera.y -= math.cos(math.radians(self.camera.zr)) * math.cos(math.radians(self.camera.xr))
            self.camera.x -= math.sin(math.radians(self.camera.zr))
            self.camera.z -= math.sin(math.radians(self.camera.xr))
        if keyboard.is_pressed("7"):
            self.camera.zr += 1
        if keyboard.is_pressed("2"):
            self.camera.xr -= 1
        if keyboard.is_pressed("3"):
            self.camera.zr -= 1
        if keyboard.is_pressed("4"):
            self.camera.yr -= 1
        if keyboard.is_pressed("8"):
            self.camera.xr += 1
        if keyboard.is_pressed("6"):
            self.camera.yr += 1
        i = 0
        for k in range(len(self.voxels)):
            self.voxels[k].update(self.camera)
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
