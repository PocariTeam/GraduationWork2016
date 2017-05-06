#pragma once

#ifndef Functor_h__
#define Functor_h__

class DeletePair
{
public:
	template <class T> void operator()( T& Pair )
	{
		if( nullptr != Pair.second )
		{
			delete Pair.second;
			Pair.second = nullptr;
		}
	}
};

class ReleaseElement
{
public:
	template <class T> void operator() ( T& Element )
	{
		if( nullptr != Element )
		{
			Element->Release();
			Element = nullptr;
		}
	}
};

class ReleasePair
{
public:
	template <class T> void operator()( T& Pair )
	{
		if( nullptr != Pair.second )
		{
			Pair.second->Release();
			Pair.second = nullptr;
		}
	}
};


#endif // Functor_h__