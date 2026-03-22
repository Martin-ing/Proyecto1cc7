ENTRY(_start)

MEMORY
{
    os_ram   : ORIGIN = 0x82000000, LENGTH = 256K

    p1_ram   : ORIGIN = 0x82100000, LENGTH = 64K
    p1_stack : ORIGIN = 0x82110000, LENGTH = 4K

    p2_ram   : ORIGIN = 0x82200000, LENGTH = 64K
    p2_stack : ORIGIN = 0x82210000, LENGTH = 4K
}

SECTIONS
{
    /* ====================================================== */
    /* OS */
    /* ====================================================== */
    . = ORIGIN(os_ram);

    .text :
    {
        *(.text*)
        *(.rodata*)
    } > os_ram

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > os_ram

    .bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > os_ram

    _stack_bottom = __bss_end__;
    _stack_top    = ORIGIN(os_ram) + LENGTH(os_ram);

    /* ====================================================== */
    /* P1 */
    /* ====================================================== */
    . = ORIGIN(p1_ram);

    .p1_text :
    {
        __p1_start__ = .;
        KEEP(*(.p1_text))
        KEEP(*(.p1_rodata))
        KEEP(*(.p1_data))
        KEEP(*(.p1_bss))
        __p1_end__ = .;
    } > p1_ram

    __p1_stack_base__ = ORIGIN(p1_stack);
    __p1_stack_top__  = ORIGIN(p1_stack) + LENGTH(p1_stack);

    /* ====================================================== */
    /* P2 */
    /* ====================================================== */
    . = ORIGIN(p2_ram);

    .p2_text :
    {
        __p2_start__ = .;
        KEEP(*(.p2_text))
        KEEP(*(.p2_rodata))
        KEEP(*(.p2_data))
        KEEP(*(.p2_bss))
        __p2_end__ = .;
    } > p2_ram

    __p2_stack_base__ = ORIGIN(p2_stack);
    __p2_stack_top__  = ORIGIN(p2_stack) + LENGTH(p2_stack);
}