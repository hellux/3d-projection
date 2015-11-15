from math import cos, sin, pi
from time import time

from lib.matrix import matrix_multiply, inverse
from lib.vector import vector_add, vector_subtract, dot_product,\
                       cross_product, inner_product

class Camera:
    """class for storing camera properties and calculating projection"""

    def __init__(self, pos=None, rot=None, res=None,
                 focal_length=None, fov=None):
        self.pos = []           #position of camera (x, y, z) [m]
        self.rot = []           #rotation of camera (alpha, beta) [rad]
        self.res = []           #resolution of output image (width, height) [px]
        self.focal_length = 0.0 #focal length [m]
        self.fov = 0.0          #field of view [rad]
        self.array_size = []    #relative pixel array dimensions (w, h) [m]

        if pos == None:
            self.pos = [0, 0, 0]
        else:
            self.pos = pos
        if rot == None:
            self.rot = [0, 0]
        else:
            self.rot = rot
        if res == None:
            self.res = (640, 480)
        else:
            self.res = res
        if focal_length == None:
            self.focal_length = 0.001
        else:
            self.focal_length = focal_length
        if fov == None:
            self.fov = 1.5
        else:
            self.fov = fov
        self._calc_array_dimensions()
        
        #progress_bar
        self.time_start = 0
        self.time_eta = 0

    def _calc_array_dimensions(self):
        """calculate the relative width and height for the pixel array"""
        width = self.focal_length * sin(self.fov/2)
        height = width * (self.res[1]/self.res[0])
        self.array_size = (width, height)

    def _camera_vector(self, col, row):
        """Parse the vector specifying the direction of a ray for a pixel at
        (row, col) in the pixel array in front of the camera."""
        
        #vector when camera points in the positive z-direction
        pixel_size = self.array_size[0] / self.res[0] #width = height

        #x center of pixels in column col
        x = -self.array_size[0]/2 + (col+0.5)*pixel_size
        #y center of pixels in row row
        y = -self.array_size[1]/2 + (row+0.5)*pixel_size
        #z coordinate for all pixels
        z = self.focal_length
        vector = Camera.rotate((x, y ,z), x=self.rot[0], y=self.rot[1])
        return vector

    def rotate(vertex, point=None, x=None, y=None, z=None):
        """rotates a vertex around a point around any or all axes"""
        alpha, beta, gamma = x, y, z
        if point == None:
            point = (0, 0, 0)
        #translate vertex to revolve around origo:
        rotated_vertex = vector_subtract(vertex, point)
        #convert vertex into a 1-row matrix: (x, y, z) --> ((x, y, z))
        rotated_vertex = (rotated_vertex,)

        if alpha != None:
            rotation_matrix_x = ((1,            0,          0),
                                 (0,            cos(alpha),  sin(alpha)),
                                 (0,            -sin(alpha), cos(alpha)))
            rotated_vertex = matrix_multiply(rotated_vertex, rotation_matrix_x)
        if beta != None:
            rotation_matrix_y = ((cos(beta),    0,          -sin(beta)),
                                 (0,            1,          0),
                                 (sin(beta),    0,          cos(beta)))
            rotated_vertex = matrix_multiply(rotated_vertex, rotation_matrix_y)
        if gamma != None:
            rotation_matrix_z = ((cos(gamma),   sin(gamma), 0),
                                 (-sin(gamma),  cos(gamma), 0),
                                 (0,            0,          1))
            rotated_vertex = matrix_multiply(rotated_vertex, rotation_matrix_z)

        #return vertex translated to revolve around point:
        return  vector_add(rotated_vertex[0], point)

    def distance_to_triangle(self, solid, triangle, V):
        S = self.pos
        #vertices for triangle T
        T = triangle['vertices']
        #normal for triangle's plane
        N = triangle['normal']
        
        try:
            t = -(dot_product(N, S)-dot_product(N, T[0])) / dot_product(N, V)
        except ZeroDivisionError:
            print('Triangle\'s plane is parallel to ray')
            return None
        #point where ray hits triangle's plane:
        P = vector_add(S, inner_product(t, V))

        R = vector_subtract(P, T[0])
        Q1 = vector_subtract(T[1], T[0])
        Q2 = vector_subtract(T[2], T[0])

        M1 = ((dot_product(Q1, Q1), dot_product(Q1, Q2)),
              (dot_product(Q1, Q2), dot_product(Q2, Q2)))
        M2 = ((dot_product(R, Q1),),
              (dot_product(R, Q2),))

        w = matrix_multiply(inverse(M1), M2)
        w = (1-w[0][0]-w[1][0], w[0][0], w[1][0])

        if all([i > 0 for i in w]) and t > 0:
            #point P is inside triangle / ray  goes through triangle
            #triangle is in front camera if t > 0
            #calculate distance similiar to Pythagoras:
            return t
        else:
            return None

    def calculate_pixel_color(self, col, row, world):
        distance = None
        V = self._camera_vector(col, row) #P(t) = S + tV
        for key, solid in world.items():
            for triangle in solid.triangles:
                d = self.distance_to_triangle(solid, triangle, V)
                if d != None:
                    if distance == None:
                        distance = d
                    elif d < distance:
                        distance = d
        if distance == None:
            color = (0, 0, 0)
        else:
            c = int((distance-4000) / 4000 * (255))
            color = (c, c, c)
        return color

    def calculate_pixel_map(self, world):
        """creates a pixel map by checking the first triangle a every ray
        through a pixel hits. For ever triangle for every pixel it checks at
        which point the ray hits the plane which the triangle lies on. 
        Then it checks if that point is inside the triangle. If that triangle
        is the closest one that the ray hits it affects the color of the pixel.

        P(t) = S + tV | equation for ray going through pixel at S + V
        N = (T1 - T0) x (T2 - T0 | expression for a normal for the plane 
        which the triangle with vertices (T0, T1, T2) lies on.

        """
        pixel_map = []
        for row in range(self.res[1]):
            pixel_row = []
            for col in range(self.res[0]):
                self.print_progress_bar((row*self.res[0]+col)/self.res[0]/self.res[1])
                color = self.calculate_pixel_color(col, row, world)
                pixel_row.append(color)
            pixel_map.append(pixel_row)

        print('\nPixel map finished.')
        return pixel_map

    def print_progress_bar(self, progress):
        if progress == 0:
            self.time_start = time()
            print('Rendering...', end='')
        else:
            self.time_elapsed = time() - self.time_start
            self.time_eta = self.time_elapsed / progress - self.time_elapsed

        string = 'Rendering... {}% {}s'.format(round(progress*100, 1),
                                               int(self.time_eta))
        progress_bar_total = 77-len(string)
        progress_bar_current = int(progress*progress_bar_total)
        progress_bar_remaining = progress_bar_total - progress_bar_current

        print('\r{} [{}{}]'.format(string,
                                   progress_bar_current*'#',
                                   progress_bar_remaining*'-'),
              end='')

def test():
    cam1 = Camera()
    assert cam1.res == (640, 480)

    cam2 = Camera(focal_length=0.1)
    assert cam2.focal_length == 0.1
    
    print(Camera.rotate((1, 0, 0), point=(1, 1, 1), y=pi/2, z=pi/2))

    print('All tests ran perfectly, no errors!')

if __name__ == '__main__':
    test()
