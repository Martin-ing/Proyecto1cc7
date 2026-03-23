ENTRY(vector_table)

MEMORY
{
    ram : ORIGIN = 0x00010000, LENGTH = 256K
    p1_ram : ORIGIN = 0x00060000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(ram);

    .text :
    {
        *(.text.vector_table) 
        *(.text*)
        *(.rodata*)
    } > ram

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > ram

    .bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > ram

    _stack_bottom = __bss_end__;
    _stack_top = ORIGIN(ram) + LENGTH(ram);

    .p1_text :
    {
        *(.p1_text*)
        *(.p1_rodata*)
    } > p1_ram

    .p1_bss :
    {
        *p1*(.bss*)
    } > p1_ram
}
