import serial
import cv2 
import time
import numpy as np
from ultralytics import YOLO

# -- set configuration --
# Serial port configuration
SERIAL_PORT = '/dev/ttyUSB0' # set serial port (change according to raspi)
BAUD_RATE = 9600 # set data transmission speed (change according to arduino)

# Vision model configuration
MODEL_PATH = "best_float32.tflite"

# -- Function --
def classification(frame, model):
    # predict
    results = model.predict(frame, imgsz=320)
    highest_confidence = 0.0 
    best_detection = None

    for box in results[0].boxes:
        if box.conf[0] > highest_confidence:
            highest_confidence = box.conf[0]
            best_detection = box
        
    # If an object was found, print its details
    if best_detection is not None:
        class_id = int(best_detection.cls[0])
        confidence = best_detection.conf[0]
    
        return class_id, confidence
    
    return None, 0.0

# -- Main Program --
if __name__ == '__main__':
    try:
        # initialize connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1, dtr=False)
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
        
        # initialize distance
        prev_dist = -1
        curr_dist = None
        diff = 5 # difference 5 cm
        
        # main loop (idle until HCSR detect object)
        while True:
            # Check if there is data in serial
            if ser.in_waiting > 0:
                # get Arduino data
                line = ser.readline().decode('utf-8').rstrip()
                
                # check if  prev_dist already initialize
                if prev_dist == -1:
                    prev_dist = float(line)

                # check the arduino message
                curr_dist = float(line)
                if abs(prev_dist - curr_dist) > diff:
                    print("OBJECT DETECTED, starting camera")
                    time.sleep(1)
                    ret, frame = cap.read() 
                    if not ret:
                        print("ERROR, FAILE to grab frame")
                        continue
                    
                    # classificate the image
                    class_id, conf = classification(frame, model)
                    
                    # send the class to Arduino
                    if class_id  is not None:
                        # print result
                        print(f"Detected {class_id} with {conf} % confidence")
                        command = f"{class_id}"
                        ser.write(command.encode('utf-8'))
                    else:
                        print("No object detected")
                        
                    # cooldown for sensor
                    time.sleep(3)
                    ser.flushInput()
                    
    except KeyboardInterrupt:
        print("\nProgram stopped by user")
    except Exception as e:
        print(f"An error occured: {e}")
    finally:
        if ser and ser.is_open:
            print("Pulsing DTR to reset Arduino...")
            ser.dtr = True  # Set DTR high
            time.sleep(0.1) # Wait a tenth of a second
            ser.dtr = False # Set DTR low to trigger reset
            ser.close()     # Now close the port
            print("Serial port closed.")
        if cap:
            cap.release()
            print("Camera released.")
