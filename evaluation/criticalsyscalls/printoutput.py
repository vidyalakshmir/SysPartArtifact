import sys 

inpfile = sys.argv[1]
inpf = open(inpfile, "r")
servers = ["bind", "httpd", "lighttpd", "memcached", "nginx", "redis"];
cricsys = ["accept", "accept4", "bind", "chmod", "clone", "connect", "execve", "execveat", "fork", "listen", "mprotect", "ptrace", "recvfrom", "setgid", "setreuid", "setuid", "socket"];
sf = [[0 for x in range(6)] for y in range(17)] #x = no: of servers, y = no: of cric. syscalls
for s in servers:
	sflist = []
	tsplist = []
	splist = []
	inpf.seek(0,0)
	for line in inpf:
		words = line.split()
		if words[1] == s:
			if words[0] == "SF":
				sflist.append(words[2])
			elif words[0] == "SP":
				splist.append(words[2])
			elif words[0] == "TSP":
				tsplist.append(words[2])
	for cs in cricsys:
		if cs in sflist:
			if cs in splist:
				if cs in tsplist: #Not filtered by any
					print(cs + "\t" + s + "\tNONE")
				else:			  #Filtered only by TSP 
					print(cs + "\t" + s + "\tTSP")
			elif cs in tsplist:  #Filtered only by SP
				print(cs + "\t" + s + "\tSP") 
			else: 				#Filtered by SP & TSP, not by SF
				print(cs + "\t" + s + "\tTSP_SP")
		elif cs in splist:
			if cs in tsplist:	#Filtered by all
				print(cs + "\t" + s + "\tSF") #Filtered only by SF
			else:
				print(cs + "\t" + s + "\tTSP_SF") #Filtered by SF & TSP
		else:
			if cs in tsplist:	#Filtered by all
				print(cs + "\t" + s + "\tSF_SP") #Filtered only by SF & SP
			else:
				print(cs + "\t" + s + "\tALL") #Filtered by all