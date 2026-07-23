SECTIONS
{
    .cmd_list : {
        . = ALIGN(8);
        __CMD_TABLE_START__ = .;
        KEEP(*(.cmd_list*))
        __AUTO_TABLE_START__ = .;
        KEEP(*(.auto_list*))
        KEEP(*(.cmd_end*))
    } > FLASH
}
