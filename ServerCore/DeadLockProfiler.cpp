#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(lock_);

	int32 lock_id = 0;

	auto target_id = name_to_id_.find(name);

	if (target_id == name_to_id_.end())
	{
		lock_id = static_cast<int32>(name_to_id_.size());
		name_to_id_[name] = lock_id;
		id_to_name_[lock_id] = name;
	}
	else
	{
		lock_id = target_id->second;
	}

	if (l_lock_stack.empty() == false)
	{
		const int32 prev_id = l_lock_stack.top();
		if (lock_id != prev_id)
		{
			set<int32>& history = lock_history_[prev_id];
			if (history.find(lock_id) == history.end())
			{
				history.insert(lock_id);
				CheckCycle();
			}
		}
	}

	l_lock_stack.push(lock_id);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(lock_);

	if (l_lock_stack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lock_id = name_to_id_[name];
	if (l_lock_stack.top() != lock_id)
		CRASH("INVALID_UNLOCK");

	l_lock_stack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lock_count = static_cast<int32>(name_to_id_.size());
	discovered_order_ = vector<int32>(lock_count, -1);
	discovered_count_ = 0;
	finished_DFS_ = vector<bool>(lock_count, false);
	parent_ = vector<int32>(lock_count, -1);

	for (int32 lock_id = 0; lock_id < lock_count; ++lock_id)
		DFS(lock_id);

	discovered_order_.clear();
	finished_DFS_.clear();
	parent_.clear();
}

void DeadLockProfiler::DFS(int32 cur_id)
{
	if (discovered_order_[cur_id] != -1)
		return;

	discovered_order_[cur_id] = discovered_count_++;

	auto find_it = lock_history_.find(cur_id);
	if (find_it == lock_history_.end())
	{
		finished_DFS_[cur_id] = true;
		return;
	}

	set<int32>& next_set = find_it->second;
	for (int32 next : next_set)
	{
		if (discovered_order_[next] == -1)
		{
			parent_[next] = cur_id;
			DFS(next);
			continue;
		}

		if (discovered_order_[cur_id] < discovered_order_[next])
			continue;

		if (finished_DFS_[next] == false)
		{
			printf("%s -> %s\n", id_to_name_[cur_id], id_to_name_[next]);

			int32 err_thread = cur_id;
			while (true)
			{
				printf("%s -> %s\n", id_to_name_[cur_id], id_to_name_[next]);
				err_thread = parent_[err_thread];
				if (err_thread == next)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}

	}
	finished_DFS_[cur_id] = true;
}
