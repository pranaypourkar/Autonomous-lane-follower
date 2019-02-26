#!/usr/bin/python
import smbus
import math
import xlsxwriter
import signal
import sys
# Register
power_mgmt_1 = 0x6b
power_mgmt_2 = 0x6c

def read_byte(reg):
    return bus.read_byte_data(address, reg)

def read_word(reg):
    h = bus.read_byte_data(address, reg)
    l = bus.read_byte_data(address, reg+1)
    value = (h << 8) + l
    return value

def read_word_2c(reg):
    val = read_word(reg)
    if (val >= 0x8000):
        return -((65535 - val) + 1)
    else:
        return val

def dist(a,b):
    return math.sqrt((a*a)+(b*b))

def get_y_rotation(x,y,z):
    radians = math.atan2(x, dist(y,z))
    return -math.degrees(radians)

def get_x_rotation(x,y,z):
    radians = math.atan2(y, dist(x,z))
    return math.degrees(radians)

def get_z_rotation(x,y,z):
    radians = math.atan2(z, dist(x,y))
    return math.degrees(radians)

def signal_handler(sig, frame):
        print('You pressed Ctrl+C!')
        wb.close()
        sys.exit(0)

bus = smbus.SMBus(1) # bus = smbus.SMBus(0) fuer Revision 1
address = 0x68       # via i2cdetect

# Aktivieren, um das Modul ansprechen zu koennen
bus.write_byte_data(address, power_mgmt_1, 0)

# **************************
wb = xlsxwriter.Workbook('sample-test.xlsx')
ws = wb.add_worksheet('my sheet')

# write the header in row 0, which is Excel row 1
ws.write_row(0, 0, ['angle'])
i=0;
# **************************

while(1):
     gyroskop_xout = read_word_2c(0x43)
     gyroskop_yout = read_word_2c(0x45)
     gyroskop_zout = read_word_2c(0x47)

     xout = read_word_2c(0x3b)
     yout = read_word_2c(0x3d)
     zout = read_word_2c(0x3f)

     beschleunigung_xout_skaliert = xout / 16384.0
     beschleunigung_yout_skaliert = yout / 16384.0
     beschleunigung_zout_skaliert = zout / 16384.0

     angle = get_x_rotation(beschleunigung_xout_skaliert, beschleunigung_yout_skaliert, beschleunigung_zout_skaliert) - 39
     
     ws.write_row(i+1, 0, [angle])
     i=i+1
     signal.signal(signal.SIGINT, signal_handler)

