OUTPUT_FORMAT(binary)
SECTIONS
{
	kernel 0x7e010 : {
		*(.text)
		*(.rodata)
		*(.data)
		*(.bss)
	}
}
