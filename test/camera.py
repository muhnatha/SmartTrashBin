import cv2  

# open default camera
cap = cv2.VideoCapture(0)

# check if cap is open
if not cap.isopened():
    print("Cannor open camera")
    exit()

while(True):
    # read frame if there is
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break
    
    # show the stream
    cv2.imshow("camera", frame)

    # keyboard interrupt
    if cv2.waitkey(1) & 0xFF == ord('q'):
        break

# release all visual
cap.release()
cv2.destroyAllWindows()
