from PIL import Image
from datetime import datetime

from lib.solid import Solid
from lib.camera import Camera

from math import pi

def main():
    item = Solid('../data/cube_tri.obj')
    world = {}
    world['item'] = item
    camera = Camera(pos=(-0.3, -0.3, -5),
                    rot=(-pi/50, pi/50),
                    res=(800, 600),
                    fov=0.9*pi)
    render(world, camera)

def render(world, camera, filepath=None):
    if filepath == None:
        filepath = 'out/{}_render.png'.format(datetime.now().isoformat('T'))
    image = Image.new('RGB', camera.res)
    pixel_map = camera.calculate_pixel_map(world)
    print('Setting up image...')
    for x in range(image.size[0]):
        for y in range(image.size[1]):
                image.putpixel((x, y), pixel_map[y][x])

    image.save(filepath)
    image.show()
    print('Image saved to {}.'.format(filepath))
    
if __name__ == '__main__':
    main()
