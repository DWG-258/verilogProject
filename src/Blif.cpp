#include"../include/Blif.h"
#include <map>

Netlist* ReadBlif(istream& B_I)
{
    BlifReader r(B_I);
    if (B_I.eof())
    {
        throw invalid_argument("no model found");
    }
    Netlist* netlist = r.readModel();
    netlist->NetlistGates = r.BlifGates;
    return netlist;
}

Netlist* BlifReader::readModel()
{
    readLine();
    if (words.empty() || words[0] != ".model")
        cout << LineNumber << ",expecting .model, found'" << Line << endl;
    if (words.size() != 2)
        cout << LineNumber << ",too many words after .model" << endl;
    ;
    BlifNetlist = new Netlist(words[1]); // �������������������
    if (BlifInformation.eof())
        cout << LineNumber << ",empty model" << endl;
    ;
    readLine();
    do
    {
        if (words.empty())
            cout << LineNumber << ",empty line?";
        // ��ȡ����ڵ�
        if (words[0] == ".inputs")
        {
            for (vector<string>::const_iterator i = (words.begin()) + 1;
                i != words.end(); i++)
            {
                BlifNetlist->BlifInputs.push_back(*i);
            }
            readLine();
            continue;
        }
        // ��ȡ����ڵ�
        else if (words[0] == ".outputs")
        {
            for (vector<string>::const_iterator i = (words.begin()) + 1;
                i != words.end(); i++)
            {
                BlifNetlist->BlifOutputs.push_back(*i);
            }
            readLine();
            continue;
        }
        else if (words[0] == ".names")
        {
            vector<string> CurPredecessorNodes;
            for (unsigned int i = 1; i < words.size() - 1; i++)
            {
                CurPredecessorNodes.push_back(words[i]);
            }
            string CurNodeIdoutput = words[words.size() - 1];
            int CurNodeLineNum = 0;
            readLine();
            string OnlyOneWord;
            OnlyOneWord.clear();
            while (!(BlifInformation.eof() || words.empty() || words[0][0] == '.'))
            {
                OnlyOneWord = words[0];
                CurNodeLineNum++;
                readLine();
            }
            string NodeType = "or";
            switch (CurNodeLineNum)
            {
            case 1:
            {
                int num = 0;
                for (char ch : OnlyOneWord)
                {
                    if (ch != '-')
                        num++;
                }
                if (num == 1)
                {
                    NodeType = "not";
                }
                else
                {
                    NodeType = "and";
                }
                if (BlifGates.find(CurNodeIdoutput) == BlifGates.end())
                {
                    Gate* NodeOutput = new Gate(CurNodeIdoutput, NodeType);
                    BlifGates[CurNodeIdoutput] = NodeOutput;
                }
                for (string NodeId : CurPredecessorNodes)
                {
                    if (BlifGates.find(NodeId) == BlifGates.end())
                    {
                        Gate* NodeInput = new Gate(NodeId, "input");
                        BlifGates[NodeId] = NodeInput;
                    }
                    BlifGates[NodeId]->NodeIdOutputs.push_back(CurNodeIdoutput);
                    BlifGates[CurNodeIdoutput]->PredecessorNodes.push_back(NodeId);
                }
                break;
            }
            default:
            {
                if (BlifGates.find(CurNodeIdoutput) == BlifGates.end())
                {
                    Gate* NodeOutput = new Gate(CurNodeIdoutput, NodeType);
                    BlifGates[CurNodeIdoutput] = NodeOutput;
                }
                for (string NodeId : CurPredecessorNodes)
                {
                    if (BlifGates.find(NodeId) == BlifGates.end())
                    {
                        Gate* NodeInput = new Gate(NodeId, "input");
                        BlifGates[NodeId] = NodeInput;
                    }
                    BlifGates[NodeId]->NodeIdOutputs.push_back(CurNodeIdoutput);
                    BlifGates[CurNodeIdoutput]->PredecessorNodes.push_back(NodeId);
                }
                break;
            }
            }
        }
    } while (!BlifInformation.eof());
    return BlifNetlist;
}

void BlifReader::readLine()
{
    Line.resize(0);
    getline(BlifInformation, Line);
    words.resize(0);
    string::const_iterator i = Line.begin();
    string::const_iterator lastWordStart;
    do
    {
        while (i != Line.end() && *i == ' ')
            i++;
        lastWordStart = i;
        while (i != Line.end() && *i != ' ')
            i++;
        if (i != lastWordStart)
            words.push_back(string(lastWordStart, i));
    } while (i != Line.end());
}


void PrintSchedule(Netlist* netlist)
{
    map<int, vector<string>> Levels;
    for (auto GatePair : netlist->NetlistGates)
    {
        Levels[GatePair.second->level].push_back(GatePair.first);
    }

    // ����ڵ��ӡ
    cout << "Input " << ":";
    for (int i = 0;i < netlist->BlifInputs.size();i++)
    {
        if (i != netlist->BlifInputs.size() - 1) {
            cout << netlist->BlifInputs[i] << ", ";
        }
        else {
            cout << netlist->BlifInputs[i];
        }

    }

    // ����ڵ��ӡ
    cout << "  Output :" << ":";
    for (int i = 0;i < netlist->BlifOutputs.size();i++)
    {
        if (i != netlist->BlifOutputs.size() - 1) {
            cout << netlist->BlifOutputs[i] << ", ";
        }
        else {
            cout << netlist->BlifOutputs[i];
        }

    }
    cout << endl;

    // ��cycle����ӡ
    cout << "Total " << Levels.rbegin()->first + 1 << " Cycles" << endl;

    for (auto LevelPair : Levels)
    {
        vector<string> AndGates;
        vector<string> OrGates;
        vector<string> NotGates;
        for (auto GateId : LevelPair.second)
        {
            if (netlist->NetlistGates[GateId]->NodeType == "and")
            {
                AndGates.push_back(GateId);
            }
            else if (netlist->NetlistGates[GateId]->NodeType == "or")
            {
                OrGates.push_back(GateId);
            }
            else if (netlist->NetlistGates[GateId]->NodeType == "not")
            {
                NotGates.push_back(GateId);
            }
        }

        if (LevelPair.first != -1) {
            cout << "Cycle " << LevelPair.first << ":";
            cout << "{ ";
            for (auto str : AndGates)
            {
                cout << str << " ";
            }
            cout << "},";
            cout << "{ ";
            for (auto str : OrGates)
            {
                cout << str << " ";
            }
            cout << "},";
            cout << "{ ";
            for (auto str : NotGates)
            {
                cout << str << " ";
            }
            cout << "}";
            cout << endl;
        }
    }
}
