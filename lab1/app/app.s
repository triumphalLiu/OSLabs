.code32

.global start
start:                       
	movb $0x0c, %ah         
	movl $((80*8+0)*2), %edi                
	movb $72, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+1)*2), %edi                       
	movb $101, %al                        
	movw %ax, %gs:(%edi)     
	movl $((80*8+2)*2), %edi               
	movb $108, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+3)*2), %edi                  
	movb $108, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+4)*2), %edi                  
	movb $111, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+5)*2), %edi                  
	movb $44, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+6)*2), %edi                  
	movb $0, %al                        
	movw %ax, %gs:(%edi)   
	movl $((80*8+7)*2), %edi                   
	movb $87, %al                        
	movw %ax, %gs:(%edi)   
	movl $((80*8+8)*2), %edi                 
	movb $111, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+9)*2), %edi                  
	movb $114, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+10)*2), %edi                  
	movb $108, %al                        
	movw %ax, %gs:(%edi) 
	movl $((80*8+11)*2), %edi                   
	movb $100, %al                        
	movw %ax, %gs:(%edi)  
	movl $((80*8+12)*2), %edi                  
	movb $33, %al                        
	movw %ax, %gs:(%edi) 
loop:  
	jmp loop
