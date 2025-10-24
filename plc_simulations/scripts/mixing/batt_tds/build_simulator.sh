#!/bin/bash

# Set variables
name="mixing_plc_simulator"
dockerfile_path="configs/docker/mixing/batt_tds/DockerfileSimulator"
image_path="images/$name.tar"

# Build the Docker image
docker buildx build --platform linux/amd64 -t "$name" -f "$dockerfile_path" .

# Save the Docker image as a .tar file
docker save -o "$image_path" "$name"
