class Rectangle:
    def __init__(self, width, height):
        self.width = width
        self.height = height
    def area(self):
        return self.width * self.height
    def __str__(self):
        return f"Rectangle(width={self.width}, height={self.height})"

    def __add__(self, other):   #객체와의 연산 가능하게 만들기
        new_width =self.width + other.width
        new_height = self.height + other.height
        return Rectangle(new_width, new_height)

rect1 = Rectangle(4, 5)  
rect2 = Rectangle(6, 7)  

