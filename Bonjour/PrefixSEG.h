#pragma once
#include <Windows.h>

/*
	¶ÎÇ°×º
*/
class PrefixSEG {
private:
	BYTE SEG = 0;
public:
	bool IsExist = false;

	BYTE GetValue()
	{
		return this->SEG;
	}

	void SetValue(BYTE SEG)
	{
		IsExist = true;
		this->SEG = SEG;
		return;
	}

};