#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "tinyxml2.h"


using namespace std;

void writeBoolVectorToBinary(const std::vector<bool>& boolVec, const std::string& filename = "program.bin") {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile.is_open()) throw std::runtime_error("Error writing binary");
    size_t size = boolVec.size();
    outputFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!boolVec.empty()) {
        std::vector<char> bytes((boolVec.size() + 7) / 8, 0);
        for (size_t i = 0; i < boolVec.size(); ++i) {
            if (boolVec[i]) bytes[i / 8] |= (1 << (i % 8));
        }
        outputFile.write(bytes.data(), bytes.size());
    }
    outputFile.close();
}

std::vector<bool> readBoolVectorFromBinary(const std::string& filename = "program.bin") {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile.is_open()) throw std::runtime_error("Error reading binary");
    size_t size;
    inputFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::vector<bool> boolVec(size);
    if (size > 0) {
        std::vector<char> bytes((size + 7) / 8);
        inputFile.read(bytes.data(), bytes.size());
        for (size_t i = 0; i < size; ++i) {
            boolVec[i] = (bytes[i / 8] & (1 << (i % 8))) != 0;
        }
    }
    inputFile.close();
    return boolVec;
}

std::vector<bool> intToBoolVector(int n, unsigned long long expected) {
    bool isNegative = false;
    std::vector<bool> boolVec;
    if (n == 0) {
        for(int i = 0; i < expected; i++) boolVec.push_back(false);
        return boolVec;
    }
    if (n < 0) {
        isNegative = true;
        n *= -1;
    }
    while (n > 0) {
        boolVec.push_back(n % 2);
        n /= 2;
    }
    while(boolVec.size() < expected-1){
        boolVec.push_back(false);
    }
    boolVec.push_back(isNegative);
    std::reverse(boolVec.begin(), boolVec.end());
    return boolVec;
}

int boolVectorToInt(std::vector<bool>& boolVec){
    int num = 0;
    bool isNegative = boolVec[0];
    boolVec[0] = false;
    std::reverse(boolVec.begin(), boolVec.end());
    for(unsigned long long i = 0; i < boolVec.size(); i++) if(boolVec[i]) num += pow(2, i);
    if (isNegative) return num * -1;
    else return num;
}

char* intToCharArray(string s, int num) {
    std::string str = std::to_string(num);
    str = s + str;
    char* char_array = new char[str.length() + 1];
    strcpy(char_array, str.c_str());
    return char_array;
}

/*
commands:
var <data> <registry>				// writing data[11] to registry[5]
get <memory> <registry>			    // writing data from memory[24] to registry[5]
put <registry> <memory>				// writing data from registry[5] to memory[24]
abs <registry> <memory>				// writing data from registry[5] to memory[24] with abs()

var = 00001 [5]+[16]=[21]
get = 00010 [5]+[29]=[34]
put = 00100 [5]+[29]=[34]
abs = 01000 [5]+[29]=[34]
<data> = [+/-][0-1023]
<memory> = [0-16777215]
<registry> = [0-31]
*/
vector<bool> varCode = { 0,0,0,0,1 };
vector<bool> getCode = { 0,0,0,1,0 };
vector<bool> putCode = { 0,0,1,0,0 };
vector<bool> absCode = { 0,1,0,0,0 };

void Compiler(string Filename){
    

    vector<bool> mainCode, temp1, temp2;
	string command;
	int param1, param2;
	ifstream input(Filename);
	while (!input.eof()) {
		input >> command >> param1 >> param2;
        if (command == "var"){
	        mainCode.insert(mainCode.end(), varCode.begin(), varCode.end());
            temp1 = intToBoolVector(param1, 11);
            temp2 = intToBoolVector(param2, 5);
        }
        else if (command == "get"){
	        mainCode.insert(mainCode.end(), getCode.begin(), getCode.end());
            temp1 = intToBoolVector(param1, 24);
            temp2 = intToBoolVector(param2, 5);
        }
        else if (command == "put"){
	        mainCode.insert(mainCode.end(), putCode.begin(), putCode.end());
            temp1 = intToBoolVector(param1, 5);
            temp2 = intToBoolVector(param2, 24);
        }
        else if (command == "abs"){
	        mainCode.insert(mainCode.end(), absCode.begin(), absCode.end());
            temp1 = intToBoolVector(param1, 5);
            temp2 = intToBoolVector(param2, 24);
        }
        mainCode.insert(mainCode.end(), temp1.begin(), temp1.end());
        mainCode.insert(mainCode.end(), temp2.begin(), temp2.end());
        cout << command << " ";
        for (auto i : temp1) cout << i;
        cout << " ";
        for (auto i : temp2) cout << i;
        cout << endl;
	}
    writeBoolVectorToBinary(mainCode);
    input.close();
}

void Executor(){
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLDeclaration* declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration);

    tinyxml2::XMLElement* root = doc.NewElement("log");
    doc.InsertEndChild(root);

    int par1, par2;
	vector<int> registry(32);
	vector<int> memory(20);
    vector<bool> code = readBoolVectorFromBinary(), temp;

    vector<bool>::iterator iter = code.begin();
    do{
        for(int i = 0; i < 5; i++){
            temp.push_back(*iter);
            ++iter;
        }
        if(temp == varCode){
            temp.clear();
            for (int i = 0; i < 11; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par1 = boolVectorToInt(temp);
            temp.clear();
            for (int i = 0; i < 5; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par2 = boolVectorToInt(temp);
            temp.clear();
            registry[par2] = par1;
            tinyxml2::XMLElement* child = doc.NewElement("var");
            child->SetAttribute(intToCharArray("register_",par2), par1);
            child->SetText("Data recorded to register");
            root->InsertEndChild(child);
        }
        else if(temp == getCode){
            temp.clear();
            for (int i = 0; i < 24; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par1 = boolVectorToInt(temp);
            temp.clear();
            for (int i = 0; i < 5; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par2 = boolVectorToInt(temp);
            temp.clear();
            registry[par2] = memory[par1];
            tinyxml2::XMLElement* child = doc.NewElement("get");
            child->SetAttribute(intToCharArray("register_", par2), memory[par1]);
            child->SetText("From memory to register");
            root->InsertEndChild(child);
        }
        else if (temp == putCode) {
            temp.clear();
            for (int i = 0; i < 5; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par1 = boolVectorToInt(temp);
            temp.clear();
            for (int i = 0; i < 24; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par2 = boolVectorToInt(temp);
            temp.clear();
            memory[par2] = registry[par1];
            tinyxml2::XMLElement* child = doc.NewElement("put");
            child->SetAttribute(intToCharArray("memory_cell_", par2), registry[par1]);
            child->SetText("Recording data to memory");
            root->InsertEndChild(child);
        }
        else if (temp == absCode) {
            temp.clear();
            for (int i = 0; i < 5; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par1 = boolVectorToInt(temp);
            temp.clear();
            for (int i = 0; i < 24; i++) {
                temp.push_back(*iter);
                ++iter;
            }
            par2 = boolVectorToInt(temp);
            temp.clear();
            memory[par2] = abs(registry[par1]);
            tinyxml2::XMLElement* child = doc.NewElement("abs");
            child->SetAttribute(intToCharArray("memory_cell_", par2), abs(registry[par1]));
            child->SetText("Applying abs function and recording");
            root->InsertEndChild(child);
        }
        temp.clear();
    } while (iter != code.end());
    doc.SaveFile("log.xml");

    tinyxml2::XMLDocument doc2;
    tinyxml2::XMLDeclaration* declaration2 = doc2.NewDeclaration();
    doc2.InsertFirstChild(declaration2);

    tinyxml2::XMLElement* root2 = doc2.NewElement("result");
    doc2.InsertEndChild(root2);

    for(int i = 0; i < memory.size(); i++){
        tinyxml2::XMLElement* child = doc2.NewElement("memory");
        child->SetAttribute(intToCharArray("memory_cell_", i), memory[i]);
        child->SetText("Current data");
        root2->InsertEndChild(child);
    }
    doc2.SaveFile("result.xml");
}

int main(int argc = 0, char* argv[] = nullptr) {
    Compiler(argv[1]);
    Executor();
    return 0;
}

