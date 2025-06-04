#include"../include/MR_LCS.h"
#include <map>
#include <algorithm>
#include <climits>
extern map<string, int> ComsumingTime;

// 寻找所有类型的就绪节点
vector<string> MrFindAllTypeReadyNodes(unordered_map<string, Gate*>& BlifGates, vector<string>& UnscheduledGates) {
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

// 寻找K类型的就绪节点
vector<string> MrFindKtypeReadyNodes(unordered_map<string, Gate*>& BlifGates, vector<string>& ReadyAllTypeGateIds, string KType) {
    vector<string>ReadyKTypeGateIds;
    for (auto GateId : ReadyAllTypeGateIds) {
        if (BlifGates[GateId]->NodeType == KType) {
            ReadyKTypeGateIds.push_back(GateId);
        }
    }
    return ReadyKTypeGateIds;
}

// 寻找K类型的正在工作节点
vector<string> MrFindKtypeOngoingStateNodes(unordered_map<string, Gate*>& BlifGates, string KType, int CurrentLevel) {
    vector<string>OngoingKtypeGateIds;
    for (auto GatePair : BlifGates) {
        if (GatePair.second->IsWorking && GatePair.second->NodeType == KType && 
            GatePair.second->level <= CurrentLevel && 
            GatePair.second->level + ComsumingTime[GatePair.second->NodeType] > CurrentLevel) {
            OngoingKtypeGateIds.push_back(GatePair.first);
        }
    }
    return OngoingKtypeGateIds;
}

// 计算节点的ALAP(As Late As Possible)值
void CalculateALAP(unordered_map<string, Gate*>& BlifGates, int DeadlineCycle) {
    // 初始化所有非输入节点的ALAP值为无穷大
    for (auto& GatePair : BlifGates) {
        if (GatePair.second->NodeType != "input") {
            GatePair.second->ALAP = DeadlineCycle;
        }
        else {
            GatePair.second->ALAP = 0;
        }
    }

    // 从输出节点反向计算ALAP值
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& GatePair : BlifGates) {
            if (GatePair.second->NodeType != "input") {
                for (auto& Successor : GatePair.second->NodeIdOutputs) {
                    if (BlifGates[Successor]->ALAP - ComsumingTime[GatePair.second->NodeType] < GatePair.second->ALAP) {
                        GatePair.second->ALAP = BlifGates[Successor]->ALAP - ComsumingTime[GatePair.second->NodeType];
                        changed = true;
                    }
                }
            }
        }
    }
}

// 计算节点的优先级（基于ALAP值）
bool CompareNodePriority(const pair<string, int>& a, const pair<string, int>& b) {
    return a.second < b.second; // 较小的ALAP值具有更高优先级
}

// MR_LCS算法实现
unordered_map<string, int> MR_LCS_Schedule(unordered_map<string, Gate*>& BlifGates, int DeadlineCycle) {
    // 重置节点状态
    for (auto& GatePair : BlifGates) {
        GatePair.second->IsScheduled = false;
        GatePair.second->IsWorking = false;
        GatePair.second->WorkingTimeLength = 0;
        GatePair.second->level = -1;
    }
    
    // 计算ALAP时间
    CalculateALAP(BlifGates, DeadlineCycle);
    
    // 初始化资源计数器
    unordered_map<string, int> ResourceCounter;
    for (auto& GatePair : BlifGates) {
        if (GatePair.second->NodeType != "input") {
            if (ResourceCounter.find(GatePair.second->NodeType) == ResourceCounter.end()) {
                ResourceCounter[GatePair.second->NodeType] = 0;
            }
        }
    }
    
    // 初始化未调度节点集合
    vector<string> UnscheduledGates;
    for (auto GatePair : BlifGates) {
        if (GatePair.second->NodeType != "input") {
            UnscheduledGates.push_back(GatePair.first);
        }
        else {
            GatePair.second->IsScheduled = true;
        }
    }
    
    // 当前周期
    int CurrentLevel = 0;
    
    while (!UnscheduledGates.empty() && CurrentLevel <= DeadlineCycle) {
        // 更新节点工作时间
        for (auto& GatePair : BlifGates) {
            if (GatePair.second->IsWorking) {
                GatePair.second->WorkingTimeLength++;
            }
        }
        
        // 资源释放
        for (auto& GatePair : BlifGates) {
            if (GatePair.second->NodeType != "input" && GatePair.second->IsWorking && 
                ComsumingTime[GatePair.second->NodeType] == GatePair.second->WorkingTimeLength) {
                GatePair.second->IsWorking = false;
            }
        }
        
        // 找出所有就绪的节点
        vector<string> ReadyAllTypeGateIds = MrFindAllTypeReadyNodes(BlifGates, UnscheduledGates);
        
        // 按节点类型分组
        map<string, vector<string>> ReadyNodesByType;
        for (auto GateId : ReadyAllTypeGateIds) {
            string NodeType = BlifGates[GateId]->NodeType;
            ReadyNodesByType[NodeType].push_back(GateId);
        }
        
        // 对每种类型的节点，按ALAP值排序（优先级排序）
        for (auto& TypeNodes : ReadyNodesByType) {
            vector<pair<string, int>> NodePriorities;
            for (auto& GateId : TypeNodes.second) {
                NodePriorities.push_back({GateId, BlifGates[GateId]->ALAP});
            }
            sort(NodePriorities.begin(), NodePriorities.end(), CompareNodePriority);
            
            // 清空原有列表并按优先级重新填充
            TypeNodes.second.clear();
            for (auto& NodePriority : NodePriorities) {
                TypeNodes.second.push_back(NodePriority.first);
            }
        }
        
        // 为每种类型的操作更新资源使用情况
        for (auto& TypeNodes : ReadyNodesByType) {
            string NodeType = TypeNodes.first;
            vector<string>& ReadyKTypeGateIds = TypeNodes.second;
            
            // 找出当前正在执行的该类型节点
            vector<string> OngoingKtypeGateIds = MrFindKtypeOngoingStateNodes(BlifGates, NodeType, CurrentLevel);
            
            // 更新该类型资源的最大使用数量
            ResourceCounter[NodeType] = max(ResourceCounter[NodeType], (int)OngoingKtypeGateIds.size());
            
            // 调度可以在当前周期启动的节点
            for (auto GateId : ReadyKTypeGateIds) {
                // 检查是否可以在不超过截止日期的情况下调度
                if (CurrentLevel + ComsumingTime[NodeType] <= DeadlineCycle) {
                    BlifGates[GateId]->IsWorking = true;
                    BlifGates[GateId]->IsScheduled = true;
                    BlifGates[GateId]->level = CurrentLevel;
                    
                    // 从未调度列表中移除
                    for (auto it = UnscheduledGates.begin(); it != UnscheduledGates.end(); ) {
                        if (*it == GateId) {
                            UnscheduledGates.erase(it);
                            break;
                        }
                        else {
                            ++it;
                        }
                    }
                    
                    // 更新资源计数
                    OngoingKtypeGateIds.push_back(GateId);
                    ResourceCounter[NodeType] = max(ResourceCounter[NodeType], (int)OngoingKtypeGateIds.size());
                }
            }
        }
        
        CurrentLevel++;
    }
    
    // 如果不能在截止日期内完成所有节点，返回空资源映射
    if (!UnscheduledGates.empty()) {
        return unordered_map<string, int>();
    }
    
    return ResourceCounter;
}