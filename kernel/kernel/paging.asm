
global set_page_directory
set_page_directory:
    mov dword ecx, [esp+4]
    mov cr3, ecx

    ; reload cr3 to force TLB flush
    mov ecx, cr3
    mov cr3, ecx

    ret
