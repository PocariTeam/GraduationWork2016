#pragma once
#ifndef Function_h__
#define Function_h__


template<typename T>
static void Safe_Release( T& Obj )
{
	if( Obj )
	{
		Obj->Release();
		Obj = nullptr;
	}
}

template<typename T>
static void Safe_Delete( T& Obj )
{
	if( Obj )
	{
		delete Obj;
		Obj = nullptr;
	}
}

template<typename T>
static void Safe_Delete_Array( T& Obj )
{
	if( Obj )
	{
		delete[] Obj;
		Obj = nullptr;
	}
}

#endif // Function_h__