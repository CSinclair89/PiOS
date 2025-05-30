#![no_main]
#![no_std]
#![no_builtins]

mod rs_helloworld;
mod rs_mmu;

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! { loop{} }
