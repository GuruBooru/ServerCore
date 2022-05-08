#pragma once

// 레퍼런스를 참조하고 있는 참조자들을 카운팅하는 클래스
// 최상위 클래스를 만들고, 해당 클래스를 상속받아 카운트
class RefCountable
{
public:
	RefCountable() : ref_count_(1) {}
	virtual ~RefCountable() {}

	int32 GetRefCount() { return ref_count_; }
	int32 AddRef() { return ++ref_count_; }
	int32 ReleaseRef()
	{
		int32 ref_count = --ref_count_;
		if (ref_count == 0)
			delete this;

		return ref_count;
	}
protected:
	atomic<int32> ref_count_;
};

/*-------------------------
*		SharedPtr
* 래퍼런스 카운팅하는 객체를 들고 있는 포인터
*
-------------------------*/
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }
	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs.ptr_); }
	// 이동
	TSharedPtr(TSharedPtr&& rhs) { ptr_ = rhs.ptr_; rhs.ptr_ = nullptr; }
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T>(rhs.ptr_)); }

	~TSharedPtr() { Release(); }

	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (ptr_ != rhs.ptr_)
		{
			Release();
			Set(rhs.ptr_);
		}

		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		ptr_ = rhs.ptr_;
		rhs.ptr_ = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return ptr_ == rhs.ptr_; }
	bool		operator== (T* ptr) const { return ptr_ == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return ptr_ != rhs.ptr_; }
	bool		operator!=(const T* ptr) const { return ptr_ != ptr; }
	bool		operator<(const TSharedPtr& rhs) const { return ptr_ < rhs.ptr_; }
	bool		operator>(const TSharedPtr& rhs) const { return ptr_ > rhs.ptr_; }
	T* operator*() { return ptr_; }
	const T* operator*() const { return ptr_; }
	operator T* () const { return ptr_; }
	T* operator->() { return ptr_; }
	const T* operator->() const { return ptr_; }

	bool IsNull() { return ptr_ == nullptr; }

private:
	inline void Set(T* ptr)
	{
		ptr_ = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (ptr_ != nullptr)
		{
			ptr_->ReleaseRef();
			ptr_ = nullptr;
		}
	}

private:
	T* ptr_ = nullptr;

};