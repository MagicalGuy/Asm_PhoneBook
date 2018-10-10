.386
.model flat,stdcall
option casemap:none

include windows.inc
include msvcrt.inc
includelib msvcrt.lib
include     kernel32.inc
includelib  kernel32.lib

.data
;定义结构体
CONTACTSSTRUCT STRUCT
	szName byte 25 dup(0)  ;名字
	szPhNumber byte 12 dup(0)  ;电话号码
CONTACTSSTRUCT ends

PCONTACTSSTRUCT typedef ptr CONTACTSSTRUCT   ;取别名（指正类型）


;声明全局变量

g_stContacts CONTACTSSTRUCT 100 dup(<'0'>)   ;定义结构体数组
g_nCount dword 0   ;元素个数
g_nCountMax dword 100   ;最大的存放元素
g_strTemContacts CONTACTSSTRUCT <'0','0'>   ;接收输入信息

;定义格式控制符

g_szScanfFormat byte "%s %s",0h
g_szScanName byte "%s",0h


g_szScanfFormatS    db  "%s",0
g_szScanfFormatSS   db  "编号：%d",0dh,0ah,
                        "姓名：%s",0dh,0ah,
                        "号码：%s",0dh,0ah,0

g_szEmpty   db  "暂无数据!",0


g_szAddStr byte "请输入姓名 号码：",0

g_szRemove byte "请输入要删除的用户名：",0

g_szModify byte "请输入要修改的用户名：",0

g_szModifyData byte "请输入新的用户名 新的号码：",0

g_szFind byte "请输入要查询的姓名：",0

g_nTemp dword 0   

g_szCLS  byte  "cls",0

g_szPause byte "pause",0

g_szOK byte "操作成功",0

g_szScanfFormatD byte "%d",0

g_menu  byte 0ah,0dh,"欢迎使用电话本",0ah,0dh,
	 "1.增加记录",0ah,0dh,
	 "2.删除记录",0ah,0dh,
	 "3.修改记录",0ah,0dh,
	 "4.查询记录",0ah,0dh,
	 "5.查询所有",0ah,0dh,
	 "请输入你的操作选项：",0ah,0dh,0

.code


sys_cls proc
    push offset g_szCLS
    call crt_system
    add esp,4
    ret
sys_cls endp


;输出菜单
MenuInfo proc   ;无参数    
    call sys_cls
    push offset g_menu
    call crt_printf
    add esp,4
    ret
MenuInfo endp



;选择菜单项
SwitchMenu  proc        ;无参数


    	cmp eax,1
	jnz @T1


    	cmp [g_nTemp],1     ;1 添加
        jz  Step1

    	cmp [g_nTemp],2    ;2 删除
        jz  Step2

    	cmp [g_nTemp],3     ;3 修改
        jz  Step3

    	cmp [g_nTemp],4     ;4 查询
        jz  Step4
        
    	cmp [g_nTemp],5     ;5 查询
        jz  Step5

    	jmp STEND


;1 添加
Step1:
    call ADD_USER
    jmp STEND

;2 删除
Step2:
    call RemoveData
    jmp STEND 
    
    
;3 修改
Step3:
    call ModifyData
    jmp STEND
    
;4 查询
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
    ;传入首地址
    lea eax,g_stContacts
    xor ebx,ebx
    ;对比当前容量
    cmp dword ptr [g_nCount],0  
    je Empty    
    jne CYCLE_READ

;无数据结束
Empty:      
    push offset g_szEmpty
    push offset g_szScanfFormatS
    call crt_printf
    add esp,8
    jmp ShowAll_End

;读取数据
CYCLE_READ:
    ;传参从右至左
    add eax,19h
    push eax
    sub eax,19h
    push eax

    push ebx
    push offset g_szScanfFormatSS
    call crt_printf
    add esp,16
    ;printf函数会影响A/C/D寄存器的值,需要重新传入地址
    inc ebx
    
    mov ecx,sizeof(CONTACTSSTRUCT)
    ;mov ecx,30
    imul ecx,ebx
    lea eax,g_stContacts
    
    add eax,ecx


    cmp dword ptr [g_nCount],ebx
    je ShowAll_End
    jmp CYCLE_READ
    
    
;查询结束
ShowAll_End:  
    	push offset g_szPause
    	call crt_system
    	add esp,4
    
    ret
FindAll endp



ADD_USER proc   ;无参数
	
	push eax
	push ebx   ;先保存一份
	lea eax,g_szAddStr
	
	push eax    ;r/m32/imm32
	call crt_printf
	add esp,4
	
	;根据ecx的值找到下一个结构体名字数组的地址
	lea esi,[g_stContacts]   ;保存数据的结构体数组
	mov ecx,g_nCount   ;获取当前已插入的用户个数
	mov eax,sizeof(CONTACTSSTRUCT)   ;计算结构体的大小
	imul eax,ecx
	add esi,eax   ;移动结构体数组的指针（用户个数*结构体的大小）
	
	;调用crt_scanf函数接收输入数据
	lea eax,[esi+CONTACTSSTRUCT.szPhNumber]   ;第二个参数  电话号码
	lea edx,[esi+CONTACTSSTRUCT.szName]   ;第一个参数   姓名
	
	push eax
	push edx
	push offset g_szScanfFormat   ;格式控制符
	call crt_scanf
	
	add esp,12   ;平衡堆栈
	;inc g_nCount   ;用户个数加1
	inc dword ptr[g_nCount]
	pop eax   ;平衡堆栈
	pop ebx
	
	
	push offset g_szOK   ;操作成功
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
	
	;先清除
	lea edi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	push ebx
	push 0
	push edi
	call crt_memset
	add esp,12
		
	
	;输入数据
	lea edi,[g_strTemContacts.szName]  ;保存结构体中名字的地址
	push edi
	push offset g_szScanName  ;格式控制符
	call crt_scanf
	add esp,8
	
	;开始查询
	mov ecx,0   ;初始化循环次数  默认从0开始
	
	mov eax,[g_nCount]
	
	CYCLE_MARK:
		cmp ecx,g_nCount   ;判断是否结束循环
		;ret
		je @end
		
	lea esi,[g_stContacts]   ;保存数据的结构体数组
	lea edi,[g_strTemContacts.szName]   ;获取当前已插入用户
	mov eax,sizeof(CONTACTSSTRUCT)   ;计算结构体的大小
	imul  eax,ecx
	add esi,eax
	
	
	;比较字符串
	mov eax,ecx   ;保存外层循环的次数
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;如果找到则跳转  输出信息
	mov ecx,eax   ;如果没有找到则继续外层循环
	inc ecx   ;层循环次数加1
	jmp CYCLE_MARK   ;无条件跳转到外层循环开始位置
	
	
	CARRIEDOUT_MARK:
	
	;输出信息
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
	
	
	;因为修改信息的第一步也是要将当前输入的信息在已保存额数组中查询
	;输入数据
	lea edi,[g_strTemContacts.szName]   ;保存结构体中名字的地址
	push edi
	push offset g_szScanName   ;格式化控制符
	call crt_scanf
	add esp,8   ;平衡
	
	;开始查询
	mov ecx,0
	
CYCLE_MARK:
	cmp ecx,g_nCount    ;判断是否结束循环
	je @end
	
	;根据ecx的值找到下一个结构体名字数组的地址
	lea esi,[g_stContacts]   ;保存数据的结构体数组
	
	
	lea edi,[g_strTemContacts.szName]   ;获取当前已插入用户个数
	mov eax,sizeof(CONTACTSSTRUCT)   ;计算结构体的大小
	imul eax,ecx
	add esi,eax
	
	
	;比较字符串
	mov eax,ecx   ;保存外层循环的次数
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;如果找到则跳转  修改信息
	mov ecx,eax   ;如果没有找到则继续外层循环
	inc ecx   ;层循环次数加1
	jmp CYCLE_MARK   ;无条件跳转到外层循环开始位置
	
	
	
CARRIEDOUT_MARK:
	;修改信息
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
	
	push offset g_szOK   ;操作成功
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
	
	
	;输入数据
	lea edi,[g_strTemContacts.szName]   ;保存结构体中名字的地址
	push edi
	push offset g_szScanName   ;格式化控制符
	call crt_scanf
	add esp,8   ;平衡
	
	;开始查询
	mov ecx,0
	
	CYCLE_MARK:
	cmp ecx,g_nCount    ;判断是否结束循环
	;ret
	je @end
	
	;根据ecx的值找到下一个结构体名字数组的地址
	lea esi,[g_stContacts]   ;保存数据的结构体数组
	
	
	lea edi,[g_strTemContacts.szName]   ;获取当前已插入用户个数
	mov eax,sizeof(CONTACTSSTRUCT)   ;计算结构体的大小
	imul eax,ecx
	add esi,eax
	
	
	;比较字符串
	mov eax,ecx   ;保存外层循环的次数
	mov ecx,6
	
	repe cmpsd dword ptr[esi],dword ptr[edi]
	
	je CARRIEDOUT_MARK   ;如果找到则跳转  修改信息
	mov ecx,eax   ;如果没有找到则继续外层循环
	inc ecx   ;层循环次数加1
	jmp CYCLE_MARK   ;无条件跳转到外层循环开始位置
	
	
	
	CARRIEDOUT_MARK:
	;删除
	
	;将esi设置为当前要删除的结构体数组的首地址
	mov edi,eax   ;eax是在上面获取到的表示当前找到的数据的位置
	lea edi,[g_stContacts]
	mov ebx,sizeof(CONTACTSSTRUCT)
	
	imul ebx,ecx   ;edi此时保存的是当前要删除的结构体数组的首地址
	mov esi,edi   ;esi此时也是当前要删除的结构体数组的首地址
	
	mov ebx,sizeof(CONTACTSSTRUCT)
	add esi,ebx   ;esi指向要删除的结构体数组的下一个元素的首地址
	
	add ecx,1   ;因为保存数据时是从数组0开始的，所以加1用于计算
		    ;需要移动多少个元素，和数据结构中线性表一样，中间某一个元素被删除后面的向前移动
	mov eax,g_nCount
	
	sub eax,ecx   ;需要移动的次数
	
	mov  ebx,sizeof(CONTACTSSTRUCT)
	push ebx   ;大小
	push 0   ;内容
	push edi   ;删除的首地址
	call crt_memset    ;调用置初始化函数
	
	add esp,12
	;dec g_nCount
	dec dword ptr[g_nCount]
	
	push offset g_szOK   ;操作成功
	call crt_printf
	
	add esp,4
	push 1000
	call Sleep
	
	ret
@end:
	ret

RemoveData endp