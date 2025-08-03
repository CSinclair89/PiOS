#![no_main]
#![no_std]
#![no_builtins]

mod rs_helloworld;
mod rs_mmu;

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! { loop{} }

// Expose the vector table

extern "C" { fn printp(msg: *const u8); }

#[no_mangle]
pub unsafe extern "C" fn install_exception_vector() {
    extern "C" { static exception_vector_start: u8; }
    core::arch::asm!(
        "msr VBAR_EL1, {0}",
        in(reg) &exception_vector_start as *const _ as u64,
        options(nostack, preserves_flags),
    );
}

#[no_mangle]
pub extern "C" fn rs_handle_sync_exception() {
    let msg = b"Entered exception handler!\n\0";
    unsafe { printp(msg.as_ptr()); }

    let esr_el1: u64;
    let far_el1: u64;

    unsafe {
        core::arch::asm!(
            "mrs {0}, esr_el1",
            "mrs {1}, far_el1",
            out(reg)esr_el1,
            out(reg)far_el1,
        );
    }

    loop{}
}
