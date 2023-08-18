#!/usr/bin/env python3

import requests as req

for x in range(10001,20000):
	req_string = "http://localhost:80/"+ str(x)+".html"
	resp = req.get(req_string)
	print(req_string)
	print(resp.text)
