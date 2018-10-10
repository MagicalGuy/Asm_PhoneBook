include PhoneBook.Inc


start:

	push ebp
    	mov ebp,esp


;操作菜单
CYCLE_MAIN:
;清屏
    	call sys_cls
    	
    	;invoke crt_system , addr g_szCLS
    	
    	
;显示菜单
    	call MenuInfo

;输入操作选项
    	push offset g_nTemp
    	push offset g_szScanfFormatD   ;%d
    	
    	call crt_scanf
    	add esp,8   ;循环内平衡堆栈 
    	

;其他数
    	call SwitchMenu

;循环
    	JMP CYCLE_MAIN
    	
    	
MAIN_END:
    	mov esp,ebp
    	pop ebp


end start
end