OUTPUT_FORMAT("binary")
ENTRY(_start)
SECTIONS
{
	.text 0x7e000 : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}
