import serial
import cv2 
import ultralytics

# set configuration
SERIAL_PORT = '/dev/ttyUSB0' # set serial port (change according to raspi)
BAUD_RATE = 9600
SENSOR_THRESHOLD_CM = 15 # threshold for trash detection (change accoridng to the structure)
MODEL_PATH = ""

# Main
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
						print("PAUSING for 1 second")
						time.sleep(1)

						ret, frame = cap.read()	
						if not ret:
							print("ERROR, FAILE to gram frame"

				except (ValueError, IndexError):
					print(f"Invalid serial data: {line}"
				

