#pragma once
#include <map>
#include <vector>

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void DFS(int32 index);

private:
	unordered_map<const char*, int32>	name_to_id_;
	unordered_map<int32, const char*>	id_to_name_;

	map<int32, set<int32>>				lock_history_;

	Mutex lock_;

private:
	vector<int32>	discovered_order_;
	int32			discovered_count_ = 0;
	vector<bool>	finished_DFS_;
	vector<int32>	parent_;
};