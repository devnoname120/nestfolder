    .arch armv7a
    .section .vitalink.fstubs.taihen,"ax",%progbits
    .align 4
    .global taiGetModuleInfo
    .type taiGetModuleInfo, %function
taiGetModuleInfo:
    .word 0x00000000
    .word 0x203D1A28
    .word 0xA5899384
    .align 4
    .global taiHookFunctionOffsetForUser
    .type taiHookFunctionOffsetForUser, %function
taiHookFunctionOffsetForUser:
    .word 0x00000000
    .word 0x203D1A28
    .word 0x0535225A
    .align 4
    .global taiHookRelease
    .type taiHookRelease, %function
taiHookRelease:
    .word 0x00000000
    .word 0x203D1A28
    .word 0x1C30E799
    .align 4
    .global taiInjectDataForUser
    .type taiInjectDataForUser, %function
taiInjectDataForUser:
    .word 0x00000000
    .word 0x203D1A28
    .word 0xCE25B9BC
    .align 4
    .global taiInjectRelease
    .type taiInjectRelease, %function
taiInjectRelease:
    .word 0x00000000
    .word 0x203D1A28
    .word 0x21A4C8A6
    .align 4
