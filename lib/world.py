class World:
    
    def __init__(self):
        self.objects = {}

    def add_object(self, name, solid):
        if name not in self.objects:
            self.objects[name] = solid
        else:
            print("Name already exists")

def test():
    from solid import Solid

    world = World()
    car = Solid('data/911.obj')
    world.add_object("Porsche", car)
    print(world.objects)

    print('All tests were successful, no errors!')

if __name__ == '__main__':
    test()
