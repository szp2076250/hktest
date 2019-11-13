#pragma once
BYTE TestShellCode[100] =
{

	0x44,0x39,0x1D,0x22,0x77,0x03,0x00,
	0x44,0x39,0x1D,0x00,0x00,0x00,0x00,
	0x44,0x39,0x1D,0x88,0x88,0x88,0x88,

	0x66,0x9A,0x01,0xA1,0x08,0x67,				//���� call far 6708:A101
		 0x9A,0x01,0xA1,0x08,0x67,0x54,0x00 ,	//���� call far 0054:6708A101
	0xEA,0xFF,0xFF,0xFF,0xFF,0x66,0x77,			//���� jmp  far 7766:FFFFFFFF
		 0x39,0x1D,0xFF,0xFF,0xFF,0xFF,			//x86: CMP  dword ptr [FFFFFFFF] , EBX		x64:	cmp	dword ptr [TestShellCode+19h],ebx
	0x67,0x39,0x1D,0xFF,0xFF,0xFF,0xFF,			//x86: CMP  dword ptr [FFFFFFFF] , EBX		x64:	cmp	dword ptr [TestShellCode+20h],ebx
	0x66,0x39,0x1D,0xFF,0xFF,0xFF,0xFF,			//x86: CMP  word ptr [FFFFFFFF] , BX		x64:	cmp	dword ptr [TestShellCode+27h],ebx
	0xC3
};



#ifdef _WIN64

EXTERN_C ULONG64 TestAsm(ULONG64 u1, ULONG64 u2);	//������ຯ��

#else



void __declspec(naked) TestAsm()
{
	__asm
	{
		//*********************��2*************************
		//���� 00~05
		add byte ptr[ebx - 0xB3B7C14], cl
		add dword ptr[esi - 0x3F], ecx
		add ah, byte ptr[esi - 0x18]
		add edx, dword ptr[ebp + 0x34E8FFF6]
		add al, 0x78
		add eax, 0x80000001


		//���� 06~07
		PUSH es
		POP es

		//����08~0E
		or byte ptr[ebx - 0x173B7C14], cl
		or dword ptr[esi + 0x57], edx
		or al, al
		or eax, dword ptr[ebp - 0x14]
		or al, 0xD8
		or eax, 0x94E80054
		push cs

		//����10~15
		adc byte ptr[eax + 0x4CF938], bh
		adc dword ptr[eax], edi
		adc cl, byte ptr[eax + eax - 0x18]
		adc edx, dword ptr[edi - 0x3]
		ADC al, 0x33
		ADC eax, 0xF3E8C033

		//����16~17
		PUSH ss
		POP ss

		//����20~25
		and byte ptr[eax - 0x4FFFB4C9], ah
		and dword ptr[ebx], ecx
		and bh, byte ptr[eax]
		and eax, dword ptr[eax]
		AND al, 0x38
		AND eax, 0x4B384800

		//����26
		adc byte ptr es : [0xF84CF938] , bh

		//����27
		DAA

		//����30~35
		xor byte ptr[eax + eax + 0x69], ch
		xor dword ptr[eax + eax + 0x20], esp
		xor dh, byte ptr[esi]
		xor eax, dword ptr[eax]
		xor al, 0x0
		XOR eax, 0x6E006100

		//����36
		and al, byte ptr ss : [eax]

		//����37
		AAA

		//����40~47
		INC eax
		INC ecx
		INC edx
		INC ebx
		INC esp
		INC ebp
		INC esi
		INC edi

		//����50~5F
		PUSH eax
		PUSH ecx
		PUSH edx
		PUSH ebx
		PUSH esp
		PUSH ebp
		PUSH esi
		PUSH edi
		POP eax
		POP ecx
		POP edx
		POP ebx
		POP esp
		POP ebp
		POP esi
		POP edi

		//����60��61
		PUSHAD
		POPAD

		//����62
		bound esp, qword ptr[ebx + 0xFF]	//	 62 63 64  boundָ��Ĳ��������дΪ������ʽ
		bound sp, dword ptr[ebx + 0x88]		//66 62 63 64

		//����63
		ARPL word ptr ds : [ebx + 0x38B8C4C4] , ax

		//����64��65
		mov dword ptr fs : [edx] , eax
		mov dword ptr gs : [edx] , eax

		//****һ����������Ҳ�п���ַ�����������****
		//push dword ptr fs : [ebx] 

		//����66
		add word ptr[eax], ax  //dword->word  eax->ax

		//����67
		//add dword ptr[bx + si], eax // [eax] -> [bx+si]   //MSVC����֧������д��

		//����68~6F
		push 0x41C82BDF
		imul edx, ebx, 0xD03348CA
		push 0x6A
		imul esp, dword ptr[edx + eax * 8 + 0x6D6CFFFF], 0x6D
		ins byte ptr[edi], dx
		ins dword ptr[edi], dx
		outsb
		outsd

		//����70~77
		JO __testJCC
		JNO __testJCC
		JB __testJCC
		JNB __testJCC
		JE __testJCC
		JNE __testJCC
		JBE __testJCC
		JNBE __testJCC

		//���ڲ���JCC����ת��ַ(����ת����Ҫ̫Զ)��JCC��VC��������в���ֱ��д��ַ����Ҫ��'__XXX'��ǩ
		__testJCC :
		//����80���漰GRUOP1��
		adc byte ptr[edi - 0x3], 0xFF
			cmp byte ptr[eax], 0xF9
			add byte ptr[eax - 0x3B7C1375], 0x11

			//����81���漰GRUOP1��
			adc edx, 0xF3E8C033
			cmp edi, 0x75C084FF
			sub dword ptr[edx], 0xFD597CE8

			//����82���漰GRUOP1��
			//adc byte ptr ds : [eax] , 0x33	//0x82��ָ���ƺ����ȼ���0x80ָ�
			//xor byte ptr ds : [ebx] , -0x40

			//����83���漰GRUOP1��
			add dword ptr[edx], -0x5F	//�����������
			and dword ptr[eax], 0x4D	//�����������
			add eax, -0x18				//�����������
			add eax, 0xE8				//�����������

			//����84~87
			test byte ptr[eax + 0x4C7D24], al
			test ebx, esi
			xchg bh, bh
			xchg dword ptr[eax + eax * 8 + 0x6A2575], eax

			//����88~89
			mov byte ptr[ebp + 0x78563412], al				//88	����ModR/M
			mov byte ptr[eax + ecx * 2 + 0x78563412], al	//88	����SIB
			mov dword ptr[ebp + esi * 2 + 0x78563412], ebp	//89	����SIB

			//����8C
			mov word ptr[eax], es
			mov word ptr[eax], cs
			mov word ptr[eax], ss
			mov word ptr[eax], ds
			mov word ptr[eax], fs
			mov word ptr[eax], gs

			//����8F
			pop dword ptr[eax]

			//����90
			NOP
			xchg ecx, eax
			xchg edx, eax
			xchg ebx, eax
			xchg esp, eax
			xchg ebp, eax
			xchg esi, eax
			xchg edi, eax

			//����9B
			wait

			//����9C
			PUSHF
			PUSHFD
			POPF
			POPFD

			//����A0~A3
			mov al, byte ptr ds : [0xFFF3F3E8]
			mov eax, dword ptr ds : [0x2575C014]
			mov byte ptr ds : [0xFC158BFF] , al
			mov dword ptr ds : [0x597CE800] , eax

			//����A4~A7
			movs byte ptr es : [edi] , byte ptr ds : [esi]		//		A4  �ȼ��� MOVSBָ��
			movs dword ptr es : [edi] , dword ptr ds : [esi]	//		A5  �ȼ��� MOVSDָ��
			movs word ptr es : [edi] , word ptr ds : [esi]		//66	A5  �ȼ��� MOVSWָ��
			cmps byte ptr ds : [esi] , byte ptr es : [edi]		//		A6	�ȼ��� CMPSBָ��
			cmps dword ptr ds : [esi] , dword ptr es : [edi]	//		A7	�ȼ��� CMPSDָ��
			cmps word ptr ds : [esi] , word ptr es : [edi]		//66	A7	�ȼ��� CMPSWָ��

			//����AA~AF
			STOSB						//		AA
			stos dword ptr[edi]			//		AB
			stos word ptr[edi]			//66	AB
			lods byte ptr[esi]			//		AC
			lods dword ptr[esi]			//		AD
			scas byte ptr[edi]			//		AE
			scas dword ptr[edi]			//		AF

			//����B0~BF
			mov al, 0x8B
			mov cl, 0x8B
			mov dl, 0x8B
			mov bl, 0x8B
			mov ah, 0x8B
			mov ch, 0x8B
			mov dh, 0x8B
			mov bh, 0x8B
			mov eax, 0xEC4589C0	//		B8
			mov ax, 0x8964		//66	B8



			//����C0
			rol byte ptr[eax], 0xE8
			rcl byte ptr[edi - 0x3], 0xFF
			sar byte ptr[eax], 0xF9
			shr al, 0xF3
			rcr byte ptr[ecx - 0x3], 0xFF
			shl byte ptr[ecx + 0x4D2078], 0x8B

			//����C1
			rol dword ptr[eax], 0xE8
			rcl dword ptr[eax], 0xE8
			sar dword ptr[eax], 0xE8
			shr dword ptr[eax], 0xE8
			rcr dword ptr[eax], 0xE8
			shl dword ptr[eax], 0xE8

			//����C4~C5
			les edx, fword ptr[edi - 0x3]
			les edi, fword ptr[eax]
			lds eax, fword ptr[eax + eax * 8 + 0x6A2575]

			//����C6~C7
			mov byte ptr ds : [ecx - 0x3] , 0x8B
			mov dword ptr ds : [eax] , 0x1

			//����D0~D3
			shl byte ptr ds : [ecx + ecx * 4 + 0x20] , 1
			sar edi, 1
			rol byte ptr ds : [0x546714] , cl
			rol eax, cl

			//����D4~D5
			//aam 0x8B //VC��֧�֣�
			//aad 0x56 //VC��֧�֣�
			__testLoopXX :

		//����D7
		XLAT

			//����E0~E2
			loopne  __testLoopXX
			loope	__testLoopXX
			loop	__testLoopXX

			//����E3
			jecxz  __testJecxzXX

			__testJecxzXX :

		//����E4~E7
		in al, 0x89
			in eax, 0x4C
			out 0x5, al
			out 0x54, eax


			//����F0
			lock push esp
			lock mov dword ptr fs : [eax] , esp

			//����F2~F3
			repne CMPSD
			repe movs byte ptr es : [edi] , byte ptr ds : [esi]	//MOVSD

			//����F4~F5
			HLT
			CMC

			//����F6~F7
			test byte ptr[eax], 0xE8	//F6 
			not byte ptr[ebp - 0x14]	//F6
			neg ah						//F6
			idiv bh						//F6
			test dword ptr[eax], 0x1	//F7
			div dword ptr[eax]			//F7
			imul dword ptr[eax - 0xA]	//F7

			//����FE~FF
			inc byte ptr[eax]	//FE		INC Eb
			dec byte ptr[ebx]	//FE		DEC Eb
			inc dword ptr[eax]	//FF		INC Ev
			dec dword ptr[eax]	//FF		DEC Ev
			call dword ptr[eax]	//FF		near CALL Ev (f64)
			call fword ptr[eax]	//FF		far CALL Ep
			jmp dword ptr[eax]	//FF		near JMP Ev (f64)
			jmp fword ptr[eax]	//FF		far JMP Mp
			push dword ptr[eax]	//FF		PUSH Ev (d64)

			//*********************��3*************************
			//����0F 00 
			sldt eax
			sldt word ptr[ecx + 0x56]
			str eax
			ltr di
			verr bx

			//����0F 01
			//ȫ����VC���Ϊ��sgdt  fword ptr [ecx+56h]  ��6�ֽڣ�
			sgdt tbyte ptr[ecx + 56h]
			sgdt fword ptr[ecx + 56h]
			sgdt word ptr[ecx + 56h]
			sgdt byte ptr[ecx + 56h]
			sgdt dword ptr[ecx + 56h]

			//����0F 02 ~ 0F 03
			lar ebp, word ptr[eax - 0xA]
			lsl eax, word ptr[eax]





			//����RET
			RET 0x6688	//C2
			RET			//C3


	}
}
#endif // !WIN32

