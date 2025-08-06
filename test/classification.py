import cv2
from ultralytics import YOLO

# Load model 
model = YOLO('/home/muhnatha/code/SmartTrashBin/best_float32.tflite')

# Open camera
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

print("Starting camera feed. Press 'k' to classify a frame or 'q' to quit.")

while True:
    # Get the current frame from the webcam
    ret, frame = cap.read()
    if not ret:
        print("Can't grab frame")
        break
    
    # Display the live camera feed
    cv2.imshow("Live Webcam Feed", frame)
    
    # Wait for a key press (1 millisecond delay)
    key = cv2.waitKey(1) & 0xFF
    
    # If 'k' is pressed, perform classification
    if key == ord('k'):
        print("\nClassifying frame...")
        highest_confidence = 0.0
        best_detection = None

        # Predict on the current frame
        results = model.predict(frame, verbose=False)
        
        # Loop through all detected boxes to find the one with the highest confidence
        for box in results[0].boxes:
            if box.conf[0] > highest_confidence:
                highest_confidence = box.conf[0]
                best_detection = box
        
                # If an object was found, print its details
        if best_detection is not None:
            class_id = int(best_detection.cls[0])
            label = model.names[class_id]
            confidence = best_detection.conf[0]
            print(f"Top Object Found: {label} (Confidence: {confidence:.2f})")
        else:
            print("No objects were detected in the frame.")

    # If 'q' is pressed, quit the loop
    if key == ord('q'):
        print("Exiting program.")
        break

# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()