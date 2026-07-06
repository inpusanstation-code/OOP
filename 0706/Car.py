class Car: 
    def __init__ (self, make, speed, color, model, price):
        self.make = make
        self.speed = speed
        self.color = color
        self.model = model
        self.price = price
        
    def set_speed(self, speed):
        self.speed = speed
    def get_speed(self):
        return self.speed
    
    def __str__(self):
        return f"Car(speed={self.speed}, color{self.color}, model={self.model})"
    


class ElectricCar(Car):
    def __init__(self, make, model, color, speed, price, batterySize):
        super().__init__(make, speed, color, model, price)
        self.batterySize = batterySize
                
    def set_batterySize(self, batterySize):
        self.batterySize = batterySize
    def get_batterySize(self):
        return self.batterySize
    def __str__(self):
        return f"ElectricCar(make={self.make}, model={self.model}, color={self.color}, speed={self.speed}, batterySize={self.batterySize})"

myCar=ElectricCar("Tesla", "T1", "blue", "100", "100", "200")

print(myCar)
print(f"battery Size: {myCar.batterySize}")


