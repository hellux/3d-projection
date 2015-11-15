from PIL import Image
from datetime import datetime

from lib.solid import Solid
from lib.camera import Camera

from math import pi

def main():
    porsche = Solid('data/cube.obj')
    world = {}
    world['porsche'] = porsche
    camera = Camera(pos=(-1, -1.5, -1), rot=(-pi/4, pi/4), res=(240, 240))
    print('Rendering...')
    render(world, camera)

def render(world, camera, filepath=None):
    if filepath == None:
               filepath = 'out/{}_render.png'.format(datetime.now().isoformat('T'))
    image = Image.new('RGB', camera.res)
    pixel_map = camera.calculate_pixel_map(world)
    print(len(pixel_map))
    print('Setting up image...')
    for x in range(image.size[0]):
        for y in range(image.size[1]):
                image.putpixel((x, y), pixel_map[y][x])

    image.save(filepath)
    image.show()
    print('Image save to {}.'.format(filepath))
    

if __name__ == '__main__':
    main()
