import math 

# Circle 클래스를 정의한다. 
class Circle:
    def __init__(self, radius = 0):
        self.radius = radius

    def area(self):
        return math.pi * (self.radius**2)

    def getArea(self):
        return  math.pi * (self.radius * self.radius)

    def getPerimeter(self):
        return 2 * math.pi * self.radius 
    
    def circumference(self):
        return 2*math.pi *self.radius

# Circle 객체를 생성한다. 
c1 = Circle(5)
print (f"Circle with radius {c1.radius}:")
print (f"Area: {c1.area()}")
print(f"Circumference: {c1.circumference(): }")

c2 = Circle(10)
print (f"Circle with radius {c2.radius}:")
print (f"Area: {c2.area()}")
print(f"Circumference: {c2.circumference(): }")

