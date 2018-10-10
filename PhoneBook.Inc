.386
.model flat,stdcall
option casemap:none

include windows.inc
include msvcrt.inc
includelib msvcrt.lib
include     kernel32.inc
includelib  kernel32.lib

.data
;����ṹ��
CONTACTSSTRUCT STRUCT
	szName byte 25 dup(0)  ;����
	szPhNumber byte 12 dup(0)  ;�绰����
CONTACTSSTRUCT ends

PCONTACTSSTRUCT typedef ptr CONTACTSSTRUCT   ;ȡ������ָ�����ͣ�


;����ȫ�ֱ���

g_stContacts CONTACTSSTRUCT 100 dup(<'0'>)   ;����ṹ������
g_nCount dword 0   ;Ԫ�ظ���
g_nCountMax dword 100   ;���Ĵ��Ԫ��
g_strTemContacts CONTACTSSTRUCT <'0','0'>   ;����������Ϣ

;�����ʽ���Ʒ�

g_szScanfFormat byte "%s %s",0h
g_szScanName byte "%s",0h


g_szScanfFormatS    db  "%s",0
g_szScanfFormatSS   db  "��ţ�%d",0dh,0ah,
                        "������%s",0dh,0ah,
                        "���룺%s",0dh,0ah,0

g_szEmpty   db  "��������!",0


g_szAddStr byte "���������� ���룺",0

g_szRemove byte "������Ҫɾ�����û�����",0

g_szModify byte "������Ҫ�޸ĵ��û�����",0

g_szModifyData byte "�������µ��û��� �µĺ��룺",0

g_szFind byte "������Ҫ��ѯ��������",0

g_nTemp dword 0   

g_szCLS  byte  "cls",0

g_szPause byte "pause",0

g_szOK byte "�����ɹ�",0

g_szScanfFormatD byte "%d",0

g_menu  byte 0ah,0dh,"��ӭʹ�õ绰��",0ah,0dh,
	 "1.���Ӽ�¼",0ah,0dh,
	 "2.ɾ����¼",0ah,0dh,
	 "3.�޸ļ�¼",0ah,0dh,
	 "4.��ѯ��¼",0ah,0dh,
	 "5.��ѯ����",0ah,0dh,
	 "��������Ĳ���ѡ�",0ah,0dh,0

.code


sys_cls proc
    push offset g_szCLS
    call crt_system
    add esp,4
    ret
sys_cls endp


;����˵�
MenuInfo proc   ;�޲���    
    call sys_cls
    push offset g_menu
    call crt_printf
    add esp,4
    ret
MenuInfo endp



;ѡ��˵���
SwitchMenu  proc        ;�޲���


    	cmp eax,1
	jnz @T1


    	cmp [g_nTemp],1     ;1 ����
        jz  Step1

    	cmp [g_nTemp],2    ;2 ɾ��
        jz  Step2

    	cmp [g_nTemp],3     ;3 �޸�
        jz  Step3

    	cmp [g_nTemp],4     ;4 ��ѯ
        jz  Step4
        
    	cmp [g_nTemp],5     ;5 ��ѯ
        jz  Step5

    	jmp STEND


;1 ����
Step1:
    call ADD_USER
    jmp STEND

;2 ɾ��
Step2:
    call RemoveData
    jmp STEND 
    
    
;3 �޸�
Step3:
    call ModifyData
    jmp STEND
    
;4 ��ѯ
Step4:
    call FindData
    jmp STEND
    
Step5:
	call FindAll
	jmp STEND

STEND:
    ret
    
    
@T1:
        call crt_getchar
        cmp eax,0ah
        jnz @T1
        
        jmp CYCLE_MAIN
    
SwitchMenu  endp


FindAll proc
	
	call sys_cls    
    ;�����׵�ַ
    lea eax,g_stContacts
    xor ebx,ebx
    ;�Աȵ�ǰ����
    cmp dword ptr [g_nCount],0  
    je Empty    
    jne CYCLE_READ

;�����ݽ���
Empty:      
    push offset g_szEmpty
    push offset g_szScanfFormatS
    call crt_printf
    add esp,8
    jmp ShowAll_End

;��ȡ����
CYCLE_READ:
    ;���δ�������
    add eax,19h
    push eax
    sub eax,19h
    push eax

    push ebx
    push offset g_szScanfFormatSS
    call crt_printf
    add esp,16
    ;printf������Ӱ��A/C/D�Ĵ�����ֵ,��Ҫ���´����ַ
    inc ebx
    
    mov ecx,sizeof(CONTACTSSTRUCT)
    ;mov ecx,30
    imul ecx,ebx
    lea eax,g_stContacts
    
    add eax,ecx


    cmp dword ptr [g_nCount],ebx
    je ShowAll_End
    jmp CYCLE_READ
    
    
;��ѯ����
ShowAll_End:  
    	push offset g_szPause
    	call crt_system
    	add esp,4
    
    ret
FindAll endp



ADD_USER proc   ;�޲���
	
	push eax
	push ebx   ;�ȱ���һ��
	lea eax,g_szAddStr
	
	push eax    ;r/m32/imm32
	call crt_printf
	add esp,4
	
	;����ecx��ֵ�ҵ���һ���ṹ����������ĵ�ַ
	lea esi,[g_stContacts]   ;�������ݵĽṹ������
	mov ecx,g_nCount   ;��ȡ��ǰ�Ѳ�����û�����
	mov eax,sizeof(CONTACTSSTRUCT)   ;����ṹ��Ĵ�С
	imul eax,ecx
	add esi,eax   ;�ƶ��ṹ�������ָ�루�û�����*�ṹ��Ĵ�С��
	
	;����crt_scanf����������������
	lea eax,[esi+CONTACTSSTRUCT.szPhNumber]   ;�ڶ�������  �绰����
	lea edx,[esi+CONTACTSSTRUCT.szName]   ;��һ������   ����
	
	push eax
	push edx
	push offset g_szScanfFormat   ;��ʽ���Ʒ�
	call crt_scanf
	
	add esp,12   ;ƽ���ջ
	;inc g_nCount   ;�û�������1
	inc dword ptr[g_nCount]
	pop eax   ;ƽ���ջ
	pop ebx
	
	
	push offset g_szOK   ;�����ɹ�
	call crt_printf
	add esp,4
	
	
	push offset g_szPause
    	call crt_system
    	add esp,4
	
	ret

ADD_USER endp




FindData proc
	push offset g_szFind
	call crt_printf
	add esp,4
	
	;�����
	lea edi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	push ebx
	push 0
	push edi
	call crt_memset
	add esp,12
		
	
	;��������
	lea edi,[g_strTemContacts.szName]  ;����ṹ�������ֵĵ�ַ
	push edi
	push offset g_szScanName  ;��ʽ���Ʒ�
	call crt_scanf
	add esp,8
	
	;��ʼ��ѯ
	mov ecx,0   ;��ʼ��ѭ������  Ĭ�ϴ�0��ʼ
	
	mov eax,[g_nCount]
	
	CYCLE_MARK:
		cmp ecx,g_nCount   ;�ж��Ƿ����ѭ��
		;ret
		je @end
		
	lea esi,[g_stContacts]   ;�������ݵĽṹ������
	lea edi,[g_strTemContacts.szName]   ;��ȡ��ǰ�Ѳ����û�
	mov eax,sizeof(CONTACTSSTRUCT)   ;����ṹ��Ĵ�С
	imul  eax,ecx
	add esi,eax
	
	
	;�Ƚ��ַ���
	mov eax,ecx   ;�������ѭ���Ĵ���
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;����ҵ�����ת  �����Ϣ
	mov ecx,eax   ;���û���ҵ���������ѭ��
	inc ecx   ;��ѭ��������1
	jmp CYCLE_MARK   ;��������ת�����ѭ����ʼλ��
	
	
	CARRIEDOUT_MARK:
	
	;�����Ϣ
	mov ecx,eax
	lea esi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	imul ebx,ecx
	add esi,ebx
	lea eax,[esi+CONTACTSSTRUCT.szPhNumber]
	push eax
	push offset g_szScanName
	call crt_printf
	add esp,8
	
	
	push offset g_szPause
    	call crt_system
    	add esp,4
	
	
	ret
	
@end:
	ret

FindData endp




ModifyData proc
	
	push offset g_szModify
	call crt_printf
	add esp,4
	
	
	;��Ϊ�޸���Ϣ�ĵ�һ��Ҳ��Ҫ����ǰ�������Ϣ���ѱ���������в�ѯ
	;��������
	lea edi,[g_strTemContacts.szName]   ;����ṹ�������ֵĵ�ַ
	push edi
	push offset g_szScanName   ;��ʽ�����Ʒ�
	call crt_scanf
	add esp,8   ;ƽ��
	
	;��ʼ��ѯ
	mov ecx,0
	
CYCLE_MARK:
	cmp ecx,g_nCount    ;�ж��Ƿ����ѭ��
	je @end
	
	;����ecx��ֵ�ҵ���һ���ṹ����������ĵ�ַ
	lea esi,[g_stContacts]   ;�������ݵĽṹ������
	
	
	lea edi,[g_strTemContacts.szName]   ;��ȡ��ǰ�Ѳ����û�����
	mov eax,sizeof(CONTACTSSTRUCT)   ;����ṹ��Ĵ�С
	imul eax,ecx
	add esi,eax
	
	
	;�Ƚ��ַ���
	mov eax,ecx   ;�������ѭ���Ĵ���
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;����ҵ�����ת  �޸���Ϣ
	mov ecx,eax   ;���û���ҵ���������ѭ��
	inc ecx   ;��ѭ��������1
	jmp CYCLE_MARK   ;��������ת�����ѭ����ʼλ��
	
	
	
CARRIEDOUT_MARK:
	;�޸���Ϣ
	mov ecx,eax
	lea esi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	imul ebx,ecx
	add esi,ebx
	
	
	push offset g_szModifyData
	call crt_printf
	add esp,4
	
		
	lea ebx,[esi+CONTACTSSTRUCT.szName]
	lea eax,[esi+CONTACTSSTRUCT.szPhNumber]
	push eax
	push ebx
	push offset g_szScanfFormat
	call crt_scanf
	add esp,12
	
	push offset g_szOK   ;�����ɹ�
	call crt_printf
	add esp,4
	
	push offset g_szPause
    	call crt_system
    	add esp,4
	
	ret
	
@end:
	ret
ModifyData endp



RemoveData proc
	
	push offset g_szRemove
	call crt_printf
	add esp,4
	
	
	;��������
	lea edi,[g_strTemContacts.szName]   ;����ṹ�������ֵĵ�ַ
	push edi
	push offset g_szScanName   ;��ʽ�����Ʒ�
	call crt_scanf
	add esp,8   ;ƽ��
	
	;��ʼ��ѯ
	mov ecx,0
	
	CYCLE_MARK:
	cmp ecx,g_nCount    ;�ж��Ƿ����ѭ��
	;ret
	je @end
	
	;����ecx��ֵ�ҵ���һ���ṹ����������ĵ�ַ
	lea esi,[g_stContacts]   ;�������ݵĽṹ������
	
	
	lea edi,[g_strTemContacts.szName]   ;��ȡ��ǰ�Ѳ����û�����
	mov eax,sizeof(CONTACTSSTRUCT)   ;����ṹ��Ĵ�С
	imul eax,ecx
	add esi,eax
	
	
	;�Ƚ��ַ���
	mov eax,ecx   ;�������ѭ���Ĵ���
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;����ҵ�����ת  �޸���Ϣ
	mov ecx,eax   ;���û���ҵ���������ѭ��
	inc ecx   ;��ѭ��������1
	jmp CYCLE_MARK   ;��������ת�����ѭ����ʼλ��
	
	
	
	CARRIEDOUT_MARK:
	;ɾ��
	
	;��esi����Ϊ��ǰҪɾ���Ľṹ��������׵�ַ
	mov edi,eax   ;eax���������ȡ���ı�ʾ��ǰ�ҵ������ݵ�λ��
	lea edi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	
	imul ebx,ecx   ;edi��ʱ������ǵ�ǰҪɾ���Ľṹ��������׵�ַ
	mov esi,edi   ;esi��ʱҲ�ǵ�ǰҪɾ���Ľṹ��������׵�ַ
	
	mov ebx,sizeof(CONTACTSSTRUCT)
	add esi,ebx   ;esiָ��Ҫɾ���Ľṹ���������һ��Ԫ�ص��׵�ַ
	
	add ecx,1   ;��Ϊ��������ʱ�Ǵ�����0��ʼ�ģ����Լ�1���ڼ���
		    ;��Ҫ�ƶ����ٸ�Ԫ�أ������ݽṹ�����Ա�һ�����м�ĳһ��Ԫ�ر�ɾ���������ǰ�ƶ�
	mov eax,g_nCount
	
	sub eax,ecx   ;��Ҫ�ƶ��Ĵ���
	
	mov  ebx,sizeof(CONTACTSSTRUCT)
	push ebx   ;��С
	push 0   ;����
	push edi   ;ɾ�����׵�ַ
	call crt_memset    ;�����ó�ʼ������
	
	add esp,12
	;dec g_nCount
	dec dword ptr[g_nCount]
	
	push offset g_szOK   ;�����ɹ�
	call crt_printf
	
	add esp,4
	push 1000
	call Sleep
	
	ret
@end:
	ret

RemoveData endp