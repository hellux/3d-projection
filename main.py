from lib.solid import Solid

def main():
    porsche = Solid('data/911.obj')

def render(world, camera, filepath=None):
    if filepath=None:
        filepath = 'out.png'


if __name__ == '__main__':
    main()
