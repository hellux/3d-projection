from lib.vector import vector_subtract, cross_product

class Solid:
    """Stores vertices and faces for a geometric solid"""
    
    def __init__(self, file_path):
        file_object = open(file_path, 'r')
        print('Parsing file at {}...'.format(file_path))
        self.vertices, self.faces = Solid._parse_obj_data(file_object)
        self.triangles = self._create_triangles()
        print('Finished, {} vertices and {} faces found'.format(len(self.vertices),
                                                                len(self.faces)))
    
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
            if len(line) == 0: #empty line means eof
                break
            line = line[:line.find('#')] #remove comments
            segments = line.split() 
            if len(segments) == 0: #skip if line is empty
                continue
            if segments[0] == 'v':
                #remove 'v' segment and convert all to floats
                vertices.append([float(coord) for coord in segments[1:]])
            elif segments[0] == 'f':
                face = []
                indices = segments[1:] #remove 'f' segment
                if len(indices) != 3:
                    print("Face is not a triangle")
                for index in indices:
                    separate_indices = index.split('/')
                    face.append(int(separate_indices[0]))
                faces.append(face)

        return tuple(vertices), tuple(faces) #return as immutable tuples

    def _create_triangles(self):
        triangles = []
        for face in self.faces:
            T = self.get_vertices(face) #vertices t0, t1, t2
            normal = Solid.get_plane_normal(T)
            triangles.append({'normal': normal, 'vertices': T})
        return triangles

    def get_plane_normal(triangle_vertices):
        """calculates and returns the normal for a plane that a triangle lies on"""
        return cross_product(
            vector_subtract(triangle_vertices[1], triangle_vertices[0]),
            vector_subtract(triangle_vertices[2], triangle_vertices[0]))


    def get_vertices(self, triangle):
        vertices = []
        for index in triangle:
            vertices.append(self.vertices[index-1])
        return tuple(vertices)

def test():
    obj = Solid("data/911.obj")
    
    assert len(obj.vertices) == 32918
    assert len(obj.faces) == 22011
    assert obj.vertices[0] == [-0.830351, -0.359413, -0.750083] 
    assert obj.vertices[obj.faces[23][0]][0] == -0.821758 
    
    print("All tests were succesful, no errors!")
    
if __name__ == '__main__':
    test()
