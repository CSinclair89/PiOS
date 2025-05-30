extern "C" { // We're pulling an external function name printp()
    fn printp(msg: *const u8);  // point to the memory address of 'msg'
}

#[no_mangle] // do not mangle method header -- needs to be callable from C
pub extern "C" fn rs_helloWorld() { // header for public external method
    let msg = b"Hello from Rust!\0"; // local variable -- new memory address.
                                     // b stands for NOT SURE YET
                                     // Don't forget Rust requires null terminator.
    unsafe { printp(msg.as_ptr()); } // 'unsafe' aka we're working with C.
                                     // make the method call using .as_ptr() core func
}
