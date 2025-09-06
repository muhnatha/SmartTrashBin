import serial
import cv2 
import time
import numpy as np
from ultralytics import YOLO

# -- set configuration --
# Serial port configuratio
SERIAL_PORT = '/dev/ttyUSB0' # set serial port (change according to raspi)
BAUD_RATE = 9600 # set data transmission speed (change according to arduino)

# Vision model configuration
MODEL_PATH = "best_float32.tflite"

# Class name
CLASS_NAMES = [
    "cans", "cardbox", "food_scraps", "glass_like_plastic", 
    "paper_material", "plastic"
]

# -- Function --
def classification(frame, model):
    # predict
    results = model.predict(frame, imgsz=640)
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
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    dtr = False
    ser.flush()
    try:
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
                sensor_part = line.split('|')[0]
                value_part = sensor_part.split(':')[1]
                numeric_string = value_part.replace('cm', '')
                line = numeric_string.strip()
                
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
                        class_name = CLASS_NAMES[class_id]
                        print(f"Detected {class_name} with {conf} % confidence")
                        
                        # Is it organic?
                        if class_name in ["food_scraps"]:
                            print("ORGANIC")
                            command = 0
                        # Is it inorganic
                        elif class_name in ["cans", "cardbox", "glass_like_plastic", "paper_material", "plastic"]:
                            print("INORGANIC")
                            command = 1
                        
                        # send command to arduino
                        if command is not None:
                            ser.write(bytes([command]))
                        else:
                            print("No class detected")
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
