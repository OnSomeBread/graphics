class Voxel:
    def __init__(self, x, y, z = 0, w = 1):
        self.x:float = x
        self.y:float = y
        self.z:float = z
        self.w:float = w

    def vprint(self):
        print(self.x/self.w, self.y/self.w, self.z/self.w)


def bezierCurve(controlPoints: list, t:float):
    cpoints = controlPoints.copy()
    while len(cpoints) > 1:
        for i in range(len(cpoints) - 1):
            cpoints[i].x += t * (cpoints[i + 1].x - cpoints[i].x)
            cpoints[i].y += t * (cpoints[i + 1].y - cpoints[i].y)
            cpoints[i].z += t * (cpoints[i + 1].z - cpoints[i].z)
            cpoints[i].w += t * (cpoints[i + 1].w - cpoints[i].w)
        cpoints.pop()
    return cpoints[0]


points = [Voxel(-1, 0, 0, 1), Voxel(0, 1, 0, 1), Voxel(3, 0, 0, 3)]
bezierCurve(points, .25).vprint()