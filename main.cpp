#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <ctype.h>
#include <map>
using namespace std;

int maxAct = 0;

// 0 - does nothing
// < - activates all below
// > - activates all above
// _ - stop/blocks signal
// / - kills one above
// \ - kills one below
// ^ - copies whatever is below and puts it ontop of the stack
//	Can be modified with a number on its right to send it to a chuck point
// c - creates a chuck point. its right must have a number to modify it
// . - activates what's directly above it on cycle 0 then dies
// = - cannot be killed
// ? - When activated, only sends a signal downward if the tiles on both sides are the same

class Tile{
	public:
		char type;
		int act;
		int loc;
		int point;  // For chuckers in chuck points
		Tile(char t, int a, int l, int p = 0){
			type = t;
			act = a;
			loc = l;
			point = p;

		}

		Tile(){
			type = '0';
			act = 0;
			loc = 0;
			point = 0;
		}
		void show(){
			cout << "type: " << type << " act: " << act << " loc: " << loc << " point: " << point << endl;
		}
		void activator(vector<Tile> &stack, int d, int startloc){
			// d is the direction either +1 for up or -1 for down
			for (int i = startloc + d; i < stack.size() && i >= 0; i+= d){
				if (stack[i].act == 0 && stack[i].type != '0' && stack[i].type != '_' && stack[i].type != '=') {
					stack[i].act = maxAct + 1;
					maxAct += 1;
				}
				if (stack[i].type == '_' or stack[i].type == '}' or stack[i].type == '{') {
					break; // Prevents further activationing
				}
			
			}

		}	
		void resetStackLocs(vector<Tile> &stack){
			for (int i =0; i < stack.size(); i++){
				stack[i].loc = i;
				
			}
		}
		
		// Finds the first chuckpoint from the bottom with the right point
		int findChuckPoint(vector<Tile> &stack, int point){
			for (int i = 0; i < stack.size(); i++){
				if (stack[i].type == 'c' && stack[i].point == point){
					return i;
				}
			}
			return -1; // -1 means no chuck point was found with that point
		}

	
		// Determines what to do with each character when they are activated
		void activate(vector<Tile> &stack){
			int remove = 0; // 0 will not remove, 1, -1 will remove in a direction
			int chuckPointLoc;
			act = 0;  // Deactivates the tile
			
			switch(type){
				// < and > are activators, they activate other stuff
				case('<'):
					activator(stack, -1, loc);
					break;
				case('>'):
					activator(stack, 1, loc);
					break;
				case ('?'):
					if (loc > 0 && loc < stack.size()-1){  // Checks if on edge
						if (stack[loc-1].type == stack[loc+1].type){  // Conditional
							activator(stack, -1, loc);
						}
					
					}
					break;
				case('/'):
					remove = 1;
					break;
				case('\\'):
					remove = -1;
					break;
				case('^'):
					if (loc > 0){
						if (point == 0){ // no chuck point set
							stack.push_back(Tile(stack[loc-1].type, 0, stack.size()));
						} else{ // This chucker wants to send to a chuckpoint
							chuckPointLoc =  findChuckPoint(stack, point);
							if (chuckPointLoc == -1){	
								stack.push_back(Tile(stack[loc-1].type, 0, stack.size()));
							} else {
								//cout << "cpml: " << chuckPointMap[point].loc << endl;
								Tile copyTile = Tile(stack[loc-1].type, 0, chuckPointLoc);
								stack.insert(stack.begin() + chuckPointLoc, copyTile);
							}
							
							
							// send to chuck point
						}
					}
					break;
				case('.'):
					if (loc < stack.size() && stack[loc+1].act == 0){
						stack[loc+1].act = maxAct+1;
						maxAct++;
						stack.erase(stack.begin() + loc);
					}
					break;

				case('{'):
					if (loc > 0 && stack[loc-1].act == 0){
						stack[loc-1].act = maxAct+1;
						maxAct++;
					}
					break;
				case('}'):
					if (loc < stack.size() && stack[loc+1].act == 0){
						stack[loc+1].act = maxAct+1;
						maxAct++;
					}
					break;

				break;
			}
			if (remove != 0){
				if (loc + remove < stack.size() && loc + remove >= 0 && stack[loc+remove].type != '='){
					stack.erase(stack.begin() + loc + remove);
				}
			}
			resetStackLocs(stack);
			
		}
};

string getStack(){
	string inp;
	getline(cin, inp);
	cin.clear();
	return inp;
}
void displayStack(vector<Tile> &stack, int cycle){
	int u;
	string space;
	cout << endl;
	for (int i =0; i < stack.size(); i++){
		u = stack.size() - i - 1; // to invert the stack to the proper orientation
		if (stack[u].loc > 99){
			space = "";
		} else if (stack[u].loc > 9){
			space = " ";
		} else {
			space = "  ";
		}
		cout << stack[u].loc << space << " | " << stack[u].type << " | ";
	        if (stack[u].act != 0){
			cout << "~" << stack[u].act;
		}
		
		cout << "  point-" << stack[u].point;
		cout << endl;
	}
	cout << "    ----- Cycle: "<< cycle << endl;
}

int getSoonestActivation(vector<Tile> &stack){
	int mLoc = 0;
	for (int i = 0; i < stack.size(); i++){
		if (stack[mLoc].act == 0 || (stack[i].act < stack[mLoc].act && stack[i].act != 0)){
			mLoc = stack[i].loc;
		}
	}
	return mLoc;
}

bool runCycle(vector<Tile> &stack){
	int loc = getSoonestActivation(stack);
	if (stack[loc].act != 0){
		stack[getSoonestActivation(stack)].activate(stack);
		return true; // Keep going
	} else {
		cout << "\nSTACK COMPLETE" << endl;
		return false; // Time to stop
	}
}	

int main(int argc, char* argv[]){
	int cycle = 0;
	int again = 1;
	bool go = true;
	int val;
	string tempStack;
	vector<Tile> stack = {};
       	if (argc == 2 && argv[1][0] != 'u'){
		ifstream stackfile (argv[1]); 
		stackfile >> tempStack;
	} else {
		tempStack = getStack();
	}
	for (int i = 0; i<tempStack.length(); i++){
		stack.push_back(Tile(tempStack[i], 0, i)); // Equivalent to python's .append
	}
	for (int i = 0; i<stack.size(); i++){
		if (stack[i].type == '.'){ // activates all the dots and chuck modifiers
			stack[i].activate(stack);
		} else if (isdigit(stack[i].type) && stack[i].type != '0'){
			val = stack[i].type;
			val -= 48; // because 0 is 48 is ascii
			if(stack[i-1].type == '^' || stack[i-1].type == 'c') {
				stack[i-1].point = val; 
				
			}
			stack.erase(stack.begin() + stack[i].loc);
			stack[i].resetStackLocs(stack);
		}
	}
	displayStack(stack, cycle);
	cin >> again;
	while (again > 0 && go){
		again--;
		go = runCycle(stack);
		cycle++;
		displayStack(stack, cycle);
		if (again < 1){
			cin >> again;
		}
	}

	return 0;
}
