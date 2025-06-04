#ifndef _MR_LCS_H
#define _MR_LCS_H
#include"Blif.h"
#include<map>

// MR_LCS（最小资源列表调度）算法
unordered_map<string, int> MR_LCS_Schedule(unordered_map<string, Gate*>& BlifGates, int DeadlineCycle);

#endif