/*
 * Set Up Interface Between C and Rust
 */

extern "C" {
    pub fn allocatePhysPages(npages: u32) -> *mut PPage;
    pub fn getPhysAddr(page: *mut PPage) -> *mut u8;
    pub fn printp(fmt: *const u8, ...) -> i32;
}

#[repr(C)]
pub struct PPage { _unused: [u8; 0] }

/*
 * Define Page Table Entry Structure
 */

// Sets bit 0 of a page table entry -- is this entry valid?
const PTE_VALID: u64 = 1 << 0;

// Sets bit 1 to indicate type of entry.
const PTE_TBL: u64 = 1 << 1;

// Sets Access Flag (AF) bit.
const PTE_AF: u64 = 1 << 10;

// Sets bit 1 to 0, defining entry as a block entry.
// Mainly for clarity when forming entries.
// const PTE_BLOCK: u64 = 0;
const PTE_SH_INNER: u64 = 0b11 << 8;
// const PTE_AP_RW: u64 = 0 << 6;
const PTE_AP_RW_EL1: u64 = 0b00 << 6;
const PTE_ATTR_IDX_0: u64 = 0 << 2;
const UXN: u64 = 1 << 54;
const PXN: u64 = 1 << 53;

// Memory Attribute Indirection Register (MAIR)
// Non-Gathering, non-Reordering, Early Write Acknowledgements (nGnRE)
// const MAIR_DEVICE_NGNRE: u64 = 0x04;
// const MAIR_NORMAL: u64 = 0xff;

#[no_mangle]
pub extern "C" fn map_page_attrs() -> u64 {
    const ATTR_IDX_NORMAL: u64 = 0 << 2;
    ATTR_IDX_NORMAL | UXN | PXN
}

#[no_mangle]
pub extern "C" fn map_device_attrs() -> u64 {
    const ATTR_IDX_DEVICE: u64 = 0x4 << 2;
    ATTR_IDX_DEVICE | UXN | PXN
}

/*
 * Create Page Table Manipulation Functions
 */

#[no_mangle]
pub extern "C" fn map_page(
    l1_tbl: *mut u64, // L1 page table
    vaddr: u64,
    paddr: u64,
    attrs: u64
) {
    let l1_idx = (vaddr >> 30) & 0x1FF;
    let l2_idx = (vaddr >> 21) & 0x1FF;

    //debug
    let msg0 = b"== map_page() start ==\n\0".as_ptr();
    unsafe { printp(msg0); }

    // debug
    let msg1 = b"l1_idx = 0x%x, l2_idx = 0x%x\n\0".as_ptr();
    unsafe { printp(msg1, l1_idx as u32, l2_idx as u32); }
    
    // Get L2 table from L1 entry
    let l1_entry = unsafe { l1_tbl.add(l1_idx as usize) };

    let l2_tbl: *mut u64 = unsafe {
        if (*l1_entry & PTE_VALID) == 0 {

            // debug
            let msg2 = b"L1 entry not valid, allocating L2 table...\n\0".as_ptr();
            printp(msg2);

            let l2_page = allocatePhysPages(1); 
            let l2_page_addr = l2_page as u64;

            // debug
            let msg3_l2_upper = (l2_page_addr >> 32) as u32;
            let msg3_l2_lower = (l2_page_addr & 0xFFFF_FFFF) as u32;
            let msg3 = b"l2_page = 0x%x%08x\n\0".as_ptr();
            printp(msg3, msg3_l2_upper, msg3_l2_lower);

            let tbl_ptr = getPhysAddr(l2_page) as *mut u64;
            let tbl_addr = tbl_ptr as u64;
            
            // debug
            let msg4 = b"l2_tbl = 0x%llx (from getPhysAddr)\n\0".as_ptr();
            printp(msg4, tbl_addr);

            *l1_entry = (tbl_ptr as u64) | PTE_VALID | PTE_TBL;
            
            // debug
            let msg5 = b"L1 entry updated\n\0".as_ptr();
            printp(msg5);

            tbl_ptr
        } else {

            // debug
            let msg6 = b"L1 entry already valid, using existing L2 table\n\0".as_ptr();
            printp(msg6);

            (*l1_entry & !0xFFF) as *mut u64
        }
    };

    // Set block entry in L2 for 2MB mapping
    unsafe {
        let l2_entry = l2_tbl.add(l2_idx as usize);
        let mapped_val = (paddr & !(0x1F_FFFF))
            | PTE_VALID 
            | PTE_AF
            | PTE_ATTR_IDX_0
            | PTE_AP_RW_EL1
            | PTE_SH_INNER
            | UXN
            | PXN;

        // debug
        let msg7_mapped_val_upper = (mapped_val >> 32) as u32;
        let msg7_mapped_val_lower = (mapped_val & 0xFFFF_FFFF) as u32;
        let msg7 = b"Writing L2[0x%x] = 0x%x%08x\n\0".as_ptr();
        printp(msg7, l2_idx as u32, msg7_mapped_val_upper, msg7_mapped_val_lower);

        *l2_entry = mapped_val;
    }

    // debug
    let msg_done = b"==completed map_page()==\n\0".as_ptr();
    unsafe { printp(msg_done); }
}

/*
 * Set System Registers in Rust & Boot-Time Setup
 */

/*
 * ARM64 processors expose system registers to control memory behavior:
 * - MAIR_EL1 - Memory Attribute Indirectional Register
 *      - Defines memory types used in page table entries
 *      - Page w/ attribute index of 0 is treated as device memory
 *      - Page w/ attribute index of 1 is treated as normal, cacheable memory
 * - TCR_EL1 - Translation Control Register
 *      - Configures vaddr-paddr translation and size
 * - TTBR0_EL1 - Translation Table Base Register 0
 *      - Holds base paddr for L1 page table
 *      - Works in tandem w/ TCR_EL1 and MMU to perform translations
 * - SCTLR_EL1 - System Control Register
 *      - Controls MMU enable/disable, cache behavior, alignment checks
 *
 * - EL1 stands for Exception Level 1 aka kernel level
 */


/*
#[no_mangle]
#[inline(never)]
unsafe fn init_mmu() {
    
    // Allocate L1 table
    let l1_page = allocatePhysPages(1);
    if l1_page.is_null() { loop {} }
    let tbl_addr = getPhysAddr(l1_page) as u64;


    // identity map a vaddr to a paddr
 
    map_page(
        tbl_addr as *mut u64,
        0x0000_0000,
        0x0000_0000,
        PTE_VALID | PTE_AF
    );

    // define register values
    let mair: u64 = (MAIR_DEVICE_NGNRE << 0) | (MAIR_NORMAL << 8);
    let tcr: u64 = (0b10010000 << 0) | (0b00 << 6) | (0b00 << 8);

    // set memory registers with respective values
    core::arch::asm!("msr MAIR_EL1, {}", in(reg) mair);
    core::arch::asm!("msr TCR_EL1, {}", in(reg) tcr);
    core::arch::asm!("msr TTBR0_EL1, {}", in(reg) tbl_addr);

    // Data Synchronization Barrier
    // - Ensures all memory operations are complete before moving on
    // - 'sy' stands for system, meaning the full scope of the system
    core::arch::asm!("dsb sy");

    // Instruction Synchronization Barrier
    // - Flushes CPU instruction pipeline before enabling MMU
    core::arch::asm!("isb");

    // enable MMU via system control register
    let mut _sctlr_el1: u64;
    core::arch::asm!(
        "mrs {val}, SCTLR_EL1",
        "orr {val}, {val}, {bit:x}",
        "msr SCTLR_EL1, {val}",
        val = out(reg) _,
        bit = in(reg) 1
    );

    // Good practice to follow with another flush after writing to system control register
    core::arch::asm!("isb");

}
*/
/*
#[no_mangle]
pub extern "C" fn init_mmu(l1_tbl: *mut u64) {
    if l1_tbl.is_null() { loop {} }

    let vaddr: u64 = 0x4000_0000;
    let paddr: u64 = 0x3F20_0000;
    let attrs: u64 = map_page_attrs();
    map_page(l1_tbl, vaddr, paddr, attrs);

    let mair: u64 = (MAIR_DEVICE_NGNRE << 0) | (MAIR_NORMAL << 8);
    let tcr: u64 = (16 << 0) | (16 << 16); 

    unsafe {
        core::arch::asm!("msr MAIR_EL1, {}", in(reg) mair);
        core::arch::asm!("msr TCR_EL1, {}", in(reg) tcr);
        core::arch::asm!("msr TTBR0_EL1, {}", in(reg) l1_tbl as u64);

        core::arch::asm!("dsb sy");
        core::arch::asm!("isb");

        let mut sctlr_el1: u64;
        core::arch::asm!(
            "mrs {val}, SCTLR_EL1",
            "orr {val}, {val}, {bits}",
            "msr SCTLR_EL1, {val}",
            val = out(reg) sctlr_el1,
            bits = in(reg) (1 << 0) | (1 << 2) | (1 << 12)
        );
        core::arch::asm!("isb");
    }
}
*/


use core::ffi::c_char;

#[no_mangle]
pub extern "C" fn init_mmu(l1_tbl: *const u64) {
    if l1_tbl.is_null() { loop{} }
    unsafe {
        
        let mair_val: u64 = 0x00FF;
        core::arch::asm!(
            "msr MAIR_EL1, {}", 
            in(reg) mair_val,
            options(nostack, preserves_flags)
            );

        let tcr_val: u64 = 0b100100000101 | (16 << 16);
        core::arch::asm!("msr TCR_EL1, {}", in(reg) tcr_val);
        core::arch::asm!("msr TTBR0_EL1, {}", in(reg) l1_tbl as u64);

        core::arch::asm!("dsb ish");
        core::arch::asm!("isb");

        let mut sctlr_el1: u64;
        core::arch::asm!(
            "mrs {val}, SCTLR_EL1",
            "orr {val}, {val}, {m_bit}",
            "orr {val}, {val}, {c_bit}",
            "orr {val}, {val}, {i_bit}",
            "msr SCTLR_EL1, {val}",
            "isb",
            val = out(reg) sctlr_el1,
            m_bit = const 1 << 0,
            c_bit = const 1 << 2,
            i_bit = const 1 << 12,
        );

        let mut new_sctlr: u64;
        core::arch::asm!("mrs {}, SCTLR_EL1", out(reg) new_sctlr);

        let fmt = b"SCTLR_EL1 = 0x%llx\n\0" as *const u8 as *const c_char;

        printp(fmt, new_sctlr);
    }
}

#[no_mangle]
pub extern "C" fn call_print_paddr(paddr: *mut u8) {
    let addr = paddr as u64;
    let fmt = b"Rust test: paddr = 0x%llx\n\0".as_ptr();
    unsafe { printp(fmt, addr); }
}
