#pragma once
#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};

#pragma endregion

#pragma region Length

template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion

#pragma region TypeAt

template<typename T, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt < TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion

#pragma region IndexOf

template<typename TypeList, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};
#pragma endregion

#pragma region Conversion

template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		// 컴파일 특성 상 가장 알맞는 함수를 사용하도록 하기 때문에
		// 변환이 된다면 From이 To로 변환할 수 있다면 Small을 반환하는 함수를,
		// 그렇지 않다면 Big을 반환하는 함수를 사용
		// --> 변환이 가능한지 아닌지를 판단할 수 있음
		//
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion

#pragma region TypeCast

template<int32 v>
struct Int2Type
{
	enum { value = v };
};

template<typename TypeList>
class TypeConversion
{
public:
	enum
	{
		length = Length<TypeList>::value
	};

	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using FromType = typename TypeAt<TypeList, i>::Result;
		using ToType = typename TypeAt<TypeList, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert_table_[i][j] = true;
		else
			s_convert_table_[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>());
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert_table_[from][to];
	}

public:
	static bool s_convert_table_[length][length];
};

template<typename TypeList>
bool TypeConversion<TypeList>::s_convert_table_[length][length];

template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TypeList = typename From::TypeList;

	if (TypeConversion<TypeList>::CanConvert(ptr->type_ID_, IndexOf<TypeList, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TypeList = typename From::TypeList;

	if (TypeConversion<TypeList>::CanConvert(ptr->type_ID_, IndexOf<TypeList, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TypeList = typename From::TypeList;

	return TypeConversion<TypeList>::CanConvert(ptr->type_ID_, IndexOf<TypeList, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TypeList = typename From::TypeList;

	return TypeConversion<TypeList>::CanConvert(ptr->type_ID_, IndexOf<TypeList, remove_pointer_t<To>>::value);
}

#pragma endregion

#define DECLARE_TYPE_LIST(Types)		using TypeList = Types; int32 type_ID_;
#define INIT_TYPE_LIST(Type)			type_ID_ = IndexOf<TypeList, Type>::value;