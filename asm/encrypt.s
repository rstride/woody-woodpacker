.global encrypt_binary_asm
encrypt_binary_asm:
    xor %rcx, %rcx        # Clear rcx, use as loop counter

loop_start:
    movb (%rdi, %rcx), %al # Load byte from [data + rcx]
    movb (%rsi, %rcx), %bl # Load byte from [key + (rcx % key_len)]
    xor %bl, %al           # XOR key byte with data byte
    movb %al, (%rdi, %rcx) # Store the result back into data
    inc %rcx               # Increment loop counter
    cmp %rcx, %rdx         # Compare counter with size
    jne loop_start         # Jump to loop_start if not done
    ret
