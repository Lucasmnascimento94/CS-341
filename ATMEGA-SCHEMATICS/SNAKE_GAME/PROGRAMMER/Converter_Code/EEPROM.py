from pyftdi.eeprom import FtdiEeprom

#  EEPROM manager
eeprom = FtdiEeprom()

# Select the FTDI device to access
eeprom.open('ftdi://ftdi:232:A50285BI/1')

# Show the EEPROM content
eeprom.dump_config()

# Show the raw EEPROM content
from pyftdi.misc import hexdump
print(hexdump(eeprom.data))