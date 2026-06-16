import numpy as np
import imutils
import time
import cv2

video_source = 0
frame_width = 1280        
frame_height = 960

CONFIDENCE_THRESHOLD = 0.5  # порог уверенности
NMS_THRESHOLD = 0.4  # параметр подавления не максимумов

WEIGHTS_FILE = 'weights/yolov4-tiny-original.weights'  # Путь до весов YOLO
NET_CFG_FILE = 'weights/yolov4-tiny-original.cfg'  # Путь до cfg-файла для YOLO
CLASSES_FILE = 'weights/yolov4-tiny-original_classes.txt'  # Путь до файла где лежат названия классов на англ. языке
CLASSES_FILE_RU = 'weights/yolov4-tiny-original_classes_ru.txt'  # Путь до файла где лежат названия классов на рус. языке
RUS_FONT = True  # Использовать русские названия классов? True - русские, False - английские

cap = cv2.VideoCapture(video_source, cv2.CAP_DSHOW)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))
cap.set(cv2.CAP_PROP_FRAME_WIDTH, frame_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, frame_height)

def load_model(weights_file: str, net_config_file: str):
    print(f"Loading model '{weights_file}'...", end='')
    net = cv2.dnn.readNet(weights_file, net_config_file)
    net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
    net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)
    net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
    net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA)
    print("done.")

    model = cv2.dnn_DetectionModel(net)
    model.setInputParams(size=(416, 416), scale=1 / 255)
    return model

# Загружаем модель
detector = load_model(WEIGHTS_FILE, NET_CFG_FILE)

# Загружаем названия классов
if RUS_FONT:
    with open(CLASSES_FILE_RU, encoding='utf-8') as file:
        common_classes = file.read().splitlines()
        print("Classes:", common_classes)
else:
    with open(CLASSES_FILE) as file:
        common_classes = file.read().splitlines()
        print("Classes:", common_classes)


# loop over the frames from the video stream
while True:
	time_start = time.monotonic()
	# grab the frame from the threaded video stream and resize it
	# to have a maximum width of 400 pixels
	ret, frame = cap.read()
	detect_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

	classes, scores, boxes = detector.detect(detect_frame, CONFIDENCE_THRESHOLD, NMS_THRESHOLD)

	detections = []
	for classid, score, box in zip(classes, scores, boxes):
		cls_name = common_classes[classid]
		label = f'{cls_name} [{score * 100:.2f}%]'
		detections.append((label, box))
	# cv2.rectangle(frame, box, color, 2)
	# cv2.putText(frame, label, (box[0], box[1] - 10), FONT, 0.5, color, 2)

	print(detections)
	print("Time, ms : ", (time.monotonic()-time_start) * 1000)

