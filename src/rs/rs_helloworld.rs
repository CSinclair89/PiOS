#![no_main]
#![no_std]

extern "C" { fn printp(msg: *const u8); }

#[no_mangle]
pub extern "C" fn rs_helloWorld() {
    let msg = b"Hello from Rust!\0";
    unsafe { printp(msg.as_ptr()); }
}

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! { loop{} }
