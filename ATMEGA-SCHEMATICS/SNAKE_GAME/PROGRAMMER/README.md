## Installation
1. [Driver](https://ftdichip.com/drivers/d2xx-drivers/)
2. Python package to run Python script and commands[PyFtdi](https://eblot.github.io/pyftdi/)
3. USB driver installtion for Windows, idk about Linux [Zadig](https://zadig.akeo.ie/)
    * To locate the device, make sure to use libusbK

----------------
## Commands
1. ```python -c "from pyftdi.ftdi import Ftdi; Ftdi.show_devices()"``` -> Shows connected devices
2.```python ftconf.py ftdi://ftdi:232:A50285BI/1 -o eeprom_config.ini``` -> Outputs the EEPROM configuration into "eeprom_config.ini"
3. ```python ftconf.py ftdi://ftdi:232:A50285BI/1 -c?``` -> Lists all supported configuration parameters in EEPROM
4. ```python ftconf.py ftdi://ftdi:232:A50285BI/1 -c [name_of_the_pin]:?``` -> Shows all configuration values
5. ```python ftconf.py --help``` -> For EXTRA HELP
6. ``````python ftconf.py ftdi://ftdi:232:A50285BI/1 -c cbus_func_0:CLK6 -c cbus_func_1:TXLED -c cbus_func_2:RXLED -c cbus_func_3:GPIO -u``` -> configure the pins in EEPROM, and update it (MAKE SURE TO INCLUDE ```-u``` or it wont update the EEPROM)
----------------

## FYI
```./Converter_Code/Show_Devices.py``` shows connected devices (same as 1 - command)
```./Converter_Code/EEPROM.py``` prints out the current EEPROM configuration
```./current_config.ini``` is the output file of the current EEPROM configuration by using this comm


## Useful sources:
[EEPROM Management](https://eblot.github.io/pyftdi/eeprom.html)
[Programmer Guide](https://ftdichip.com/wp-content/uploads/2023/09/D2XX_Programmers_Guide.pdf) (I think this is something you would use a lot)

