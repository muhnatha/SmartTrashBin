# Start from the official Ubuntu 22.04 base image

FROM ubuntu:22.04


# Set environment variables to prevent interactive prompts during installation

ENV DEBIAN_FRONTEND=noninteractive


# Update package lists and install essential dependencies

# - python3 and python3-pip: For running our Python script

# - ffmpeg libsm6 libxext6: Dependencies for OpenCV

# - libgl1-mesa-glx: Required for GUI windows (like cv2.imshow) to work

RUN apt-get update && apt-get install -y \

    python3 \

    python3-pip \

    ffmpeg \

    libsm6 \

    libxext6 \

    libgl1-mesa-glx \

    && rm -rf /var/lib/apt/lists/*


# Install the OpenCV library using pip

RUN pip3 install opencv-python


# Set the working directory inside the container

WORKDIR /app
