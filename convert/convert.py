import re

vertices = []
uvs = []
normals = []

builtVertices = []
vertexMap = {}
builtIndices = []

class OBJVertex:
    def __init__(self, x, y, z) -> None:
        self.x = x
        self.y = y
        self.z = z
class OBJUV:
    def __init__(self, u, v) -> None:
        self.u = u
        self.v = v
class OBJNormal:
    def __init__(self, x, y, z) -> None:
        self.x = x
        self.y = y
        self.z = z
class Vertex:
    def __init__(self, x, y, z, u, v, nx, ny, nz, textured, normals) -> None:
        self.x = x
        self.y = y
        self.z = z
        self.u = u
        self.v = v
        self.nx = nx
        self.ny = ny
        self.nz = nz
        self.textured = textured
        self.normals = normals

inFile = input("Input File: ");
outFile = input("Output File: ");

with open(str(inFile), 'r') as input:
    for line in input.readlines():
        line = re.sub("\s+", " ", line)
        tokens = line.strip().split(' ')
        command = tokens[0]
        tokens = tokens[1::]
        
        if command == 'v':
            x = float(tokens[0])
            y = float(tokens[1])
            z = float(tokens[2])
            vertices.append(OBJVertex(x,y,z))
        if command == 'vt':
            u = float(tokens[0])
            v = float(tokens[1])
            uvs.append(OBJUV(u, v))
        if command == 'vn':
            x = float(tokens[0])
            y = float(tokens[1])
            z = float(tokens[2])
            normals.append(OBJNormal(x,y,z))
        if command == 'f':
            for token in tokens:
                splitTokens = token.split('/')
                vertex = vertices[int(splitTokens[0]) - 1]

                doUv = False
                uv = OBJUV(0, 0)
                if len(splitTokens) >= 2:
                    if len(splitTokens[1]) > 0:
                        uv = uvs[int(splitTokens[1]) - 1]
                        doUv = True

                doNormals = False
                normal = OBJNormal(0,0,0)
                if len(splitTokens) >= 3:
                    if len(splitTokens[2]) > 0:
                        normal = normals[int(splitTokens[2]) - 1]
                        doNormals = True

                if token not in vertexMap:
                    builtVertices.append(Vertex(vertex.x, vertex.y, vertex.z, uv.u, uv.v, normal.x, normal.y, normal.z, doUv, doNormals))
                    vertexMap[token] = len(builtVertices) - 1
                
                builtIndices.append(vertexMap[token])

with open(str(outFile), "w") as output:
    output.write("vertices %d\n" % (len(builtVertices)))
    for v in builtVertices:
        output.write("%f %f %f %f %f %f %f %f %d %d\n" %
            (v.x, v.y, v.z, v.u, v.v, v.nx, v.ny, v.nz, 0 if not v.textured else 1, 0 if not v.normals else 1)
        )
    output.write("indices %d\n" % (len(builtIndices)))
    output.write(' '.join([str(x) for x in builtIndices]))