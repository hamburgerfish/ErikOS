ENTRY(_start)

SECTIONS {
	. = 0x1000;
	.text ALIGN(4K) : {
		*(.text)
	}

	.gap : {
		. = . + 1024;
	}

	.data ALIGN(4K) : {
		*(.data)
	}

	. = ALIGN(4K);
	.rodata ALIGN(4K) : {
		*(.rodata)
	}

	.bss ALIGN(4K) : {
		*(.bss)
	}
}
