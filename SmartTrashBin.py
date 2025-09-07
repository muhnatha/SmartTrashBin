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
        
        if confidence > 0.5:
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
            # FIX 1: Read a frame from the camera at the start of every loop
            ret, frame = cap.read()
            if not ret:
                print("ERROR, FAILED to grab frame")
                break

            # FIX 2: Show the frame in a window
            cv2.imshow('Camera Feed', frame)

            # FIX 3: Add waitKey(1). This is ESSENTIAL for imshow to work.
            # It waits 1ms for a key press and allows the window to update.
            # Press 'q' to quit the program.
            if cv2.waitKey(1) & 0xFF == ord('q'):
                print("'q' pressed, stopping program.")
                break
            if ser.in_waiting > 0:
                try:
                    # get Arduino data and decode it
                    line = ser.readline().decode('utf-8', errors='ignore').rstrip()                    
                    sensor_part = line.split('|')[0]
                    value_part = sensor_part.split('S1:')[1]
                    numeric_string = value_part.replace('cm', '')
                    distance_str = numeric_string.strip()
                        
                    # Convert the clean string to a float
                    curr_dist = float(distance_str) 

                except (UnicodeDecodeError, ValueError, IndexError) as e:
                    print(f"Could not decode or parse serial data: {e}")
                    ser.flushInput() # Clear the input buffer to remove bad data
                    continue # Skip to the next loop iteration

                if curr_dist is not None and curr_dist < prev_dist and abs(prev_dist - curr_dist) > diff:
                    print("OBJECT DETECTED, starting camera")
                    time.sleep(5)
                    ret, frame = cap.read()
                    #if not ret:
                    #    print("ERROR, FAILED to grab frame")
                    #    continue
                    #if ret:
                    #    cv2.imshow('Camera Feed', frame)
                    
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
                            command = 0
                        # Is it Inorganic
                        elif class_name in ["glass", "metal", "plastic", "textiles", "trash"]:
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
        cv2.destroyAllWindows()
        print("Camera window closed.")
