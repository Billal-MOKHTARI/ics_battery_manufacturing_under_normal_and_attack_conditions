#!/bin/bash
#
# Fetch MySQL server logs for a GNS3 container and filter for 'GENERATED' entries.
# Usage: ./get_mysql_logs.sh

# Get the first matching container ID
container_id=$(docker ps -a | grep gns3 | grep mysql-server | awk '{print $1}' | head -n 1)

# Check if a container was found
if [ -z "$container_id" ]; then
    echo "ERROR: No GNS3 MySQL container found." >&2
    exit 1
fi

# Fetch and filter logs
echo "Checking logs for container: $container_id"
docker logs "$container_id" | grep GENERATED