compile:
	g++ main.cpp -o s.exe
run:
	./s.exe ui # uses user input
cun: compile run
usets:
	./s.exe ts  # use ts file for stack input
thing:
	./s.exe mult
