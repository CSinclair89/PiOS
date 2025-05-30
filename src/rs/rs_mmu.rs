/*
 * Set Up Interface Between C and Rust
 */

extern "C" {
    pub fn allocatePhysPages(npages: u32) -> *mut PPage;
}

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
const PTE_BLOCK: u64 = 0;

// Memory Attribute Indirection Register (MAIR)
// Non-Gathering, non-Reordering, Early Write Acknowledgements (nGnRE)
const MAIR_DEVICE_NGNRE: u64 = 0x00;
const MAIR_NORMAL: u64 = 0xff;

/*
 * Create Page Table Manipulation Functions
 */

pub unsafe fn map_page(
    l1_tbl: *mut u64, // L1 page table
    vaddr: u64,
    paddr: u64,
    attrs: u64
) {
    let l1_idx = (vaddr >> 30) & 0x1FF;
    let l2_idx = (vaddr >> 21) & 0x1FF;

    // Get L2 table from L1 entry
    let l1_entry = l1_tbl.add(l1_idx as usize);

    let l2_tbl: *mut u64;
    if *l1_entry & PTE_VALID == 0 {
        let l2_page = allocatePhysPages(1);
        l2_tbl = (*l2_page).phys_addr as *mut u64;
        *l1_entry = (l2_tbl as u64) | PTE_VALID | PTE_TBL;
    } else {
        l2_tbl = (*l1_entry & !0xFFF) as *mut u64;
    }

    // Set block entry in L2 for 2MB mapping
    let l2_entry = l2_tbl.add(l2_idx as usize);
    *l2_entry = (paddr & !(0x1F_FFFF)) | PTE_VALID | PTE_AF | PTE_BLOCK | attrs;
}

/*
 * Create Rust-safe Wrapper for C function: allocatePhysPages()
 */

#[repr(C)]
pub struct PPage {
    pub next: *mut PPage,
    pub prev: *mut PPage,
    pub phys_addr: *mut u8
}

use core::ptr::NonNull;

pub struct PhysPage { ptr: NonNull<PPage> }

impl PhysPage {
    
    pub unsafe fn from_raw(ptr: *mut PPage) -> Option<Self> {
        NonNull::new(ptr).map(|nn| PhysPage {ptr: nn })
    }    

    pub fn phys_addr(&self) -> *mut u8 {
        unsafe { self.ptr.as_ref().phys_addr }
    }
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


#[no_mangle]
#[inline(never)]
unsafe fn init_mmu() {
    
    // Allocate L1 table
    let raw_ptr = allocatePhysPages(1);
    let l1_tbl = match PhysPage::from_raw(raw_ptr) {
        Some(page) => page,
        None => loop {}
    };
    let tbl_addr = l1_tbl.phys_addr() as u64;


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

