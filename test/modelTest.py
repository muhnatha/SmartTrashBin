from ultralytics import YOLO

# Load your model
model = YOLO('best_float32.tflite')

# Run prediction on the webcam and display the results in real-time
model.predict(source=0, show=True)