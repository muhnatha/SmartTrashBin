import serial
import cv2 
import time
import numpy as np
from ultralytics import YOLO

# -- set configuration --

# Serial port configuration
SERIAL_PORT = '/dev/ttyUSB0' # set serial port (change according to raspi)
BAUD_RATE = 9600

# detection threshold configuration
DETECTION_THRESHOLD_CM = 15 # threshold for trash detection (change accoridng to the structure)

# Vision model configuration
MODEL_PATH = "best_float32.tflite"

# -- Function --
def classification(frame, model):
    # predict
	results = model.predict(frame, imgsz=320)
    
	for box in results[0].boxes:
		if box.conf[0] > highest_confidence:
			highest_confidence = box.conf[0]
			best_detection = box
        
                # If an object was found, print its details
		if best_detection is not None:
			class_id = int(best_detection.cls[0])
			confidence = best_detection.conf[0]
    
	return class_id, confidence

# -- Main Program --
if __name__ == '__main__':
    try:
        # initialize connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        ser.flush() # clear any old data
        print(f"successfuly connected to Arduino on {SERIAL_PORT}")

        # initialize camera
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            raise IOError("Cannot open camera")
        print("Camera sucessfully opened")
        
        # load model
        model = YOLO(MODEL_PATH)
        print("waiting for HCSR")
        
        # main loop (idle until HCSR detect object)
        while True:
            # Check if there is data in serial
            if ser.in_waiting > 0:
                # get Arduino data
                line = ser.readline().deacode('utf-8').rstrip()
                try:
                    dist = int(line)
                    print(f"Distance received {dist} cm")

                    # check the threshold
                    if 0 < dist < DETECTION_THRESHOLD_CM:
                        print("OBJECT DETECTED")
                        time.sleep(1)

                        ret, frame = cap.read() 
                        if not ret:
                            print("ERROR, FAILE to grab frame")
                            continue
                        
                        # classificate the image
                        class_id, conf = classification(frame, model)
                        
                        # cooldown for sensor
                        time.sleep(3)
                        ser.flushInput()
                except (ValueError, IndexError):
                    print(f"Invalid serial data: {line}")
                
    except KeyboardInterrupt:
        print("\nProgram stopped by user")
    except Exception as e:
        print(f"An error occured: {e}")
    finally:
        if ser and ser.is_open:
            ser.close()
        if cap:
            cap.release()
