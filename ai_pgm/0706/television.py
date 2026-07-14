class Television:
    serial_number = 0 #클래스 변수 선언
    def __init__(self, channel, volume, on):
        Television.serial_number +=1
        self.serial_number = Television.serial_number
        self.channel = channel
        self.volume = volume
        self.on = on
    def __str__(self): #객체 출력
        return f"Television (channel={self.channel}, volume={self.volume}, on={self.on})"
  
    def set_channel(self, channel): # 채널 설정 (setter)
        self.channel = channel
    def get_channel(self): # 채널 가져오기 (getter)
        return self.channel
    
tv1 = Television(1, 10, True) #객체 생성
tv2 = Television(2, 20, False) #객체 생성
tv3 = Television(3, 30, True)

print(tv1.get_channel()) #tv1의 채널 가져오기
print(tv1.channel) # tv1의 채널 가져오기 

print(tv1)
print(tv2)
print(tv3)








    