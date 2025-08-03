#[no_mangle]
#[link_section = ".vectors"]
pub static EXCEPTION_VEC_TBL: [u32; 512] = [
    0; 512
];
