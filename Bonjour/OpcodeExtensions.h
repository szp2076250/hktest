#pragma once
#include "String.h"
#include "MODRM.h"

#define GROUP1 "GROUP1"
#define GROUP1A "GROUP1A"
#define GROUP2 "GROUP2"
#define GROUP3 "GROUP3"
#define GROUP4 "GROUP4"
#define GROUP5 "GROUP5"
#define GROUP6 "GROUP6"
#define GROUP7 "GROUP7"
#define GROUP8 "GROUP8"
#define GROUP9 "GROUP9"
#define GROUP10 "GROUP10"
#define GROUP11 "GROUP11"
#define GROUP12 "GROUP12"
#define GROUP13 "GROUP13"
#define GROUP14 "GROUP14"
#define GROUP15 "GROUP15"
#define GROUP16 "GROUP16"
#define GROUP17 "GROUP17"

/*
	Table A-6. Opcode Extensions for One- and Two-byte Opcodes by Group Number *
*/
class OpcodeExtensions {

public:
	static bool IsOpcodeExtensions(String str)
	{
		if (str.Equals(GROUP1)
			|| str.Equals(GROUP1A)
			|| str.Equals(GROUP2)
			|| str.Equals(GROUP3)
			|| str.Equals(GROUP4)
			|| str.Equals(GROUP5)
			|| str.Equals(GROUP6)
			|| str.Equals(GROUP7)
			|| str.Equals(GROUP8)
			|| str.Equals(GROUP9)
			|| str.Equals(GROUP10)
			|| str.Equals(GROUP11)
			|| str.Equals(GROUP12)
			|| str.Equals(GROUP13)
			|| str.Equals(GROUP14)
			|| str.Equals(GROUP15)
			|| str.Equals(GROUP16)
			|| str.Equals(GROUP17)) {
			return true;
		}
		else
		{
			return false;
		}
	}

	//************************************
	// Method:     Map_TableA6
	// Description:查GROUP表，映射Opcode和operand
	//				Table A-6. Opcode Extensions for One- and Two-byte Opcodes by Group Number *
	// Parameter:  String GroupName - 
	// Parameter:  MODRM ModRM - 
	// Returns:    String - 
	//************************************
	static void Map_TableA6(String GroupName, MODRM ModRM, PBYTE pOpcode, OUT String& superscript, OUT String& opcode, OUT String& operand01, OUT String& operand02)
	{
		if (GroupName.Equals(GROUP1))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "ADD";	break;
				case 1:	opcode = "OR";	break;
				case 2:	opcode = "ADC";	break;
				case 3:	opcode = "SBB";	break;
				case 4:	opcode = "AND";	break;
				case 5:	opcode = "SUB";	break;
				case 6:	opcode = "XOR";	break;
				case 7:	opcode = "CMP";	break;
				default:MessageBox(0, TEXT("OpcodeExtensions发生错误"), 0, 0);
			}
		}
		else if (GroupName.Equals(GROUP1A))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:opcode = "POP"; break;
				default:opcode = "[非法操作数]"; break;
			}
		}
		else if (GroupName.Equals(GROUP2))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "ROL";			break;
				case 1:	opcode = "ROR";			break;
				case 2:	opcode = "RCL";			break;
				case 3:	opcode = "RCR";			break;
				case 4:	opcode = "SHL/SAL";		break;
				case 5:	opcode = "SHR";			break;
				case 7:	opcode = "SAR";			break;
				default:opcode = "[非法操作数]"; break;
			}
		}
		else if (GroupName.Equals(GROUP3))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:
				{
					opcode = "TEST";
					//针对【GROUP3】的'TEST'专门映射：2号operand = Ib或Iz
					//当opcode=F6H && Mod.Reg_Opcode=000B时 >>>> TEST Eb , Ib
					//当opcode=F7H && Mod.Reg_Opcode=000B时 >>>> TEST Ev , Iz
					if (*pOpcode == 0xF6) { operand02 = "Ib"; }
					else if (*pOpcode == 0xF7) { operand02 = "Iz"; }
					break;
				}
				case 2:	opcode = "NOT";			break;
				case 3:	opcode = "NEG";			break;
				case 4:	opcode = "MUL";			break;
				case 5:	opcode = "IMUL";		break;
				case 6:	opcode = "DIV";			break;
				case 7:	opcode = "IDIV";		break;
				default:opcode = "[非法操作数]"; break;
			}
		}
		else if (GroupName.Equals(GROUP4))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "INC";	operand01 = "Eb"; break;
				case 1:	opcode = "DEC";	operand01 = "Eb"; break;
				default:opcode = "[非法操作数]";	break;
			}
		}
		else if (GroupName.Equals(GROUP5))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "INC";	operand01 = "Ev"; break;
				case 1:	opcode = "DEC";	operand01 = "Ev"; break;
				case 2:	opcode = "CALL";operand01 = "Ev"; superscript = "f64"; break;		//near CALL Ev (f64)
				case 3:	opcode = "CALL";operand01 = "Ep"; break;							//far CALL Ep 
				case 4:	opcode = "JMP";	operand01 = "Ev"; superscript = "f64";  break;		//near JMP Ev (f64)
				case 5:	opcode = "JMP";	operand01 = "Mp"; break;							//far JMP Mp 
				case 6:	opcode = "PUSH"; operand01 = "Ev"; superscript = "d64"; break;		//PUSH Ev (d64)
				default:opcode = "[非法操作数]";	break;
			}
		}


		else if (GroupName.Equals(GROUP6))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "SLDT";	operand01 = "Rv/Mw"; break;
				case 1:	opcode = "STR";		operand01 = "Rv/Mw"; break;
				case 2:	opcode = "LLDT";	operand01 = "Ew";	break;
				case 3:	opcode = "LTR";		operand01 = "Ew";	break;
				case 4:	opcode = "VERR";	operand01 = "Ew";	break;
				case 5:	opcode = "VERW";	operand01 = "Ew";	break;
				default:opcode = "[非法操作数]";					break;
			}
		}
		else if (GroupName.Equals(GROUP7))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "SGDT";	operand01 = "Ms";		break;
				case 1:	opcode = "SIDT";	operand01 = "Ms";		break;
				case 2:	opcode = "LGDT";	operand01 = "Ms";		break;
				case 3:	opcode = "LIDT";	operand01 = "Ms";		break;
				case 4:	opcode = "SMSW";	operand01 = "Rv/Mw";	break;
				case 6:	opcode = "LMSW";	operand01 = "Ew";		break;
				case 7:	opcode = "INVLPG";	operand01 = "Mb";		break;
				default:opcode = "[非法操作数]";						break;
			}
		}
		else if (GroupName.Equals(GROUP11))
		{
			switch (ModRM.RegOpcode)
			{
				case 0:	opcode = "MOV";			break;
				default:opcode = "[非法操作数]"; break;
			}
		}
		else
		{
			MessageBox(0, TEXT("OpcodeExtensions发生错误"), 0, 0);
			opcode = "OpcodeExtensions发生错误";
		}

	}




};