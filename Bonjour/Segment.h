#pragma once
#include <Windows.h>
#include "String.h"
#include "PrefixSEG.h"

class Segment {
public:
	String SegmentStr;

	String GetString(PrefixSEG SEG)
	{
		if (SEG.IsExist == false)
		{
			SegmentStr = "";
		}
		else
		{
			switch (SEG.GetValue())
			{
				case 0x26:this->SegmentStr = "es: "; break;
				case 0x36:this->SegmentStr = "ss: "; break;
				case 0x64:this->SegmentStr = "fs: "; break;
				case 0x65:this->SegmentStr = "gs: "; break;
				case 0x3E:this->SegmentStr = "ds: "; break;
				default: SegmentStr = ""; break;
			}
		}

		return this->SegmentStr;
	}
};