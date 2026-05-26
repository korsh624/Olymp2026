import cv2
import numpy as np
import serial
import time
# =========================
# Настройки
# =========================
WIDTH = 1280
HEIGHT = 720
SHOW=True
# Максимальная скорость
MAX_SPEED = 60

# Коэффициенты ПИД
KP = 0.25
KD = 0.12

last_error = 0

# =========================
# Камера
# =========================
cap = cv2.VideoCapture(0)
ser = serial.Serial('/dev/ttyUSB0', 115200)
time.sleep(2)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))
cap.set(cv2.CAP_PROP_FRAME_WIDTH, WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, HEIGHT)

while True:

    ret, frame = cap.read()

    if not ret:
        break

    # -----------------------------------
    # Обрезаем изображение
    # -----------------------------------
    frame = frame[450:, 250:950]

    h, w = frame.shape[:2]

    # -----------------------------------
    # Перевод в HSV
    # -----------------------------------
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Белая разметка
    lower_white = np.array([0, 0, 180])
    upper_white = np.array([255, 80, 255])

    mask = cv2.inRange(hsv, lower_white, upper_white)

    # Убираем шум
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

    # -----------------------------------
    # Ищем контуры
    # -----------------------------------
    contours, _ = cv2.findContours(
        mask,
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE
    )

    left_x = None
    right_x = None

    # Центр изображения
    center_image = w // 2

    # -----------------------------------
    # Ищем левую и правую линию
    # -----------------------------------
    for cnt in contours:

        area = cv2.contourArea(cnt)

        if area < 500:
            continue

        x, y, cw, ch = cv2.boundingRect(cnt)

        cx = x + cw // 2

        # Левая линия
        if cx < center_image:
            if left_x is None or cx > left_x:
                left_x = cx

        # Правая линия
        else:
            if right_x is None or cx < right_x:
                right_x = cx

        # Рисуем контур
        cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)

    # -----------------------------------
    # Вычисляем центр дороги
    # -----------------------------------
    if left_x is not None and right_x is not None:

        road_center = (left_x + right_x) // 2

    elif left_x is not None:

        road_center = left_x + 250

    elif right_x is not None:

        road_center = right_x - 250

    else:
        road_center = center_image

    # -----------------------------------
    # Ошибка
    # -----------------------------------
    error = center_image - road_center

    # ПИД
    derivative = error - last_error

    steering = KP * error + KD * derivative

    last_error = error

    # Ограничение угла
    steering = int(np.clip(steering, -45, 45))

    # -----------------------------------
    # Скорость
    # Чем больше угол — тем меньше скорость
    # -----------------------------------
    speed = MAX_SPEED - abs(steering) * 1.5

    speed = int(np.clip(speed, 40, MAX_SPEED))
    if SHOW:

        # -----------------------------------
        # Отображение
        # -----------------------------------
        cv2.line(frame, (center_image, 0), (center_image, h), (255, 0, 0), 2)

        cv2.circle(frame, (road_center, h // 2), 8, (0, 0, 255), -1)

        cv2.putText(
            frame,
            f"ANGLE: {steering}",
            (20, 40),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 255, 255),
            2
        )
        cv2.putText(
            frame,
            f"SPEED: {speed}",
            (20, 80),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 255, 255),
            2
        )
        cv2.imshow("mask", mask)
        cv2.imshow("frame", frame)
    # -----------------------------------
    # Отправка в Arduino
    # -----------------------------------
    # Пример:
    # serial.write(f"{speed},{steering}\n".encode())
    data = f"{speed},{steering}\n"
    ser.write(data.encode())
    print(data)
    time.sleep(0.05)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()