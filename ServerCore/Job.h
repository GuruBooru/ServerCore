#pragma once
#include <functional>

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : callback_(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* mem_func)(Args...), Args&&... args)
	{
		callback_ = [owner, mem_func, args...]()
		{
			(owner.get()->*mem_func)(args...);
		};
	}

	void Execute()
	{
		callback_();
	}

private:
	CallbackType callback_;
};

