#pragma once

// ���۷����� �����ϰ� �ִ� �����ڵ��� ī�����ϴ� Ŭ����
// �ֻ��� Ŭ������ �����, �ش� Ŭ������ ��ӹ޾� ī��Ʈ
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
* ���۷��� ī�����ϴ� ��ü�� ��� �ִ� ������
*
-------------------------*/
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }
	// ����
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs.ptr_); }
	// �̵�
	TSharedPtr(TSharedPtr&& rhs) { ptr_ = rhs.ptr_; rhs.ptr_ = nullptr; }
	// ��� ���� ����
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