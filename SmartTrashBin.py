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
MODEL_PATH = "classification_model.tflite"

# Class name
CLASS_NAMES = [
    "battery", "organic_waste", "paper_cardboard", "glass",
    "metal", "plastic", "textiles", "trash"
]

# -- Function --
def classification(frame, model):
    # predict
    results = model.predict(frame, imgsz=224)

    # Check if a classification was made
    if results[0].probs is not None:
        # FIX: For TFLite, get the raw probability tensor and find the max
        # Get the index of the highest probability for the class ID
        class_id = np.argmax(results[0].probs.data.cpu().numpy())
        # The confidence is the highest value in that tensor
        confidence = np.max(results[0].probs.data.cpu().numpy())
        
        return class_id, confidence
        
    # Return None if no classification was made
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
        model = YOLO(MODEL_PATH, task='classify')
        print("waiting for HCSR")
        
        # initialize distance
        prev_dist = 35
        curr_dist = None
        diff = 5 # difference 5 cm
        
        # main loop (idle until HCSR detect object)
        while True:
            # Check if there is data in serial
            ret, frame = cap.read()
            if not ret:
                print("ERROR, FAILED to grab frame")
                break
            
            if ser.in_waiting > 0:
                signal = ser.read().decode('utf-8')

                if signal == 'D':
                    print("OBJECT DETECTED, starting camera")
                    #  time.sleep(1)
                    ret, frame = cap.read()
                    
                    # classificate the image
                    class_id, conf = classification(frame, model)

                    # send the class to Arduino
                    if class_id is not None:
                        # print result
                        class_name = CLASS_NAMES[class_id]
                        print(f"Detected {class_name} with {conf*100:.2f}% confidence")
                        # Initialize command as None
                        command = None

                        # Is it Battery Waste?
                        if class_name in ["battery"]:
                            print("Battery Waste")
                            command = 2
                        # Is it Organic?
                        elif class_name in ["organic_waste", "paper_cardboard"]:
                            print("ORGANIC")
                            command = 1
                        # Is it Inorganic
                        elif class_name in ["glass", "metal", "plastic", "textiles", "trash"]:
                            print("INORGANIC")
                            command = 0

                        # send command to arduino
                        if command is not None:
                            ser.write(bytes([command]))
                        else:
                            print("No class detected")
                    else:
                        print("No object detected")

                    # cooldown for sensor
                    # time.sleep(3)
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
        cv2.destroyAllWindows()
        print("Camera window closed.")
