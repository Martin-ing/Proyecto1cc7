ENTRY(_start)

MEMORY 
{
    os_ram : ORIGIN = 0x00010000, LENGTH = 64K
    p1_ram : ORIGIN  = 0x00060000, LENGTH = 64K
    p1_stack : ORIGIN = 0x00070000, LENGTH = 4K
    p2_ram : ORIGIN = 0x00080000, LENGTH = 64K
    p2_stack : ORIGIN = 0x00090000, LENGTH = 4K
}

SECTIONS
{
    . = ORIGIN(os_ram);

    .text :
    {
        *(.text.vector_table) 
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
    _stack_top = ORIGIN(os_ram) + LENGTH(os_ram);

    .p1_text :
    {
        *(.p1_text*)
        *(.p1_rodata*)
    } > p1_ram

    .p1_bss :
    {
        *p1*(.bss*)
    } > p1_ram


    __p1_stack_base__ = ORIGIN(p1_stack);
    __p1_stack_top__  = ORIGIN(p1_stack) + LENGTH(p1_stack);


    .p2_text :
    {
        *(.p2_text*)
        *(.p2_rodata*)
    } > p2_ram

    .p2_bss :
    {
        *p2*(.bss*)
    } > p2_ram


    __p2_stack_base__ = ORIGIN(p2_stack);
    __p2_stack_top__  = ORIGIN(p2_stack) + LENGTH(p2_stack);

}
