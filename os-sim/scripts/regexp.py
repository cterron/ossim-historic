#!/usr/bin/python 
# 2008 DK @ ossim
import sys,re

if sys.argv[3] is None:
        print "Args are filename, regexp and [0|1]"

f = open(sys.argv[1], 'r')
data = f.readlines()
exp=sys.argv[2]

print sys.argv[2]

line_match = 0

matched = 0

for line in data:
	result = re.findall(exp,line)
	try:
		tmp = result[0]
	except IndexError:
		if sys.argv[3] is "1":
			print "Not matched:", line
		continue
	print result
	matched += 1


print "Counted", len(data), "lines."
print "Matched", matched, "lines."
