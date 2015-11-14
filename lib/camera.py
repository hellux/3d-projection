class Camera:
    """class for storing camera properties and calculating projection"""
    def __init__(self, pos=None, res=None, focal_length=None, fov=None):
        self.pos = []           #position of camera (x, y, z) [m]
        self.res = []           #resolution of output image (width, height) [px]
        self.focal_length = 0   #focal length [m]
        self.fov = 0            #field of view [rad]
        self.array_size = []    #relative pixel array dimensions (w, h) [m]

        if pos == None:
            self.pos = [0, 0, 0]
        else:
            self.pos = pos
        if res == None:
            self.res = (640, 480)
        else:
            self.res = res
        if focal_length == None:
            self.focal_length == 0.01
        else:
            self.focal_length = focal_length
        if fov == None:
            self.fov = 1.5
        else:
            self.fov = fov

        _calc_array_dimensions()

    def _calc_array_dimensions():
        """calculate the relative width and height for the pixel array"""
        width = self.focal_length * sin(self.fov/2)
        height = self.array_width * (self.pos[1]/self.pos[0])

        self.array_size = (width, height)

    def parse_array_vertex(x, y):
        """Parse the vertex located in the pixel array in front of the camera
        for specific pixel at (x, y)"""
        

def test():
    cam1 = Camera()
    assert cam1.res == (640, 480)

    cam2 = Camera(focal_length=0.1)
    assert cam2.focal_length == 0.1
    
    print('All tests ran perfectly, no errors!')

if __name__ == '__main__':
    test()
