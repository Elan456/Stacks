#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <ctype.h>
#include <map>
#include <sstream>
#include <cmath>
#include <ctime>
#include <chrono>
#include <bits/stdc++.h>
#include <algorithm>
using namespace std;

int maxAct = 0;

// 0 - does nothing
// < - activates all below
// > - activates all above
// ( - deactivates all below, can be blocked only by blockers not pokers
// ) - deactivates all above, can be blocked only by blockers not pokers
// { - activates one below, blocks signals from passing through
// } - activates one above, blocks signals from passing through
// _ - stop/blocks signal, and deactivators
// / - kills one above
// \ - kills one below
// '-' - kills whatever is next to the chuckpoint its assigned
// ^ - copies whatever is below and puts it on top of the stack
//	Can be modified with a number on its right to send it to a chuck point
//	If an r is next to the number it will chuck to the right of the chuck point
// + - equivalent to chucking a 0 i.e. (0^)
// . - activates what's directly above it on cycle 0 then dies
// , - when activated, activates whatever is the chuckpoint its assigned to
// = - cannot be killed
// ? - When activated, sends a signal in the direction with the most immediate zeros
// ` - Takes in user input when activated as a string where each char is inserted into the stack numbers will add that number of zeros to the stack. Used for input values if program ran from &
// * - will cause the chuckpoint to be a substack, using * will give the value or number of units in the substack.  
// @ - when activated will output to the stdout the integer value held at its chuckpoint. Outputs to output values if inside file ran using &
// $ - When activated outputs to stdout the integer as a char
// ; - When activated, outputs a newline to stdout
// ~ - Clears a substack
// & - To run another stacks program. * before and after are for input and output e.g. *v1*v2&Mult*output
// Checks if the string input is a positive integer
bool isInt(string input){
	for (int i = 0; i < input.length(); i++){
		if (!isdigit(input[i])){
			return false;
		}
	}
	return true;
}

int runStack(string tempStack, vector<int> &inputs, vector<int> &outputs, bool called, bool askForCycles, bool showStack); // Prototype
string readStackFromFile(string fileName){
	string tempStack;
	ifstream t(fileName);
	stringstream buffer;
	buffer << t.rdbuf();
	tempStack = buffer.str();
	return tempStack;
}

class Tile{
	public:
		char type; // What character it is
		int act; // What cycle it will activated on
		int loc; // Where it is in the stack
		int point;  // For chuckers in chuck points
		bool blocksSignals = false;
		bool chuckToAble = true; // Can stuff be chucked to this tile
		bool activatable = true; // Can this be activated i.e. does it do stuff
		bool killable = true; // If a killer can kill this
		bool ofSubStack = false;
		string fileName;
		bool isMain; // Is this tile being used as part of the primary stack or a called stack?
		Tile(bool m = true, char t = '|', int a = 0, int l = 0, int p = 0){
			isMain = m; // Defaults to true because unit tiles do not care
			type = t;
			act = a;
			loc = l;
			point = p;
			fileName = "";

			if (type == '}' || type == '{' || type == '_'){
				blocksSignals = true;
			}
			if (type == '^' || type == ',' || type == '`' || type == '+' || type == '-' || type == '&'){
				chuckToAble = false;
			}
			if (type == '|' || type == '=' || type == '_' || type == '*'){
				activatable = false;
			}
			if (type == '='){
				killable = false;
			}
			if (type == '*' || type == '@' || type == '~' || type == '$'){
				ofSubStack = true;
			}

			

		}

		
		bool operator == (Tile &t2) {
			return (this == &t2);
		}
		int getStackValue(vector<Tile> &stack){
			return stack.size();
		}


		int findMyself(vector<Tile> &stack){
			int i = 0;
			while (&stack[i] != this){i++;}
			return i;
		}

		void show(){
			cout << "type: " << type << " act: " << act << " loc: " << loc << " point: " << point << endl;
		}
		void activator(vector<Tile> &stack, int d, int startloc){
			// d is the direction either +1 for up or -1 for down
			for (int i = startloc + d; i < stack.size() && i >= 0; i+= d){
				if (stack[i].act == 0 && stack[i].activatable) {
					stack[i].act = maxAct + 1;
					maxAct += 1;
				}
				if (stack[i].blocksSignals) {
					break; // Prevents further activationing
				}
			
			}

		}	
		void deactivator(vector<Tile> &stack, int d, int startloc){
			for (int i = startloc + d; i < stack.size() && i >= 0; i+=d){
				if (stack[i].act > 0){
					stack[i].act = 0;
				}
				if (stack[i].type == '_'){
					break;
				}
			}
		}
				
		
		// Finds the first chuckpoint from the bottom with the right point
		int findChuckPoint(vector<Tile> &stack, int point){
			if (point == 0){
				return -1;
			}
			for (int i = 0; i < stack.size(); i++){
				if (type == ',' && stack[i].type != ',' && stack[i].point == point){
					return i;
				}
				if (stack[i].chuckToAble && stack[i].point == point && i != loc){
					return i;
				}
			}
			return -1; // -1 means no chuck point was found with that point
		}

		int findMostUnitsDirection(vector<Tile> &stack, vector<vector<Tile>> &subStacks){
			int right = 0;
			int left = 0;
			int d = 1;
			if (stack[loc + 1].type == '*'){
				right = getStackValue(subStacks[stack[loc+1].point]);
			} else {
				while (stack[loc + d].type == '|'){
					right++;
					d++;
				}
			}
			// Tallying the left side

			if (stack[loc - 1].type == '*'){
				left = getStackValue(subStacks[stack[loc-1].point]);
			} else {
				d = -1;
				while (stack[loc + d].type == '|'){
					left++;
					d--;
				}
			}
			if (right > left) {
				return 1;
			} else {
				return -1; // Left is the default direction
			}
		}

		void chuck(vector<Tile> &stack, Tile newTile, vector<vector<Tile>> &subStacks){
			int right;  // equal to 1 if chucking to the right of the point
			int chuckPointLoc;
			if (loc > 0){
				if (point == 0){ // no chuck point set
					stack.push_back(newTile);
				} else{ // This chucker wants to send to a chuckpoint
					chuckPointLoc = findChuckPoint(stack, abs(point));
					if (chuckPointLoc == -1){ // Could not find the chuckpoint	
						stack.push_back(newTile);
					} else {
						// If it is negative that chuck to the right of the chuckpoint
						if (point < 0){right = 1;} else {right = 0;} // Sets right value
						if (!stack[chuckPointLoc].ofSubStack) {
							stack.insert(stack.begin() + chuckPointLoc + right, newTile);
						} else {
							subStacks[stack[chuckPointLoc].point].push_back(newTile);
						}

					}			// send to chuck point
				}
			}
	
		}

	
		// Determines what to do with each character when they are activated
		void activate(vector<Tile> &stack,int inloc, vector<vector<Tile>> &subStacks, vector<int> &inputs, vector<int> &outputs){
			int remove = 0; // 0 will not remove, 1, -1 will remove in a direction
			int chuckPointLoc;
			int right;
			int killLoc;
			vector<Tile> subStack;
			string input;
			int iInput;
			act = 0;  // Deactivates the tile
			loc = inloc;
		//	show();
			switch(type){
				// < and > are activators, they activate other stuff
				case('<'):
					activator(stack, -1, loc);
					break;
				case('>'):
					activator(stack, 1, loc);
					break;
				case(')'):
					deactivator(stack, 1, loc);
					break;
				case('('):
					deactivator(stack, -1, loc);
					break;
				case ('?'):
					activator(stack, findMostUnitsDirection(stack, subStacks), loc);
					break;
				case('/'):
					remove = 1;
					break;
				case('\\'):
					remove = -1;
					break;
				case('^'):
					chuck(stack, Tile(isMain, stack[loc-1].type,0,0), subStacks);
					break;
				case('.'):
					if (loc < stack.size() && stack[loc+1].act == 0){
						stack[loc+1].act = maxAct+1;
						maxAct++;
					}
					break;
				
				case(','):
					chuckPointLoc = findChuckPoint(stack, point);
					if (chuckPointLoc != -1 && stack[chuckPointLoc].act == 0){
						stack[chuckPointLoc].act = maxAct+1;
						maxAct++;
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
				case('-'):
					killLoc = 0;
					right = 0;
					chuckPointLoc = findChuckPoint(stack, abs(point));
					if (chuckPointLoc == -1){
						killLoc = stack.size() - 1;
					} else if (stack[chuckPointLoc].ofSubStack) {
						subStacks[stack[chuckPointLoc].point].pop_back();

					 } else {
						if (point < 0){
							// Check if there is actually anything to the right to destroy
							if (chuckPointLoc + 1 < stack.size()){
								killLoc = chuckPointLoc + 1;
							} else {break;} // Cannot kill anything
						} else {
							killLoc = chuckPointLoc - 1;
						}
					}
					if (stack[killLoc].killable && !stack[chuckPointLoc].ofSubStack){
						stack.erase(stack.begin() + killLoc);
					}
					break;
				case('+'):
					chuck(stack, Tile(), subStacks);
					break;
				case ('`'):
					if (isMain){
						cin >> input;	
						if (isInt(input)){
							for (int i = 0; i < stoi(input); i++){
								chuck(stack, Tile(), subStacks);
							}
						} else {
							for (int c = 0; c < input.length(); c++){
								chuck(stack, Tile(input[c]), subStacks);	
							}
						}
					} else {
						iInput = inputs[0];
						inputs.erase(inputs.begin());
						for (int i =0; i < iInput; i++){
							chuck(stack, Tile(), subStacks);
						}

					}
					break;
				case ('@'):
					if (!isMain){
						outputs.push_back(getStackValue(subStacks[point]));
					} else {
						cout << getStackValue(subStacks[point]);
					}
					break;
				case ('$'):
					cout << static_cast<char>(getStackValue(subStacks[point]));
					break;
				case (';'):
					if (isMain){
						cout << endl;
					}
					break;
				case ('~'):
					subStacks[point].clear();
					break;
				case('&'):
					{
						vector<int> useInputs;
						vector<int> useOutputs;
						int i = 1;
						while (stack[loc - i].type == '*'){
							useInputs.insert(useInputs.begin(),
									getStackValue(subStacks[stack[loc-i].point]));
							i++;
						}


						string rawStack = readStackFromFile(fileName);
						runStack(rawStack, useInputs, useOutputs, true, false, false);
						
						i = 1;
						while (stack[loc + i].type == '*'){
							subStacks[stack[loc + i].point].clear(); // Empties
							for (int j =0; j < useOutputs[0]; j++){
								subStacks[stack[loc+i].point].push_back(
										Tile());
							}
							useOutputs.erase(useOutputs.begin());
							i++;
						}



					
					}
					// Find substacks to use for inputs
					// Find substack to be used for output
					// Run the stack in the file
					// Use the inputs properly
					// Set the output stacks properly

				break;
			}
			if (remove != 0){
				if (loc + remove < stack.size() && loc + remove >= 0 && stack[loc+remove].type != '='){
					stack.erase(stack.begin() + loc + remove);
				}
			}
			
		}
};


string getStack(){
	string inp;
	getline(cin, inp);
	cin.clear();
	return inp;
}
void displayStack(vector<Tile> &stack, vector<string> &subStackNames, vector<vector<Tile>> subStacks, int cycle){
	int u;
	int digits;
	cout << endl;
	/*for (int i =0; i < subStackNames.size(); i++){
		cout << subStackNames[i] << "* = "<< subStacks[i].size() << endl;
	}
	*/
	for (int i =0; i < stack.size(); i++){
		u = stack.size() - i - 1; // to invert the stack to the proper orientation
		switch(u){
			case(0): digits = 1; break;
			default: digits = int(log10(u) + 1); break;
		}
		for (int j = 0; j < 3 - digits; j++){
			cout << " ";
		}
		cout << u << "   | " << stack[u].type << " | ";
	       	if (stack[u].act != 0){
			cout << "~" << stack[u].act;
		}

		if (abs(stack[u].point) > 0) {cout << " " << subStackNames[stack[u].point] << "*" << subStacks[stack[u].point].size();}	
			cout << endl;
	}
	cout << "    ----- Cycle: "<< cycle << endl;
}

int getSoonestActivation(vector<Tile> &stack){
	int mLoc = 0;
	for (int i = 0; i < stack.size(); i++){
		if (stack[mLoc].act == 0 || (stack[i].act < stack[mLoc].act && stack[i].act != 0)){
			mLoc = i;
		}
	}
	return mLoc;
}

bool runCycle(vector<Tile> &stack, vector<vector<Tile>> &subStack, vector<int> &inputs, vector<int> &outputs){
	int loc = getSoonestActivation(stack);
	if (stack[loc].act != 0){
		stack[getSoonestActivation(stack)].activate(stack, loc, subStack, inputs, outputs);
		return true; // Keep going
	} else {
		return false; // Time to stop because everything is deactivated
	}
}	

int main(int argc, char* argv[]){
	
	bool askForCycles = false; // Should it ask for cycles or go to completion
	bool showStack = false; // Is the stack shown?
	string tempStack;
	
	if (argc >= 2 && argv[1][0] != 'u'){
		tempStack = readStackFromFile(argv[1]);
	}
	if (argc == 3){
		if (string(argv[2]) == "-s"){ // to show all stuff
			askForCycles = true;
			showStack = true;
		}
	}
	if (argc < 2 || argv[1][0] == 'u') {
		tempStack = getStack();
	}
	vector<int> out;
	vector<int> in;
	runStack(tempStack, in, out, false, askForCycles, showStack);

}
int runStack(string tempStack, vector<int> &inputs, vector<int> &outputs, bool called = false, bool askForCycles = false, bool showStack = false){
	int cycle = 0;
	int again = 1;
	bool go = true;
	int val;
	
	// Creating vectors to keep track of i/o for called stacks

	vector<Tile> stack = {};
	vector<vector<Tile>> subStacks;
	vector<string> subStackNames{"base"}; // Keeps track of all the names for substacks, the index is the actual point


	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
       	// Converting file to stack
	bool inComment = false;
	for (int i = 0; i<tempStack.length(); i++){
		if (tempStack[i] == '\n'){
			inComment = false;
			stack.push_back(Tile(called, '=', 0, i));
			stack.push_back(Tile(called, '_', 0, i));
			stack.push_back(Tile(called, '=', 0, i));
		} else if (tempStack[i] == '#') {
			inComment = true;
		} else if (!inComment && tempStack[i] != ' ' && tempStack[i] != '\t'){
			stack.push_back(Tile(!called, tempStack[i], 0, i)); // Equivalent to python's .append
		}
	}
	for (int i = 0; i<stack.size(); i++){
		if (stack[i].type == '.'){ // activates all the dots
			stack[i].activate(stack, i, subStacks, inputs, outputs);
		}
	}

	string pointStr;
	int j;
	vector<string>::iterator nameLoc;
	for (int i = 0; i<stack.size(); i++){  // Modifies things to have points based on numbers 
		pointStr = "";
		j = 1;
		if (isdigit(stack[i].type) || isalpha(stack[i].type)){ // Found a number, keep going to get the whole number
			while (isdigit(stack[i].type) || isalpha(stack[i].type)) {
				pointStr += stack[i].type;
				i++;
				j++;
			}
			if (stack[i-j].type == '&'){ // The point should be a file
				ifstream file;
				file.open(pointStr);
				if (!file){
					cout << "File \"" << pointStr << "\" not found!" << endl;
				}
				stack[i-j].fileName = pointStr;
			}
			// Checking if no substack of this name exists
			nameLoc = find(subStackNames.begin(), subStackNames.end(), pointStr);
			if (nameLoc == subStackNames.end()){
				stack[i-j].point = subStackNames.size();
				subStackNames.push_back(pointStr); // Adds the new name to the name holder 
			} else {
				stack[i-j].point = nameLoc - subStackNames.begin(); // Gets the index of that name
			}
		}
	}
	 
	for (int i =0; i<stack.size(); i++){ // Removing all the initialization stuff
		if (isdigit(stack[i].type) || isalpha(stack[i].type) || stack[i].type == '!' || stack[i].type == '.') {
			stack.erase(stack.begin() + i);
			i--;
		}
	}


	// Populating the substack vector with one for every unique name found in the code provided
	for (int i =0; i < subStackNames.size(); i++){
		vector<Tile> blank;
		subStacks.push_back(blank);
	}

	if (showStack){displayStack(stack, subStackNames, subStacks, cycle);}

	if (askForCycles){cin >> again;}
	while ((!askForCycles || again > 0) && go){
		again--;
		go = runCycle(stack, subStacks, inputs, outputs);
		cycle++;
		if (showStack){displayStack(stack, subStackNames, subStacks, cycle);}
		if (again < 1 && askForCycles){
			cin >> again;
		}
	}
	
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	//cout << "Run time (ms): " << chrono::duration_cast<chrono::milliseconds>(end-begin).count() << endl;
	return 0;
}
