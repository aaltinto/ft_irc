#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <server_ip> <port>"
    exit 1
fi

SERVER=$1
PORT=$2
CHANNEL="test"
NPC_COUNT=10
SIGNAL_FILE="/tmp/npc_message_signal"
MESSAGE_FILE="/tmp/npc_message_content"

# Clean up previous signal files if they exist
rm -f $SIGNAL_FILE $MESSAGE_FILE

# Names and surnames lists
NAMES=(
    "Alice" "Bob" "Charlie" "Dave" "Eve" "Frank" "Grace" "Heidi" "Ivan" "Judy"
    "Karl" "Linda" "Mike" "Nancy" "Oscar" "Peggy" "Quinn" "Rita" "Steve" "Tina"
    "Ursula" "Victor" "Walter" "Xena" "Yolanda" "Zack"
)

SURNAMES=(
    "Smith" "Johnson" "Williams" "Brown" "Davis" "Miller" "Wilson" "Moore" "Taylor" "Anderson"
    "Thomas" "Jackson" "White" "Harris" "Martin" "Thompson" "Garcia" "Martinez" "Robinson" "Clark"
)

# Function to handle cleanup on script termination
cleanup() {
    echo "Cleaning up and terminating NPCs..."
    rm -f $SIGNAL_FILE $MESSAGE_FILE
    kill $(jobs -p) 2>/dev/null
    exit 0
}

# Set up cleanup on script termination
trap cleanup SIGINT SIGTERM

for i in $(seq 1 $NPC_COUNT); do
    # Create unique name
    NAME_INDEX=$((RANDOM % ${#NAMES[@]}))
    SURNAME_INDEX=$((RANDOM % ${#SURNAMES[@]}))
    NPC_NAME="${NAMES[$NAME_INDEX]}${SURNAMES[$SURNAME_INDEX]}${i}"
    
    # Connect to server with nc and send IRC commands
    (
        {
            echo "PASS a "
            sleep 1
            echo "NICK ${NPC_NAME} "
            sleep 1
            echo "USER ${NPC_NAME} 0 * :${NPC_NAME} "
            sleep 1
            echo "JOIN #${CHANNEL}"
            
            # Keep connection open and check for signal to send message
            while true; do
                if [ -f "$SIGNAL_FILE" ]; then
                    MESSAGE=$(cat "$MESSAGE_FILE" 2>/dev/null)
                    if [ -n "$MESSAGE" ]; then
                        echo "PRIVMSG #${CHANNEL} :${MESSAGE}"
                        # Small random delay to prevent flooding
                        sleep $(echo "scale=4; 0.1 + 0.2 * $RANDOM / 32767" | bc)
                    fi
                fi
                sleep 1
            done
        } | nc $SERVER $PORT
    ) &
    
    # Short delay to avoid overloading the server
    sleep 0.1
    
    # Show progress
    if [ $((i % 50)) -eq 0 ]; then
        echo "${i} NPCs connected..."
    fi
done

echo "${NPC_COUNT} NPCs have been connected to server ${SERVER}:${PORT} and joined channel #${CHANNEL}"
echo "Enter a message and all NPCs will send it to the channel."
echo "Type 'exit' to terminate all connections."

# Wait for commands from terminal
while true; do
    read -p "Message to send (or 'exit'): " USER_MESSAGE
    
    if [ "$USER_MESSAGE" = "exit" ]; then
        cleanup
        exit 0
    else
        # Create signal files for NPCs
        echo "$USER_MESSAGE" > $MESSAGE_FILE
        touch $SIGNAL_FILE
        echo "Signaling NPCs to send message..."
        sleep 2
        rm -f $SIGNAL_FILE
        echo "Done."
    fi
done