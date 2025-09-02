.section .vectors, "a"
.align 11
.global exception_vector_start

exception_vector_start:
	// EL1t
	adr x0, rs_handle_sync_exception	// Synchronous
	br x0
	adr x0, rs_handle_sync_exception	// IRQ
	br x0	
	adr x0, rs_handle_sync_exception	// FIQ
	br x0
	adr x0, rs_handle_sync_exception	// SError
	br x0
	// EL1h
	adr x0, rs_handle_sync_exception	// Synchronous
	br x0
	adr x0, rs_handle_sync_exception	// IRQ
	br x0
	adr x0, rs_handle_sync_exception	// FIQ
	br x0
	adr x0, rs_handle_sync_exception	// SError
	br x0
	// EL0 64
	adr x0, rs_handle_sync_exception	// Synchronous
	br x0
	adr x0, rs_handle_sync_exception	// IRQ
	br x0
	adr x0, rs_handle_sync_exception	// FIQ
	br x0
	adr x0, rs_handle_sync_exception	// SError
	br x0
	// EL0 32
	adr x0, rs_handle_sync_exception	// Synchronous
	br x0
	adr x0, rs_handle_sync_exception	// IRQ
	br x0
	adr x0, rs_handle_sync_exception	// FIQ
	br x0
	adr x0, rs_handle_sync_exception	// SError
	br x0
/*
.section .text.vectors, "ax"
.align 11 // 2kb = 2^11
.global _exception_vectors
.type _exception_vectors, %function

_exception_vectors:
	// EL1t: Synchronous exceptions
	// Offset: 0x000
	b rs_handle_sync_exception
	
	// EL1t: IRQ
	// Offset: 0x080
	b .

	// EL1t: FIQ
	// Offset: 0x100
	b .

	// EL1t: SError
	// Offset: 0x180
	b .

	// -----------------
	// EL1h: Synchronous exceptions
	// Offset: 0x200 
	b rs_handle_sync_exception

	// EL1h: IRQ
	// Offset: 0x280
	b .

	// EL1h: FIQ
	// Offset: 0x300
	b .

	// EL1h: SError
	// Offset: 0x380
	b .

	// ------------------
	// EL0: Synchronous, IRQ, FIQ, SError (AArch64)
	// Offsets: 0x400 - 0x580
	.rept 4
	b .
	.endr

	// EL0: Synchronouss, IRQ, FIQ, SError (AArch32)
	// Offsets: 0x600 - 0x680
	.rept 4
	b .
	.endr	
*/
//.align 11

