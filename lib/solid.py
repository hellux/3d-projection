class Solid:
    """Stores vertices and faces for a geometric solid"""
    
    def __init__(self, file_path):
        file_object = open(file_path, 'r')
        self.vertices, self.faces = Solid._parse_obj_data(file_object)
    
    @staticmethod
    def _parse_obj_data(file_object):
        """Parse vertices and faces from a .obj file.
        
        File structure:

        Vertex line:
        v x y z
        x, y, z are float coordinates for a vertex

        Face / triangle line:
        f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3A
        vx, vtx, vnx are indices for a vertices, texture coordinates, vertex normals
        """
        
        vertices = [] # list of coordinates - [[x, y, z], ... ]
        faces = [] # list of indices of vertices for faces - [[v1, v2, v3] ... ]

        while True:
            line = file_object.readline()
            line = line[:line.find('#')] #remove comments
            segments = line.split() 
            if len(segments) == 0: #skip if line is empty
                continue
            if segments[0] == 'v':
                vertices.append([float(coord) for coord in segments[1:]])
            elif segments[0] == 'f':
                face = []
                indices = segments[1:]
                if len(indices) != 3:
                    print("Face is not a triangle")
                for index in indices:
                    face.append(int(index[:index.find('/')]))
                faces.append(face)

        return tuple(vertices), tuple(faces) #return as immutable tuples

def test():
    obj = Solid("data/911.obj")
    
    assert len(obj.vertices) == 32918
    assert len(obj.faces) == 22011
    assert obj.vertices[0] == [-0.830351, -0.359413, -0.750083] 
    assert obj.vertices[obj.faces[23][0]][0] == -0.821758 
    
    print("All tests were succesful, no errors!")
    
if __name__ == '__main__':
    test()
