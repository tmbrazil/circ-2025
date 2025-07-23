import cv2
import numpy as np

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_250)

aruco_params = cv2.aruco.DetectorParameters()

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("erro")
    exit()

print('q')

while True:
    ret, frame = cap.read()
    if not ret:
        print("nao foi possível receber o frame")
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    corners, ids, rejected_img_points = cv2.aruco.detectMarkers(gray, aruco_dict, parameters=aruco_params)

    if ids is not None:
        cv2.aruco.drawDetectedMarkers(frame, corners, ids)

    cv2.imshow('Detector de Aruco', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
