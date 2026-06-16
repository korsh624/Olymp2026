import cv2
import numpy as np

# Пути
MODEL_PATH = "best.onnx"

# Имена классов
CLASSES = [
    "Gumanoid"
]

# Загрузка сети
net = cv2.dnn.readNetFromONNX(MODEL_PATH)

# Если OpenCV собран с CUDA
# net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
# net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA)

# Для CPU
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

while True:
    ret, frame = cap.read()

    if not ret:
        break

    h, w = frame.shape[:2]

    # Подготовка изображения
    blob = cv2.dnn.blobFromImage(
        frame,
        scalefactor=1 / 255.0,
        size=(640, 640),
        swapRB=True,
        crop=False
    )

    net.setInput(blob)

    outputs = net.forward()

    # YOLOv8 ONNX
    predictions = outputs[0]

    boxes = []
    scores = []
    class_ids = []

    for detection in predictions.T:

        scores_classes = detection[4:]

        class_id = np.argmax(scores_classes)
        confidence = scores_classes[class_id]

        if confidence > 0.5:

            cx, cy, bw, bh = detection[:4]

            x = int((cx - bw / 2) * w / 640)
            y = int((cy - bh / 2) * h / 640)

            bw = int(bw * w / 640)
            bh = int(bh * h / 640)

            boxes.append([x, y, bw, bh])
            scores.append(float(confidence))
            class_ids.append(class_id)

    # NMS
    indices = cv2.dnn.NMSBoxes(
        boxes,
        scores,
        score_threshold=0.5,
        nms_threshold=0.45
    )

    if len(indices) > 0:

        for i in indices.flatten():

            x, y, bw, bh = boxes[i]

            cv2.rectangle(
                frame,
                (x, y),
                (x + bw, y + bh),
                (0, 255, 0),
                2
            )

            label = f"{CLASSES[class_ids[i]]}: {scores[i]:.2f}"

            cv2.putText(
                frame,
                label,
                (x, y - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 255, 0),
                2
            )

            print(label, x, y, bw, bh)

    cv2.imshow("Detection", frame)

    key = cv2.waitKey(1)

    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()