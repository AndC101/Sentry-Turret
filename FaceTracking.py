#Andrew Chen and Anish Nagariya
#Jan 22nd 2024
#Uses OpenCV and a preloaded model to detect and send serialized info to the arduino.

import time # library to pause code
import cv2 as cv # library to use camera
import mediapipe as mp # library for face detection
import numpy as np # coordinate and number preprocessing
import serial # connect to arduino
mp_face_detection = mp.solutions.face_detection # import face detection module
cap = cv.VideoCapture(0) # initialize camera
ser = serial.Serial('COM5', 115200, timeout=0.1) # initalize arduino


with mp_face_detection.FaceDetection(model_selection=1, min_detection_confidence=0.5) as face_detector: # use camera is object to detect face
    frame_counter = 0 # number of frames for FPS
    fonts = cv.FONT_HERSHEY_PLAIN # fon we are using
    start_time = time.time() # time it takes
    cFace = [0, 0] # coordinates of face (initalize)
    while True: # run forever unless key q pressed
        frame_counter += 1 # add frames
        ret, frame = cap.read() # capture image using camera
        if ret is False:
            break
        rgb_frame = cv.cvtColor(frame, cv.COLOR_BGR2RGB) # get colored image


        results = face_detector.process(rgb_frame) # parse picture into face detection
        frame_height, frame_width, c = frame.shape # split frame into more detailed object
        if results.detections: # check if face detected
            for face in results.detections: # for every face find coordinates
                face_react = np.multiply(
                    [
                        face.location_data.relative_bounding_box.xmin,
                        face.location_data.relative_bounding_box.ymin,
                        face.location_data.relative_bounding_box.width,
                        face.location_data.relative_bounding_box.height,
                    ],
                    [frame_width, frame_height, frame_width, frame_height]).astype(int)
                
                #displays coordinates for the top left corner of the face recognition box
                cFace = [(face.location_data.relative_bounding_box.xmin + face.location_data.relative_bounding_box.width/2)*100 , (face.location_data.relative_bounding_box.ymin + face.location_data.relative_bounding_box.height/2 ) *100]
                cv.putText(frame, str(round(cFace[0])) + " " + str(round(cFace[1])), (100, 100),cv.FONT_HERSHEY_DUPLEX,0.7,(0, 255, 255),2,)

                # send arduino location of face and angles for servos
                ser.write(f"pan:{round(cFace[0])}\n".encode())
                ser.write(f"tilt:{round(cFace[1])}\n".encode())
                ser.write("high".encode())

                # draw rectangles around the face in camera image
                cv.rectangle(frame, face_react, color=(255, 255, 255), thickness=2)
                key_points = np.array([(p.x, p.y) for p in face.location_data.relative_keypoints])
                key_points_coords = np.multiply(key_points,[frame_width,frame_height]).astype(int)
                
                # coordinates of face
                for p in key_points_coords:
                    cv.circle(frame, p, 4, (255, 255, 255), 2)
                    cv.circle(frame, p, 2, (0, 0, 0), -1)
        # if face not detected send low to arduino
        else:
            ser.write("low".encode())

        
        fps = frame_counter / (time.time() - start_time) # calculate FPS
        cv.putText(frame,f"FPS: {fps:.2f}",(30, 30),cv.FONT_HERSHEY_DUPLEX,0.7,(0, 255, 255),2,) # display FPS
        cv.imshow("frame", frame) # show picture
        key = cv.waitKey(1) # check if any key is pressed on keyboard
        if key == ord("q"): # if q is pressed stop program
            break


    cap.release() # release captured images
    cv.destroyAllWindows() # close open instances of camera