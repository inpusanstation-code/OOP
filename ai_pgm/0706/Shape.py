class Shape:
    def __init__(self, name):    
        self.name = name
    def getArea(self):
        raise NotImplementedError("이것은 추상메소드입니다. ")

class Circle(Shape):
    def __init__(self, name, radius):    
        super().__init__(name)
        self.radius = radius

    def getArea(self):             
        return  3.141592*self.radius**2

class Rectangle(Shape):
    def __init__(self, name, width, height):    
        super().__init__(name)
        self.width = width
        self.height = height

    def getArea(self):             
        return  self.width*self.height


shapeList = [Circle(5), Rectangle(4, 6)]
for shape in shapeList:
    print(f"{shape.name} area: {shape.area()}")
    
    