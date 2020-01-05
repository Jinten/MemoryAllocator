#pragma once

// this class is not exclusivity.
template<class _Arg>
class Delegate
{
public:
	Delegate() {};
	~Delegate() {};

	template<class T>
	static TArgAction<_Arg> bind(void(T::* method)(_Arg), T* instancePtr)
	{
		return std::bind(method, instancePtr, std::placeholders::_1);
	}

	template<class T>
	void addBind(void(T::* method)(_Arg), T* instancePtr)
	{
		mArgDelegateList.push_back(bind(method, instancePtr));
	}

	template<class T>
	void removeBind(void(T::* method)(_Arg), T* instancePtr)
	{
		auto& remove = bind(method, instancePtr);

		mArgDelegateList.remove_if([&](const TArgAction<_Arg>& src) { return src.target<_Arg>() == remove.target<_Arg>(); });
	}

	void operator += (TArgAction<_Arg>& func)
	{
		mArgDelegateList.push_back(func);
	}

	void operator -=(TArgAction<_Arg>& func)
	{
		mArgDelegateList.remove_if([&](const TArgAction<_Arg>& src) { return src.target<_Arg>() == func.target<_Arg>(); });
	}

	void operator += (TArgAction<_Arg>&& func)
	{
		mArgDelegateList.push_back(func);
	}

	void operator -=(TArgAction<_Arg>&& func)
	{
		mArgDelegateList.remove_if([&](const TArgAction<_Arg>& src) { return src.target<_Arg>() == func.target<_Arg>(); });
	}

	void invoke(_Arg arg)
	{
		for(auto& it : mArgDelegateList)
		{
			it(arg);
		}
	}

private:
	std::list<TArgAction<_Arg>> mArgDelegateList;
};


// this class is not exclusivity.
template<>
class Delegate<void>
{
public:
	Delegate() {};
	~Delegate() {};

	template<class T>
	static Action bind(void(T::* method)(), T* instancePtr)
	{
		return std::bind(method, instancePtr);
	}

	template<class T>
	void addBind(void(T::* method)(), T* instancePtr)
	{
		mDelegateList.push_back(bind(method, instancePtr));
	}

	template<class T>
	void removeBind(void(T::* method)(), T* instancePtr)
	{
		auto& remove = bind(method, instancePtr);

		mDelegateList.remove_if([&](const Action& src) { return src.target<void>() == remove.target<void>(); });
	}

	void operator += (Action& func)
	{
		mDelegateList.push_back(func);
	}

	void operator -=(Action& func)
	{
		mDelegateList.remove_if([&](const Action& src) { return src.target<void>() == func.target<void>(); });
	}

	void operator += (Action&& func)
	{
		mDelegateList.push_back(func);
	}

	void operator -=(Action&& func)
	{
		mDelegateList.remove_if([&](const Action& src) { return src.target<void>() == func.target<void>(); });
	}

	void invoke()
	{
		for(auto& it : mDelegateList)
		{
			it();
		}
	}

private:
	std::list<Action>	mDelegateList;
};