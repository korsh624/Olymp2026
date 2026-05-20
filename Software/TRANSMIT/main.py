import serial
import time

ser = serial.Serial('COM21', 115200)

time.sleep(2)

while True:

    speed = 0      # -100 ... 100
    angle = 90      # 45 ... 135

    data = f"{speed},{angle}\n"

    ser.write(data.encode())

    print(data)

    time.sleep(0.05)