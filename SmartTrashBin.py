import serial
import cv2 
import time
import numpy as np
import ncnn

# -- set configuration --

# Serial port configuration
SERIAL_PORT = '/dev/ttyUSB0' # set serial port (change according to raspi)
BAUD_RATE = 9600

# detection threshold configuration
SENSOR_THRESHOLD_CM = 15 # threshold for trash detection (change accoridng to the structure)

# Vision model configuration
MODEL_PATH = ""
BIN_PATH = ""
LABEL_PATH = ""

# input configuration
WIDTH = 224
HEIGHT = 224

# -- Function --
def load_labels(path):
	with open(path, 'r')  as f:
		return [line.strip() for line in f.readlines()]

def classification(img, model):
	# convert from np_array to Mat
	mat_in = ncnn.Mat.from_pixels(img, ncnn.Mat.PixelType.PIXEL_BGR, WIDTH, HEIGHT)
	
	# normalized image
	mean_vals = [123.675, 116.28, 103.53]
    norm_vals = [1.0/58.395, 1.0/57.12, 1.0/57.375]
    mat_in.substract_mean_normalize(mean_vals, norm_vals)
	
	# inference
	ex = model.create_extractor()
	ex.input("in0", mat_in)
	ret_code, mat_out = ex.extract("out0")
	
	# result
	prediction = np.array(mat_out)
	class_id = np.argmax(prediction)
	confidence = prediction[class_id]
	print(f"classified class {class_id} with confidence {confidence}")
	
	return class_id

# -- Main Program --
if __name__ = '__main__':
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
		model = ncnn.Net()		
		model.opt.use_vulkan_compute = False
		net.load_param(PARAM_PATH)
		net.load_model(BIN_PATH)
		print("NCNN classification model loaded")
		
		# load labels
		labels = load_labels(LABEL_PATH)
		print("Labels loaded")

		print("waiting for HCSR")
		
		# main loop (idle until HCSR detect object)
		while True:
			# Check if there is data in serial
			if ser.in_waiting > 0:
				# get Arduino data
				line = ser.readline().deacode('utf-8').rstrip()
				try:
					dist = int(line)
					print(f"Distance received {dis} cm")

					# check the threshold
					if 0 < dist < DETECTION_THRESHOLD_CM:
						print("OBJECT DETECTED")
						time.sleep(1)

						ret, frame = cap.read()	
						if not ret:
							print("ERROR, FAILE to grab frame")
							continue
						
						# preprocess received image
						input_img = cv2.resize(frame, (WIDTH, HEIGHT))
						
						# classificate the image
						class_id = classificate(input_img, model)
						
						# cooldown for sensor
						time.sleep(3)
						ser.flushInput()
				except (ValueError, IndexError):
					print(f"Invalid serial data: {line}"
				
	except KeyboardInterrupt
		print("\nProgram stopped by user")
	except Exception as e:
		print(f"An error occured: {e}")
	finally:
		if ser and ser.is_open:
			ser.close()
		if cap:
			cap.release()
