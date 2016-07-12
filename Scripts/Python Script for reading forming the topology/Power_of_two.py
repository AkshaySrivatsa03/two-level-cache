number = 1003
if (number)%2 ==0:
	power = 0
	done = False	
	while not done:
		if 2**power == number:
			print(power)
			done = True
		if 2**power > number:
			print("Not a power of two")
			done = True		
		else:
			power += 1
else:
	print("Not divisible by two")
