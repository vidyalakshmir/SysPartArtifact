#!/usr/bin/env python3
import os

for x in range(1,100):
	for y in range(1,100):
		req_string = "dig -x " +str(x) + "." + str(y) + "." + str(x) + "." + str(y)
		os.system(req_string)
		print(req_string)
