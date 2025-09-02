ENTRY(_start)
MEMORY
{
	/* Default memory regions with appropriate attributes */
	TEXT (rw)  : ORIGIN = 0x80000, LENGTH = 1M /* Read + Execute */
	DATA (rw)  : ORIGIN = 0x100000, LENGTH = 1M /* Read + Write */
	BSS  (rw)  : ORIGIN = 0x120000, LENGTH = 1M /* Read + Write */
}
SECTIONS
{
	/* Text section: RX */
	. = 0x80000;
	.text.boot : {
		KEEP(*(.text.boot))
	} > TEXT

	. = 0x8100;
	.vectors : ALIGN(0x800) {
		KEEP(*(.vectors))
	} > TEXT

	.text : {
		*(.text)
	} > TEXT
	/*
	.text : {
		KEEP(*(.text.boot))
		*(.vectors)
		*(.text)
	} > TEXT
	*/
	__text_start = ADDR(.text.boot);
	__text_end = __text_start + SIZEOF(.text);

	/* Read-only data section: RX */
	.rodata : {
		*(.rodata)
	} > TEXT
	__rodata_start = ADDR(.rodata);
	__rodata_end = __rodata_start + SIZEOF(.rodata);

	/* Data section: RW */
	.data : {
		*(.data)
	} > DATA
	__data_start = ADDR(.data);
	__data_end = __data_start + SIZEOF(.data);

	/* BSS section: RW */
	.bss : {
		bss = .;
		*(.bss)
	} > BSS
	__bss_start = ADDR(.bss);
	__bss_end = __bss_start + SIZEOF(.bss);
	__bss_size = __bss_end - __bss_start;

	/* End of kernel */
	__end = .;
}
