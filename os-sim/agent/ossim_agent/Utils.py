import struct,string
def dumphexdata(data):
	l = len(data)
	offset = 0
	blocks = l / 16
	rest = l % 16
	pchar = string.letters+string.digits+string.punctuation
	for i in range(0,blocks):
		c = "%08x\t" % offset
		da = ""
		for j in range(0,16):
			(d,) = struct.unpack("B",data[16*i+j])
			cs = "%02x " % d
			if string.find(pchar,chr(d))!=-1:
				da=da+chr(d)
			else:
				da=da+"."
			c = c + cs
		print c+da
		offset = offset + 16
	da = ""
	c = "%08x\t" % offset
	for i in range(0,rest):
		(d,) = struct.unpack("B",data[blocks*16+i])
		cs = "%02x " % d
		if string.find(pchar,chr(d))!=-1:
			da = da + chr (d)
		else:
			da = da +  "." 
		c = c + cs
	c = c+"   "*(16-rest)+da+" "*(16-rest)
	print c
	
