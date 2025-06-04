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
    Netlist* BlifNetlist;                     // ָ�� Netlist ������ָ�룬���ڴ洢��������߼��ź�������Ϣ
    istream& BlifInformation;                    // ����һ�������������ڴ� BLIF �ļ��ж�ȡ����
    unordered_map<string, Gate*>BlifGates;
    string Line;                          // ���ڴ洢�� BLIF �ļ��ж�ȡ�ĵ�ǰ��
    vector<string> words;                  // ���ڴ洢�ӵ�ǰ���н������ĵ���
    unsigned LineNumber;                  // ���ڸ��ٵ�ǰ���ڴ�����к�

    BlifReader(istream& B_I) : BlifNetlist(nullptr), BlifInformation(B_I), LineNumber(0) {

    }
    Netlist* readModel();               // ��Ա���������ڶ�ȡ���������� BLIF ģ��
    void readLine();                    // ���ڶ�ȡ BLIF �ļ�����һ��
};
#endif
