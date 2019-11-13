#pragma once
#include <Windows.h>
#include "MODRM.h"

class PrefixREX
{
private:
	BYTE REX; //ÎªÁË·½±ã²âÊÔ£¬½«REXµÄÖµ´æÈë
public:
	/*
		REXÇ°×ºµÄ¸ßËÄÎ»¹Ì¶¨ÊÇ0100,ÎÞÒâÒå¡£
		REXÇ°×ºµÄµÍËÄÎ»·Ö±ðÊÇW¡¢R¡¢X¡¢B
	*/
	BYTE W = 0;
	BYTE R = 0;
	BYTE X = 0;
	BYTE B = 0;

	bool IsExist = false;

	//************************************
	// Method:     SetUp 
	// Description:¹¹½¨REXÇ°×º
	// Parameter:  BYTE REX - 
	// Returns:    void - 
	//************************************
	void SetUp(BYTE REX)
	{
		if (REX != 0)
		{
			this->IsExist = true;

			//·ÖÎöREXÇ°×ºµÄW¡¢R¡¢X¡¢BÓò
			this->W = REX >> 3 & 0x1;
			this->R = REX >> 2 & 0x1;
			this->X = REX >> 1 & 0x1;
			this->B = REX & 0x1;

			this->REX = REX;//ÓÃÓÚ²âÊÔ
		}
	}

	//************************************
	// Method:     Modify_ModRM_SIB 
	// Description:¸ù¾ÝREXÇ°×ºÐÞÕýMODRMºÍSIB×Ö¶Î
	// Parameter:  OUT MODRM & ModRM - 
	// Parameter:  OUT SIB & SIB - 
	// Returns:    void - 
	//************************************
	void Modify_ModRM_SIB(OUT MODRM& ModRM, OUT SIB& SIB)
	{
		/*
		*	ÅÐ¶ÏÊÇ·ñÐèÒªÐÞ¸ÄMODRMºÍSIB×Ö¶Î(¸ù¾ÝREXÇ°×º)
		*/

		//Èç¹û²»´æÔÚREXÇ°×º£¬Ôò½áÊø¡£
		if (this->IsExist == false) { return; }

		//Èç¹û²»´æÔÚSIB×Ö¶ÎÇÒmodÓò==11£¬Ôò¶ÔÕÕÍ¼2-4£¨Figure 2-4. Memory Addressing Without an SIB Byte; REX.X Not Used£©
		//Èç¹û²»´æÔÚSIB×Ö¶ÎÇÒmodÓò!=11£¬Ôò¶ÔÕÕÍ¼2-5£¨Figure 2-5. Register-Register Addressing (No Memory Operand); REX.X Not Used£©
		if (ModRM.IsExist == true && SIB.IsExist == false)
		{
			ModRM.RegOpcode += (this->R << 3);
			ModRM.RM += (this->B << 3);
		}
		//Èç¹û´æÔÚSIBÇÒmodÓò!=11£¬Ôò¶ÔÕÕÍ¼2-6(Figure 2-6. Memory Addressing With a SIB Byte)
		else if (ModRM.IsExist == true && SIB.IsExist == false && ModRM.Mod != 11)
		{
			ModRM.RegOpcode += (this->R << 3);
			SIB.Index += (this->X << 3);
			SIB.Base += (this->B << 3);
		}

		//Èç¹ûÊÇ[opcode reg]?£¬Ôò¶ÔÕÕÍ¼2-7(Figure 2-7. Register Operand Coded in Opcode Byte; REX.X & REX.R Not Used)
		//Ä¿Ç°ÈÏÎª£ºREX.R»áÐÞ¸ÄegOpcode£¬¡¾G¡¿ºÍ¡¾R¡¿»áÊÜµ½Ó°Ïì
		//else if (ModRM.IsExist == true && SIB.IsExist == false && ModRM.Mod == 11)
		//{
		//	ModRM.RegOpcode += (this->R << 3);
		//}

		else
		{
			ModRM.RegOpcode += (this->R << 3);	//REX.RËÆºõ×ÜÊÇÒªÐÞ¸ÄMODRM.RegOpcodeÓò
		}
	}


	//************************************
	// Method:     ChooseREG 
	// Description:Í¨¹ýREXÇ°×ºÑ¡ÔñÐÎÈç¡®rAX/r8¡¯µÄ²Ù×÷Êý¾ßÌåÊÇÄÄ¸ö¼Ä´æÆ÷
	// Parameter:  String ¼Ä´æÆ÷ÐòºÅ - 
	// Parameter:  String leftREG - 
	// Parameter:  String rightREG - 
	// Returns:    String - 
	//************************************
	String ChooseREG(String ¼Ä´æÆ÷ÐòºÅ, String leftREG, String rightREG)
	{
		int REG = 0;

		//ºÏ·¨ÐÔ¼ì²é:Ö»ÔÊÐí´«Èë0~7ºÅ¼Ä´æÆ÷
		if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_0ºÅ)) { REG = 0; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_1ºÅ)) { REG = 1; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_2ºÅ)) { REG = 2; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_3ºÅ)) { REG = 3; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_4ºÅ)) { REG = 4; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_5ºÅ)) { REG = 5; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_6ºÅ)) { REG = 6; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_7ºÅ)) { REG = 7; }
		else { MessageBox(0, TEXT("REGÐòºÅ´íÎó£¬Ö»ÔÊÐí´«Èë0~7ºÅ¼Ä´æÆ÷"), 0, 0); return 0; }

		//Èç¹ûÔÚx64ÏÂ´æÔÚREXÇ°×º£¬Ôò¿ÉÄÜ»á¸Ä±äREGÐòºÅ
		if (this->IsExist)
		{
			REG += this->B << 3;
		}

		//¸ù¾ÝREGÐòºÅÓ³Éä¼Ä´æÆ÷Ãû:Èç¹ûÍ¨¹ýREXµÄÐÞÕýºó£¬¼Ä´æÆ÷ÐòºÅÈÔÐ¡ÓÚ7£¬ËµÃ÷ÊÇ×ó±ßµÄ¼Ä´æÆ÷Ãû£»ÀýÈç rAX/r8µÄ×ó±ß¼Ä´æÆ÷¾ÍÊÇrAX
		if (REG <= 7) { return leftREG; }
		else { return rightREG; }
	}

	//************************************
	// Method:     d64 
	// Description:¸ù¾ÝREXÇ°×ºÖØÐÂ¼ÆËã¼Ä´æÆ÷Ãû£¨ÔÚd64ÉÏ±êµÄÇé¿öÏÂ£©
	// Parameter:  int REGÐòºÅ - 
	// Parameter:  PrefixREX Ç°×ºREX - 
	// Parameter:  operandSize - 
	// Returns:    String - 
	//************************************
	String d64(String ¼Ä´æÆ÷ÐòºÅ, int& operandSize)
	{
		int REG = 0;
		//ºÏ·¨ÐÔ¼ì²é:Ö»ÔÊÐí´«Èë0~7ºÅ¼Ä´æÆ÷
		if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_0ºÅ)) { REG = 0; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_1ºÅ)) { REG = 1; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_2ºÅ)) { REG = 2; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_3ºÅ)) { REG = 3; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_4ºÅ)) { REG = 4; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_5ºÅ)) { REG = 5; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_6ºÅ)) { REG = 6; }
		else if (¼Ä´æÆ÷ÐòºÅ.Equals(¼Ä´æÆ÷_7ºÅ)) { REG = 7; }
		else { MessageBox(0, TEXT("REGÐòºÅ´íÎó£¬Ö»ÔÊÐí0~7ºÅ¼Ä´æÆ÷"), 0, 0); return 0; }

#ifdef _WIN64

		//d64: When in 64 - bit mode, instruction defaults to 64 - bit operand size and cannot encode 32 - bit operand size.
		//Èç¹ûÔÚx64ÏÂ£¬d64ÉÏ±ê»á½«operandSizeÊôÐÔ¸ÄÎª64
		operandSize = 64;

		//Èç¹ûÔÚx64ÏÂ´æÔÚREXÇ°×º£¬Ôò¿ÉÄÜ»á¸Ä±äREGÐòºÅ
		if (this->IsExist)
		{
			REG += this->B << 3;
		}

		//¸ù¾ÝREGÐòºÅÓ³Éä¼Ä´æÆ÷Ãû
		switch (REG)
		{
			case 0:return "rAX";
			case 1:return "rCX";
			case 2:return"rDX";
			case 3:return "rBX";
			case 4:return "rSP";
			case 5:return "rBP";
			case 6:return "rSI";
			case 7:return "rDI";
			case 8:return "r8";
			case 9:return "r9";
			case 10:return "r10";
			case 11:return "r11";
			case 12:return "r12";
			case 13:return "r13";
			case 14:return "r14";
			case 15:return "r15";
		}
#else
		//¸ù¾ÝREGÐòºÅÓ³Éä¼Ä´æÆ÷Ãû
		switch (REG)
		{
			case 0:return "eAX";
			case 1:return "eCX";
			case 2:return "eDX";
			case 3:return "eBX";
			case 4:return "eSP";
			case 5:return "eBP";
			case 6:return "eSI";
			case 7:return "eDI";
		}
#endif
	}
};



