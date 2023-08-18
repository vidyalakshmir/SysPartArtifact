import redis

r = redis.Redis(host='localhost', port=6379, db=0)
for x in range(1,10000):
	r.set(str(x), str(x))
 	print(r.get(str(x)))

