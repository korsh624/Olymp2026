import cv2
cap=cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
while True:
     ret,frame=cap.read()
     frame = frame[450:, 250:950]
     cv2.imshow('frame',frame)

     if cv2.waitKey(1) & 0xFF == ord('q'):
         break
cv2.destroyAllWindows()
cap.release()