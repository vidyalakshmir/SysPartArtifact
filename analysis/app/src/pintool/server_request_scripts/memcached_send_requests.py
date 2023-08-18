#!/usr/bin/env python

from pymemcache.client.base import Client

client = Client('localhost')
for x in range(1,10000):
	client.set(str(x), str(x))
	result = client.get(str(x))
	print(result)
