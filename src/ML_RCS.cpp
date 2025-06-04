#include"../include/ML_RCS.h"
#include<map>
extern map<string, int> ComsumingTime;

vector<string> MlFindAllTypeReadyNodes(unordered_map<string, Gate*>& BlifGates, vector<string>& UnscheduledGates) {
	vector<string> ReadyGateIds;
	for (auto GateId : UnscheduledGates) {
		bool flag = true;
		for (auto GateInputId : BlifGates[GateId]->PredecessorNodes)
		{
			if (BlifGates[GateInputId]->IsScheduled == false)
			{
				flag = false;
			}
		}
		if (flag)
		{
			ReadyGateIds.push_back(GateId);
		}
	}
	return ReadyGateIds;
}

vector<string> MlFindKtypeReadyNodes(unordered_map<string, Gate*>& BlifGates, vector<string>& ReadyAllTypeGateIds, string KType) {
	vector<string>ReadyKTypeGateIds;
	for (auto GateId : ReadyAllTypeGateIds) {
		if (BlifGates[GateId]->NodeType == KType) {
			ReadyKTypeGateIds.push_back(GateId);
		}
	}
	return ReadyKTypeGateIds;
}

vector<string> MlFindKtypeOngoingStateNodes(unordered_map<string, Gate*>& BlifGates, string KType) {
	vector<string>OngoingKtypeGateIds;
	for (auto GatePair : BlifGates) {
		if (GatePair.second->IsWorking&&GatePair.second->NodeType == KType) {
			OngoingKtypeGateIds.push_back(GatePair.first);
		}
	}
	return OngoingKtypeGateIds;
}

int ML_RCS_Schedule(unordered_map<string, Gate*>& BlifGates, unordered_map<string, int>& Resources) {
	// ��ʼ��l������ʼcycle
	int CurrentLevel = 0;

	// ��ʼ��δ���ȼ���UnscheduleGates
    vector<string> UnscheduledGates;
    for (auto GatePair : BlifGates)
    {
        if (GatePair.second->NodeType != "input")
        {
            UnscheduledGates.push_back(GatePair.first);
        }
        else
        {
            GatePair.second->IsScheduled = true;
        }
    }
    while (!UnscheduledGates.empty()) {

		// ���½ڵ㹤��ʱ��
		for (auto GatePair : BlifGates) {
			if (GatePair.second->IsWorking) {
				GatePair.second->WorkingTimeLength++;
			}
		}

		// ��Դ�ͷ�
		for (auto GatePair : BlifGates)
		{
			if (GatePair.second->NodeType != "input" && GatePair.second->IsWorking &&ComsumingTime[GatePair.second->NodeType] == GatePair.second->WorkingTimeLength)
			{
				GatePair.second->IsWorking = false;
			}
		}

		// "ready_state" V����
		vector<string> ReadyAllTypeGateIds = MlFindAllTypeReadyNodes(BlifGates, UnscheduledGates);


		for (auto operatePair : Resources) {

			// �ڼ���V���ҳ�k���;����Ľڵ㼯�ϣ� U
			vector<string> ReadyKTypeGateIds = MlFindKtypeReadyNodes(BlifGates, ReadyAllTypeGateIds, operatePair.first);

			// �ҳ�k�������ڹ����Ľڵ㼯�ϣ�T
			vector<string> OngoingKtypeGateIds = MlFindKtypeOngoingStateNodes(BlifGates, operatePair.first);

			// ѡ��Sk���е���
			size_t AvailableKtypeResourceNum = Resources[operatePair.first] - OngoingKtypeGateIds.size();
			while (AvailableKtypeResourceNum-- > 0 && !ReadyKTypeGateIds.empty()) {
				auto GateId = ReadyKTypeGateIds.front();
				ReadyKTypeGateIds.erase(ReadyKTypeGateIds.begin());
				BlifGates[GateId]->IsWorking = true;
				BlifGates[GateId]->IsScheduled = true;
				BlifGates[GateId]->level = CurrentLevel;
				for (vector<string>::iterator it = UnscheduledGates.begin();it != UnscheduledGates.end();) {
					if (GateId == *it) {
						UnscheduledGates.erase(it);
						break;
					}
					else {
						it++;
					}
				}
			}
		}
		CurrentLevel++;
    }
	return CurrentLevel - 1;
}
