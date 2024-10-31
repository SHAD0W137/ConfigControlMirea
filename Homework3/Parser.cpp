#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
/*
\ CL example
\ Time to create some constants
8800 -> CONST;
20 -> YACONST;
\ Here's a database:
DATABASE = @{
	EXAMPLE = 123;
	ANOTHER = |YACONST|;
	SUBDATA = @{
		YETANOTHEREXAMPLE = 148;
		ANDAGAIN = |CONST|;
	}
}
\ End of file
////////////////////////////////
# CL example
# Time to create some constants
CONST = 8800
YACONST = 20;
# Here's a database:
[DATABASE]
EXAMPLE = 123
ANOTHER = 20
[DATABASE.SUBDATA]
YETANOTHEREXAMPLE = 148
ANDAGAIN = 8800
# End of file
*/

//queue codes: v - variable; d - dictionary; c - comment

struct variable {
	string name;
	int value;
	variable(string n, int v){
		name = n;
		value = v;
	}
};

struct dictionary{
	string name;
	vector<dictionary> dictionaries;
	vector<variable> variables;
	vector<string> comments;
	vector<variable> constants;
};

char What(const char c){
	const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const string numbers = "0123456789";
	if (c == '\\') return 'c';
	else if (c == '@') return 'd';
	else if (c == '>') return '>';
	else if (c == '=') return '=';
	else if (c == '{') return '{';
	else if (c == '|') return '|';
	else if (c == ';') return ';';
	else if (c == '}') return '}';
	else if (c == '\n') return 'e';
	for (const char letter : letters) if (c == letter) return 'l';
	for (const char number : numbers) if (c == number) return 'n';
}

dictionary parse_dictionary(string namepath = "", string content = "", vector<variable> constants = {}) {
	string arg1, arg2, mode = "none";
	int br_open=0, br_closed=0;
	dictionary dict;
	dict.name = namepath;
	string new_content;
	try {
		for (int i = 0; i < content.size(); i++) {
			switch (What(content[i])) {
			case 'c':
				while (true) {
					i++;
					if (What(content[i]) == 'e') {
						dict.comments.push_back(arg1);
						arg1 = "";
						break;
					}
					arg1.push_back(content[i]);
				}
				break;
			case '=':
				mode = "var";
				break;
			case '>':
				mode = "const";
				break;
			case 'l':
				if (mode == "var" || mode == "const") {
					arg2.push_back(content[i]);
				}
				else {
					arg1.push_back(content[i]);
				}
				break;
			case 'n':
				if (mode == "var" || mode == "const") {
					arg2.push_back(content[i]);
				}
				else {
					arg1.push_back(content[i]);
				}
				break;
			case ';':
				if (mode == "var") {
					dict.variables.emplace_back(arg1, stoi(arg2));
				}
				else if (mode == "const") {
					dict.constants.emplace_back(arg2, stoi(arg1));
					constants.emplace_back(arg2, stoi(arg1));
				}
				mode = ""; arg1 = ""; arg2 = ""; break;
			case 'd':
				i++;
				while (true) {
					i++;
					if (What(content[i]) == '{') br_open++;
					if (What(content[i]) == '}') br_closed++;
					if (br_closed > br_open) break;
					new_content.push_back(content[i]);
				}
				dict.dictionaries.push_back(parse_dictionary(dict.name + arg1 + ".", new_content, constants));
				mode = ""; arg1 = ""; arg2 = ""; break;
			case '|':
				string tmp;
				while (true) {
					i++;
					if (content[i] == '|') {
						for (auto cns : dict.constants) {
							if (cns.name == tmp) {
								dict.variables.emplace_back(arg1, cns.value);
								mode = ""; arg1 = ""; arg2 = ""; break;
							}
						}
						for (auto cns : constants) {
							if (cns.name == tmp) {
								dict.variables.emplace_back(arg1, cns.value);
								mode = ""; arg1 = ""; arg2 = ""; break;
							}
						}
						break;
					}
					else tmp.push_back(content[i]);
				}
			}
		}
	}
	catch (int) { cout << endl << "Some shit gone wrong" << endl; }
	return dict;
}

void output_data(dictionary dict){
	if (dict.name.size() > 0) {
		dict.name.pop_back();
		cout << endl << "[" << dict.name << "]" << endl;
	}
	for(auto i: dict.comments) cout << "#" << i << endl;
	for(auto i: dict.constants) cout << i.name << " = " << i.value << endl;
	for(auto i:dict.variables) cout << i.name << " = " << i.value << endl;
	for(auto i:dict.dictionaries){
		output_data(i);
	}
}

void output_file(dictionary dict) {
	ofstream out("output.txt", std::ios::app);
	if (dict.name.size() > 0) {
		dict.name.pop_back();
		out << endl << "[" << dict.name << "]" << endl;
	}
	for (auto i : dict.comments) out << "#" << i << endl;
	for (auto i : dict.constants) out << i.name << " = " << i.value << endl;
	for (auto i : dict.variables) out << i.name << " = " << i.value << endl;
	for (auto i : dict.dictionaries) {
		output_file(i);
	}
	out.close();
}

int main(int argc = 0, char* argv[] = nullptr) {
	char c;
	string content;

	vector<string> comments;
	vector<variable> constants;
	vector<variable> variables;
	vector<dictionary> dictionaries;

	string fname = "input.txt";
	fname = argv[1];
	
	ifstream input(fname);
	while(!input.eof()){
		c = input.get();
		content.push_back(c);
		cout << c;
	}
	input.close();
	ofstream out("output.txt");
	out.close();
	
	output_data(parse_dictionary("", content));
	output_file(parse_dictionary("", content));
}
