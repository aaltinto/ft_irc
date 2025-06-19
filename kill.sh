#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <process_name>"
    exit 1
fi

PROCESS_NAME=$1
COUNT=0
MAX_ATTEMPTS=10
ATTEMPTS=0

echo "Starting to kill all processes matching: $PROCESS_NAME"

# Get the script's own PID and process group
SCRIPT_PID=$$
SCRIPT_PGID=$(ps -o pgid= $SCRIPT_PID | tr -d ' ')

while [ $ATTEMPTS -lt $MAX_ATTEMPTS ]; do
    # Get PIDs of matching processes, excluding this script and its process group
    PIDS=$(pgrep -f "$PROCESS_NAME" | grep -v "^$SCRIPT_PID$" | grep -v "^$SCRIPT_PGID$")
    
    # If no PIDs found, break the loop
    if [ -z "$PIDS" ]; then
        echo "No more processes found matching '$PROCESS_NAME'"
        break
    fi
    
    # Kill each PID
    for PID in $PIDS; do
        # Double check it's not our script or in our process group
        if [ "$PID" != "$SCRIPT_PID" ] && [ "$(ps -o pgid= $PID 2>/dev/null | tr -d ' ')" != "$SCRIPT_PGID" ]; then
            echo "Killing process $PID ($PROCESS_NAME)"
            kill -9 $PID 2>/dev/null
            ((COUNT++))
        fi
    done
    
    ((ATTEMPTS++))
    echo "Attempt $ATTEMPTS of $MAX_ATTEMPTS completed"
    sleep 1
done

if [ $ATTEMPTS -eq $MAX_ATTEMPTS ]; then
    echo "Maximum attempts reached. Some processes might still be running."
fi

echo "Total processes killed: $COUNT"
exit 0