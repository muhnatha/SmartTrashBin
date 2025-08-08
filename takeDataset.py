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
    
    # resize the image
    frame = cv2.resize(frame, (640, 640))
    # show the video
    cv2.imshow("camera", frame)

    key = cv2.waitKey(1)
    
    # save images
    if key == ord('s'):
        # save image 
        cv2.imwrite(f"/home/muhnatha/Documents/images_SmartTrashBin/paper/paper{count}.jpg", frame)
        print("saved")
        count = count+1
    # exit
    elif key == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


