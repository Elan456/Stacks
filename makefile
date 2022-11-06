compile:
	g++ main.cpp -g -o stacks
erun: # Runs easy, every number of cycles has to be specified
	./stacks ui # uses user input
cun: compile erun
usets:
	./stacks ts  # use ts file for stack input
mt:
	./stacks thing
sum:
	./stacks Sum -n
