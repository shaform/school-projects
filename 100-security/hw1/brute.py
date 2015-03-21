import string

source = string.ascii_letters + string.digits

for a in source:
	for b in source:
		for c in source:
			for d in source:
				print a+b+c+d
