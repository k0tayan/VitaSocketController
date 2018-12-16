import socket
import struct
from machine import Pin
p = Pin(2, Pin.OUT)
# https://ht-deko.com/arduino/esp-wroom-32.html
p2 = Pin(12, Pin.OUT)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('0.0.0.0', 5000))

SCE_CTRL_UP = 1 << 6
SCE_CTRL_RIGHT = 1 << 5
SCE_CTRL_DOWN = 1 << 4
SCE_CTRL_LEFT = 1 << 3
SCE_CTRL_LTRIGGER = 1 << 2
SCE_CTRL_RTRIGGER = 1 << 1
SCE_CTRL_TRIANGLE =  1 << 6
SCE_CTRL_CIRCLE = 1 << 5
SCE_CTRL_CROSS = 1 << 4
SCE_CTRL_SQUARE = 1 << 3
SCE_CTRL_START = 1 << 2
SCE_CTRL_SELECT = 1 << 1

class Vita:
	def __init__(self):
		self.up = False
		self.right = False
		self.down = False
		self.left = False
		self.circle = False
		self.triangle = False
		self.square = False
		self.cross = False
		self.r = False
		self.l = False
		self.select = False
		self.start = False
		self.rx = 0
		self.ry = 0
		self.lx = 0
		self.ly = 0

	def update(self, buf):
		buf = struct.unpack("7B", buf)
		check_sum = buf[0] & 0x7F
		su = -check_sum
		for d in buf:
			su += d
		su &= 0x7F
		if(check_sum == su):
			self.up = buf[1] & SCE_CTRL_UP
			self.right = buf[1] & SCE_CTRL_RIGHT
			self.down = buf[1] & SCE_CTRL_DOWN
			self.left = buf[1] & SCE_CTRL_LEFT
			self.l = buf[1] & SCE_CTRL_LTRIGGER
			self.r = buf[1] & SCE_CTRL_RTRIGGER
			self.circle = buf[2] & SCE_CTRL_CIRCLE
			self.triangle = buf[2] & SCE_CTRL_TRIANGLE
			self.square = buf[2] & SCE_CTRL_SQUARE
			self.cross = buf[2] & SCE_CTRL_CROSS
			self.select = buf[2] & SCE_CTRL_SELECT
			self.start = buf[2] & SCE_CTRL_START
			self.rx = buf[3]
			self.ry = buf[4]
			self.lx = buf[5]
			self.ly = buf[6]

vita = Vita()

while True:
	data, addr = s.recvfrom(1024)
	vita.update(data)
	p.value(bool(vita.up))
	p2.value(bool(vita.down))