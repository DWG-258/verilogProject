#ifndef _BLIF_H
#define _BLIF_H
#include<iostream>
#include<memory>
#include<unordered_map>
#include<vector>
#include<string>
using namespace std;

class Netlist;
class Gate;
Netlist* ReadBlif(istream& B_I);
void PrintSchedule(Netlist* netlist);

class Gate {
public:
    Gate(string Name, string NodeType) {
        this->Name = Name;
        this->NodeType = NodeType;
    }

    string Name;
    string NodeType;
    vector<string>PredecessorNodes;
    vector<string>NodeIdOutputs;
    int level = -1;
    bool IsScheduled = false;
    bool IsWorking = false;
    int WorkingTimeLength = 0;
    int ALAP = 0;
};

class Netlist
{
public:
    Netlist(string n) : name(n) {}
    string name;
    unordered_map<string, Gate*>NetlistGates;
    vector<string>BlifInputs;
    vector<string>BlifOutputs;
};

class BlifReader
{
public:
    Netlist* BlifNetlist;                     // 指向 Netlist 类对象的指针，用于存储解析后的逻辑门和连接信息
    istream& BlifInformation;                    // 引用一个输入流，用于从 BLIF 文件中读取数据
    unordered_map<string, Gate*>BlifGates;
    string Line;                          // 用于存储从 BLIF 文件中读取的当前行
    vector<string> words;                  // 用于存储从当前行中解析出的单词
    unsigned LineNumber;                  // 用于跟踪当前正在处理的行号

    BlifReader(istream& B_I) : BlifNetlist(nullptr), BlifInformation(B_I), LineNumber(0) {

    }
    Netlist* readModel();               // 成员函数，用于读取并解析整个 BLIF 模型
    void readLine();                    // 用于读取 BLIF 文件的下一行
};
#endif
