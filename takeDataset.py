import cv2

# open tha camera (default = 0)
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Cannor open camera")
    exit()

# numbering image
count = 0
print("Press 's' to save the frame, or 'q' to quit")

while True:
    # read one frame
    ret, frame = cap.read()
    if not ret:
        print("Failed to gram frame")
        break

    # show the video
    cv2.imshow("camera", frame)

    key = cv2.waitKey(1)
    
    # save images
    if key == ord('s'):
        # save image 
        cv2.imwrite(f"images/namaBenda{count}.jpg", frame)
        print("saved")
    # exit
    elif key == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


