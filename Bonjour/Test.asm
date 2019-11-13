.CODE
TestAsm PROC

;**********≤‚ ‘±Ì1**************************

	;≤‚ ‘66«∞◊∫(oprand size->16, address size=64)
	ADD word ptr[rax], ax	  
	;≤‚ ‘67«∞◊∫(oprand size=32 , address size->32)
	ADD dword ptr [eax], eax

	;≤‚ ‘REX«∞◊∫
	add dword ptr [rax], eax	;40 or none
	add dword ptr [r8], eax		;41
	add dword ptr [rax], r8d	;44
	add dword ptr [r8], r8d		;45
	add qword ptr [rax], rax	;48

	;≤‚ ‘50~5F  
	PUSH rax
	PUSH rcx
	PUSH rdx
	PUSH rbx
	PUSH rsp
	PUSH rbp
	PUSH rsi
	PUSH rdi
	POP rax
	POP rcx
	POP rdx
	POP rbx
	POP rsp
	POP rbp
	POP rsi
	POP rdi

	;≤‚ ‘ 41 50 ~ 41 5F
	PUSH r8
	PUSH r9
	PUSH r10
	PUSH r11
	PUSH r12
	PUSH r13
	PUSH r14
	PUSH r15
	POP r8
	POP r9
	POP r10
	POP r11
	POP r12
	POP r13
	POP r14
	POP r15

	;≤‚ ‘70~77
	JO __testJCC
	JNO __testJCC
	JB __testJCC
	JNB __testJCC
	JE __testJCC
	JNE __testJCC
	JBE __testJCC
	JNBE __testJCC
__testJCC:

	;≤‚ ‘80
	and byte ptr [rbx], 63h
	add ah, 38h
	or ah, 64h

	;≤‚ ‘81
	and dword ptr [rbx-7Dh], 12345678h
	add dword ptr [rbx-333CC73Ch], 55182474h
	add dword ptr [rcx+56h], 24AC8D48h

	;≤‚ ‘83
	cmp edi, 4Eh
	or dword ptr [rax-73h], 0FFFFFFFFh
	add dword ptr [rax], 0FFFFFFFFh		;’‚¿Ô”–Œ Ã‚£¨Œ™ ≤√¥FFFFFFFFª·±ªVCª„±‡Œ™FF£ø

	;≤‚ ‘84~87
	test byte ptr [rbx+0A540705h], cl
	test dword ptr [rax-73h], ecx
	xchg byte ptr [rax+330004A8h], ah
	xchg dword ptr [rax-77h], ecx
	
	;≤‚ ‘8C
	mov word ptr [rax] , es
	mov word ptr [rax] , cs
	mov word ptr [rax] , ss
	mov word ptr [rax] , ds
	mov word ptr [rax] , fs
	mov word ptr [rax] , gs

	;≤‚ ‘8D
	lea ecx, [rbx+0FC085D8h]
	lea rcx, [rbx+0FC085D8h]


	;≤‚ ‘8E
	mov es, word ptr [rcx]

	;≤‚ ‘8F
	pop qword ptr [rbx]

	;≤‚ ‘90
	NOP				;90
	xchg r8d, eax	;40 90
	xchg r8d, eax	;43	90
	xchg r8d, eax	;45	90
	xchg r8, rax	;49 90

	;≤‚ ‘91~97
	xchg ecx, eax
	xchg edx, eax
	xchg ebx, eax
	xchg esp, eax
	xchg ebp, eax
	xchg esi, eax
	xchg edi, eax

	;≤‚ ‘98°¢99
	cbw		;66:98
	cwde	;	98
	cdqe	;48:98
	cwd		;66:99
	cdq		;	99
	cqo		;48:99

	
	;≤‚ ‘9B
	wait

	;≤‚ ‘9C
	PUSHF
	PUSHFQ




	;≤‚ ‘48 91 ~ 48 97
	xchg rcx, rax
	xchg rdx, rax
	xchg rbx, rax
	xchg rsp, rax
	xchg rbp, rax
	xchg rsi, rax
	xchg rdi, rax

	;≤‚ ‘49 91 ~ 49 97
	xchg r8, rax
	xchg r9, rax
	xchg r10, rax
	xchg r11, rax
	xchg r12, rax
	xchg r13, rax
	xchg r14, rax
	xchg r15, rax

	;≤‚ ‘A0~A3
	mov al, byte ptr [44C75024448948FFh]	;	A0
	mov eax, dword ptr [448D480018001648h]	;	A1
	mov rax, qword ptr [448D480018001648h]	;48 A1
	mov byte ptr [8B48682444894870h], al	;	A2
	mov dword ptr [01000000602444C7h], eax	;	A3
	mov qword ptr [01000000602444C7h], rax	;48	A3

	;≤‚ ‘A4~A7
	movsb		;	A4	movs byte ptr [rdi],byte ptr [rsi]
	movsd		;	A5	movs dword ptr [rdi],dword ptr [rsi] 
	movsw		;66 A5	movs word ptr [rdi],word ptr [rsi] 
	movsq		;48 A5	movs qword ptr [rdi],qword ptr [rsi]  
	cmpsb		;	A6
	cmpsd		;	A7
	cmpsw		;66	A7
	cmpsq		;48	A7

	;≤‚ ‘B0~BF
	mov al, 0A5h
	mov cl, 0A5h
	mov dl, 0A5h
	mov bl, 0A5h
	mov ah, 0A5h
	mov ch, 0A5h
	mov dh, 0A5h
	mov bh, 0A5h
	mov eax, 16482444h			;	B8
	mov rax, 89487024448D4800h	;48	B8
	mov r8, 89487024448D4800h	;49	B8



	;≤‚ ‘C0
	rol al, 04Bh					
	ror byte ptr [rax-73h], 54h		
	rcl bl, 79h
	rcr bl, 0AAh
	shl bh, 0FFh
	shr al, 0D8h
	sar bl, 0D0h


	;≤‚ ‘C1
	rol dword ptr [rcx], 0CBh		;	C1
	ror dword ptr [rax-73h], 54h	;	C1	
	rcl dword ptr [rax], 0FFh		;	C1
	rcr dword ptr [rax], 0FFh		;	C1
	shl dword ptr [rax], 0FFh		;	C1
	shr dword ptr [rax], 0FFh		;	C1
	sar dword ptr [rax], 0FFh		;	C1
	rol dword ptr [ecx], 0CBh		;67	C1
	ror dword ptr [eax-73h], 54h	;67	C1	
	rcl dword ptr [eax], 0FFh		;67	C1
	
	;≤‚ ‘CC~CD
	int 3
	INT 66h

	;≤‚ ‘E0~E2
	loopne  __testLoopXX
	loope	__testLoopXX
	loop	__testLoopXX
__testLoopXX:

	;≤‚ ‘E3
	 JrCXZ __testJrCXZ
__testJrCXZ:

	;≤‚ ‘E4~E7
	in al, 89h
	in eax, 4Ch
	out 5h, al
	out 54h, eax

	;≤‚ ‘F2~F3
	repne cmpsd
	rep movsd

	;≤‚ ‘F6~F7
	test byte ptr [rcx+55182474H], 57H		;F6
	not byte ptr [rsi+48H]					;F6
	imul byte ptr [rsp-100H]				;F6
	div dword ptr [rbx]						;F7
	test dword ptr [rax-77H], 0F085H		;F7
	test dword ptr [rbx+rcx*4+5H], 0A5407H	;F7



;**********≤‚ ‘±Ì3**************************

	;≤‚ ‘0F 00
	str word ptr [rax-7Dh]
	sldt ebx

	;≤‚ ‘0F01
	;»´≤ø±ªvcª„±‡Œ™ sgdt  tbyte ptr [rcx+56h]
	sgdt tbyte ptr [rcx+56h]	
	sgdt fword ptr [rcx+56h]
	sgdt dword ptr [rcx+56h]
	sgdt word ptr [rcx+56h]

	;≤‚ ‘0F02~0F03°¢0F05~0F07
	lar eax, word ptr [12345678h]
	lsl ecx, word ptr [rdi]
	SYSCALL
	CLTS
	SYSRET

	;≤‚ ‘0F80~0F8F
	jo __testJCC
	jno __testJCC
	js __testJCC
	jns __testJCC
	jle __testJCC
	jnle __testJCC

	;≤‚ ‘0FB0~0FB7
	cmpxchg byte ptr [rcx], al
	cmpxchg dword ptr [rdi], ecx
	lss esp, fword ptr [rax+rcx*2]
	lss sp, dword ptr [rax+rcx*2]
	lss rsp, tbyte ptr [rax+rcx*2]
	btr dword ptr [rsp+60h], ecx
	lfs ebp, fword ptr [rbx-9h]
	lgs esp, fword ptr [rsi-7Bh]
	movzx ecx, byte ptr [rax+11Eh]
	movzx eax, word ptr [rbp+70h]

	;≤‚ ‘
	movsxd rax, dword ptr [rbx-333CC73Ch]

    RET
TestAsm ENDP

END

