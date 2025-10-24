#!/bin/bash

# Set variables
name="mixing_plc_data_logger"
version="1.0.0"
dockerfile_path="configs/docker/mixing/batt_tds/DockerfileDataLogger"
image_path="configs/images/$name.tar"

# Build the Docker image
docker buildx build --load -t "$name:$version" -f "$dockerfile_path" .

# Save the Docker image as a .tar file
docker save -o "$image_path" "$name"