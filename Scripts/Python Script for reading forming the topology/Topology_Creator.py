# This script can be used to create the topology files necessary for
# implementing various network topologies for ndnSIM

# Warning - This script creates cache servers in a binary tree architecture

# The total no of clients for this topology
total_no_clients = 3000

# The total no of cache servers for this topology
# Number entered should be a two to the power of number of levels, in order to form a tree
# for example for a tree of 3 levels, enter 8 (2^3)
total_no_of_cache_servers = 256

# This method finds out if the argument is a power of two
# and if it is, which power of two it is
def pow_of_two(number):
	if (number)%2 ==0:
		power = 0
		done = False	
		while not done:
			if 2**power == number:
				done = True
			elif 2**power > number:
				power = float("inf")
				done = True		
			else:
				power += 1
	else:	
		power = float("inf")
	return power

# This method creates links between clients and caches and between caches themselves
# (to form the tree of cache servers)
def write_links(name_of_file):
	global total_no_clients
	global total_no_of_cache_servers
	print("Link creation")

	next_cache = 0
	to_number = total_no_of_cache_servers - 1
	count = 0
	power = pow_of_two(total_no_of_cache_servers)
	leaf_nodes = 2**(power-1)
	remainder = int(total_no_clients%leaf_nodes)
	with open(name_of_file, "a") as text_file:
		text_file.write("link" + "\n")
		text_file.write("" + "\n")
		text_file.write("# from" + "\t" + "\t" + "to" + "\t" + "\t" +"capacity" + "\t" + "metric" + "\t" + "delay" + "\t" + "queue" + "\n")
		text_file.write("" + "\n")
# These two loops assign clients to the leaf nodes of the cache server tree
		for link in range(total_no_clients, remainder,-1):
			if next_cache == int(total_no_clients/leaf_nodes):
				next_cache = 0
				to_number -= 1
			if to_number > 99:			
				if link > 9:
					text_file.write("client" + str(link) + "\t" + "cache" + str(to_number) + "\t" + "10Mbps" + "\t" + "\t" +"1" + "\t" + "10ms" + "\t" + "10"+ "\n")
				else:
					text_file.write("client" + str(link) + "\t" +"\t" + "cache" + str(to_number) + "\t" + "10Mbps" + "\t" + "\t" +"1" + "\t" + "10ms" + "\t" + "10"+ "\n")
			else:
				text_file.write("client" + str(link) + "\t" + "cache" + str(to_number) + "\t" + "\t" +"10Mbps" + "\t" + "\t" +"1" + "\t" + "10ms" + "\t" + "10"+ "\n")
			next_cache += 1

		for link in range(remainder, 0, -1):
			if link > 9:
				text_file.write("client" + str(link) + "\t" + "cache" + str(to_number) + "\t" + "\t" +"10Mbps" + "\t" + "\t" +"1" + "\t" + "10ms" + "\t" + "10"+ "\n")
			else:
				text_file.write("client" + str(link) + "\t" + "\t" + "cache" + str(to_number) + "\t" + "\t" +"10Mbps" + "\t" + "\t" +"1" + "\t" + "10ms" + "\t" + "10"+ "\n")

# This loop connects cache servers to each other in order to form a binary tree with the origin server at the top			
		to_number -= 1
		for cache in range(total_no_of_cache_servers-1, 1, -1):
			if cache < 100:
				text_file.write("cache" + str(cache) + "\t" + "\t" + "cache" + str(to_number) + "\t" + "\t" + "10Mbps" + "\t" + "\t" + "1" + "\t" + "10ms" + "\t" + "10" + "\n")
			else:
				text_file.write("cache" + str(cache) + "\t" + "cache" + str(to_number) + "\t" + "\t" + "10Mbps" + "\t" + "\t" + "1" + "\t" + "10ms" + "\t" + "10" + "\n")
			count += 1
			if int(count%2) == 0:
				count = 0
				to_number -= 1
		text_file.write("cache1" + "\t" + "\t" + "originServer" + "\t" + "10Mbps" + "\t" + "\t" + "1" + "\t" + "10ms" + "\t" + "10" + "\n")

	return None

# This method creates positions for cache servers
def write_cacheServer(name_of_file):
	global total_no_of_cache_servers
	print("Cache Server creation")
	x = 0
	y = 0
	with open(name_of_file, "a") as text_file:
		for cache in range(total_no_of_cache_servers-1, 0, -1):		
			x += 1			
			if cache % 4 == 0:
				y += 1
			if cache < 100:			
				text_file.write("cache" + str(cache) + "\t" + "\t" +str(y) + "\t" + str(x%4) + "\n")
			else:
				text_file.write("cache" + str(cache)  + "\t" +str(y) + "\t" + str(x%4) + "\n")
		text_file.write("originServer" + "\t" + "0" + "\t" + "0" + "\n")
		text_file.write(" " + "\n")
	return None

# This method re-assigns values
def assign(value):
	return (int(value/2))

# This method creates positions for the clients
def write_client(name_of_file):
	global total_no_clients
	print("Client creation")
	reset = False
	with open(name_of_file, "a") as text_file:
		text_file.write("#node" + "\t" +"\t" + "y" +"\t" + "x" + "\n")
		x = assign(total_no_clients)		
		for client in range(total_no_clients, 0, -1):			
			if client > total_no_clients/2:				
				text_file.write("client" + str(client) + "\t" + "120" +"\t" + str(x) + "\n")
				x -= 1
			else :
				if not reset:
					x = assign(total_no_clients)
					reset = True
				if client >9:
					text_file.write("client" + str(client) + "\t" + "120" +"\t" + "-" + str(x) + "\n")
				else:
					text_file.write("client" + str(client) + "\t" + "\t" + "120" +"\t" + "-" +str(x) + "\n")
				x -= 1	
	text_file.close()
	return None

# This method creates the topology file and adds basic descriptions
def createFile(name_of_file):
	global total_no_clients
	global total_no_of_cache_servers

	with open(name_of_file, "w") as text_file:
		text_file.write("#" + "\t" + str(name_of_file) + "\n")
		text_file.write("#" + "\t" + str(total_no_of_cache_servers) + " caches" +"\t" + str(total_no_clients) + " clients" + "\t" + "1 Origin Server" +"\n")
		text_file.write(" " + "\n")		
		text_file.write(" " + "\n")		
		text_file.write("#" + "\t" + "/--------\ " + "\t" + "/--------\ " + "\t" + "/--------------\ " + "\n")
		text_file.write("#" + "\t" + "| client |------| cache1 |------| OriginServer |" + "\n")		
		text_file.write("#" + "\t" + "/--------\ " + "\t" + "/--------\ " + "\t" + "/--------------\ " + "\n")
		text_file.write("#" + "\n")
		text_file.write("" + "\n")		
		text_file.write("router" + "\n")
		text_file.write("" + "\n")		
	text_file.close()

	return None

# The main method
def main():
	global total_no_clients
	global total_no_of_cache_servers

	name_of_file = "topo-project_3000clients_256caches.txt"
	total_no_of_cache_servers=int(input("Enter number of cache servers (power of two) : "))
	total_no_clients = int(input("Please enter number of clients : "))
	name_of_file = str(input("PLease enter the name of the file : "))

# For python version 2
#	total_no_of_cache_servers=int(raw_input("Enter number of cache servers (power of two) : "))
#	total_no_clients = int(raw_input("Please enter number of clients : "))
#	name_of_file = str(raw_input("PLease enter the name of the file : "))

	createFile(name_of_file)
	write_client(name_of_file)
	write_cacheServer(name_of_file)
	write_links(name_of_file)
	return None

main()

