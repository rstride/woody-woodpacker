section .text
    global _start

_start:
    call    _inject
    db      "....WOODY....\n", 0

_ft_strlen:
    xor     rax, rax
_strlen_loop:
    cmp     byte [rsi + rax], 0
    je      _strlen_end
    inc     rax
    jmp     _strlen_loop
_strlen_end:
    ret

_inject:
    pop     rsi                 ; Address of the message
    push    rdi                 ; Save rdi
    push    rdx                 ; Save rdx

    call    _ft_strlen
    mov     rdx, rax            ; Length of the message
    mov     rax, 1              ; sys_write
    mov     rdi, 1              ; stdout
    syscall                     ; Write message

    sub     rsi, 5              ; Adjust for 'call' instruction size

    ; Align address to page boundary
    mov     rdi, rsi
    and     rdi, 0xfffffffffffff000

    ; Calculate size to cover injected code
    mov     rsi, 0x1000         ; Size (adjust if needed)

    mov     rax, 10             ; sys_mprotect
    mov     rdx, 0x7            ; PROT_READ | PROT_WRITE | PROT_EXEC
    syscall

    ; Check if mprotect failed
    cmp     rax, 0
    jne     _exit

    jmp     _key

_decryption:
    pop     rsi                 ; Pointer to key

    ; Calculate address of encrypted section
    mov     rax, [rel offset]
    add     rax, [rel base_addr]

    xor     rcx, rcx            ; Counter
    xor     rdx, rdx            ; Key index

_decrypt_loop:
    cmp     rcx, [rel size]
    jge     _decrypt_end

    mov     bl, [rax + rcx]
    xor     bl, [rsi + rdx]
    mov     [rax + rcx], bl

    inc     rcx
    inc     rdx
    cmp     rdx, [rel key_size]
    jb      _decrypt_loop
    xor     rdx, rdx            ; Reset key index
    jmp     _decrypt_loop

_decrypt_end:
    pop     rdx                 ; Restore rdx
    pop     rdi                 ; Restore rdi

    ; Jump to original entry point
    mov     rax, [rel old_entry]
    add     rax, [rel base_addr]
    jmp     rax

_exit:
    mov     rax, 60             ; sys_exit
    xor     rdi, rdi
    syscall

_params:
    base_addr   dq 0x0          ; Base address
    offset      dq 0x0          ; Offset of encrypted section
    size        dq 0x0          ; Size of encrypted section
    old_entry   dq 0x0          ; Original entry point
    key_size    dq 0x0          ; Key size

_key:
    call    _decryption
    ; Key data will be appended here
