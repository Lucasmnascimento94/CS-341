#!/bin/bash

filePath="$PWD/terminal/usb_devices.txt" 
usbPath="$PWD/terminal/usb.txt" 

function find_usb(){
    ls -l /dev/serial/by-id/ > "$filePath"
    cat "$filePath"
}

function readSerial(){
    PORT_N="${1:-0}"
    BAUD="${2:-115200}"
    PORT="/dev/ttyUSB${PORT_N}"

    ls -l /dev/serial/by-id/ > "$filePath"

    echo "reading from PORT: $PORT - BAUD: $BAUD"
    stty -F  "$PORT" "$BAUD" cs8 -cstopb -parenb -ixon -ixoff -crtscts -echo -icanon
    cat "$PORT"
}

function terminal(){
    # Read positional arguments, defaulting if not provided
    ACTION="${1:-find_usb}"
    PORT="${2:-0}"
    BAUD="${3:-9600}"

    if [[ "$ACTION" == "read" ]];
    then
        readSerial "$PORT" "$BAUD"

    else
        find_usb
    fi
}

# The fix: Pass the script's command-line arguments to the terminal function
terminal "$@"