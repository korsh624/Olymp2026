import cv2

# Открываем веб-камеру
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

# Проверяем открытие камеры
if not cap.isOpened():
    print("Не удалось открыть камеру")
    exit()

# Получаем разрешение камеры
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

# Кодек для MP4
fourcc = cv2.VideoWriter_fourcc(*'mp4v')

# Создаем объект записи видео
out = cv2.VideoWriter(
    'output.mp4',
    fourcc,
    30.0,               # FPS
    (width, height)
)

print("Нажмите ESC для завершения записи")

while True:
    ret, frame = cap.read()

    if not ret:
        break

    # Записываем кадр
    out.write(frame)

    # Показываем изображение
    cv2.imshow("Camera", frame)

    # Выход по ESC
    if cv2.waitKey(1) == 27:
        break

# Освобождаем ресурсы
cap.release()
out.release()
cv2.destroyAllWindows()

print("Видео сохранено в output.mp4")