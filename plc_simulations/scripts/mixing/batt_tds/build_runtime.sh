#!/bin/bash

# Set variables
name="mixing_plc_runtime"
version="1.0.0"
dockerfile_path="configs/docker/mixing/batt_tds/DockerfileRuntime"
image_path="images/$name.tar"

# Build the Docker image
docker buildx build --load -t "$name:$version" -f "$dockerfile_path" .

# Save the Docker image as a .tar file
docker save -o "$image_path" "$name"
