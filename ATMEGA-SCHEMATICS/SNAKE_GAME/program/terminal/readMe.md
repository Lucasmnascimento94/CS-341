# USB Serial Terminal Scripts

Two small, cross-platform helpers to open a **USB-UART data stream** from your board.
- **Linux/macOS:** Bash (`terminal/terminal_linux.sh`)
- **Windows:** PowerShell (`terminal/terminal_win.ps1`)

Use these to quickly read/hex-dump what your microcontroller prints over a USB-to-UART adapter (like printf in C / print in Python).

---

## Quick start

### Linux / macOS
List devices (stable paths under `/dev/serial/by-id` on Linux):

./terminal/terminal_linux.sh find_usb_device

# Example Output
lrwxrwxrwx ... usb-FTDI_FT232R_USB_UART_BG00XUIF-if00-port0 -> ../../ttyUSB1
lrwxrwxrwx ... usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0 -> ../../ttyUSB0


# by number (0 -> /dev/ttyUSB0)
./terminal/terminal_linux.sh read 0 9600

Note: 9600 is a baud rate, so you will have to send the baud rate that your device is sending data.


### Windows OS
TO DO