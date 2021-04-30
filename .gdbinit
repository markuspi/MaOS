target remote :1234
dashboard registers -style list "eax ebx ecx edx esp ebp esi edi eip cr0 cr2 cr3 cr4 cs"
break *0x0010000c
continue
