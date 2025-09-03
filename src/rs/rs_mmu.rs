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

// Page Table Entry Flags
const PTE_VALID: u64 = 1 << 0;      // Sets bit 0 of a page table entry -- is this entry valid?
const PTE_TBL: u64 = 1 << 1;        // Sets bit 1 to indicate type of entry.
const PTE_AF: u64 = 1 << 10;        // Sets Access Flag (AF) bit.
const PTE_BLOCK: u64 = 0 << 1;
const PTE_SH_OUTER: u64 = 2 << 8;
const PTE_SH_INNER: u64 = 3 << 8;
const PTE_AP_RW_EL1: u64 = 0 << 6;
const PTE_ATTR_IDX_0: u64 = 0 << 2;
const PTE_ATTR_IDX_1: u64 = 1 << 2;
const PAGE_DESC_NORM: u64 = (PTE_VALID | PTE_AF | PTE_ATTR_IDX_1 | PTE_AP_RW_EL1 | PTE_SH_INNER);

// Execute-never Flags
const UXN: u64 = 1 << 54;
const PXN: u64 = 1 << 53;

// Memory Attribute Indirection Register Values
const MAIR_DEVICE_NGNRE: u64 = 0x04;
const MAIR_NORMAL: u64 = 0xff;

/*
 * Attribute Functions for Various Mappings
 */

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

#[no_mangle]
pub extern "C" fn rs_page_desc_norm() -> u64 { PAGE_DESC_NORM }

/*
 * Create Page Table Manipulation Functions
 */

#[no_mangle]
pub extern "C" fn rs_map_page(
    l1_tbl: *mut u64,
    vaddr: u64,
    paddr: u64,
    attrs: u64) {

    let l1_idx = (vaddr >> 30) & 0x1FF;     // Top-level index (L1 - bits [38:30])
    let l2_idx = (vaddr >> 21) & 0x1FF;     // Second-level index (L2 - bits [29:21])

    // Pointer to L1 entry
    let l1_entry = unsafe { l1_tbl.add(l1_idx as usize) };

    // Determine which L2 table to use (allocate if missing)
    let l2_tbl: *mut u64 = unsafe {

        // If L1 entry is invalid, allocate new L2 table
        if (*l1_entry & PTE_VALID) == 0 {
            
            // Allocate new page for L2 table and get it's paddr
            let l2_page = allocatePhysPages(1);
            let tbl_ptr = getPhysAddr(l2_page) as *mut u64;

            // Update L1 entry to point to new L2 table
            *l1_entry = (tbl_ptr as u64 & !0xFF) | PTE_VALID | PTE_TBL;

            // Return pointer to new L2 table
            tbl_ptr
    
        // Otherwise reuse existing L2 table that L1 aready points to
        } else {

            // ???
            (*l1_entry & !0xFFF) as *mut u64
        }
    };

    // Pointer to L2 entry corresponding to vaddr
    let l2_entry = unsafe { l2_tbl.add(l2_idx as usize) };

    unsafe { 
        
        // Encode physical page address into entry + attributes
        let mapped_val = paddr & !(0x1F_FFFF) | 0x705;

        // Write final mapping into L2 entry
        *l2_entry = mapped_val;
    };
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
pub extern "C" fn rs_init_mmu(l1_tbl: *const u64) {

    // Define register values for MAIR, TCR, and TTBR0
    let mair_val = (0xFF << 0) | (0x04 << 8) | (0x44 << 16);
    let tcr_val =
        (16 << 0) |         // T0SZ: 16 = 48-bit space
        (0b00 << 6) |       // IRGN0: Inner cacheable normal memory
        (0b00 << 8) |       // ORGN0: Outer cacheable normal memory
        (0b11 << 12) |      // SH0: Shareable memory type for TTBR0
        (0b0 << 14);        // TG0: Granule size for TTBR0, 0b0 = 4KB

    // Update registers w/ new values
    unsafe {
        core::arch::asm!("msr MAIR_EL1, {}", in(reg) mair_val);
        core::arch::asm!("msr TCR_EL1, {}", in(reg) tcr_val);
        core::arch::asm!("msr TTBR0_EL1, {}", in(reg) l1_tbl as u64);
    
        // Invalidate TLB
        core::arch::asm!("dsb ish");        // Data Sync Barrier
        core::arch::asm!("isb");            // Instructon Sync Barrier
        core::arch::asm!("tlbi vmalle1");   // Invalidate all TLB entries
        core::arch::asm!("dsb ish");        // Ensure completion of TLB invalidation
        core::arch::asm!("isb");            // Flush pipeline so changes are visible

        // Read SCTLR_EL1 value into sctlr var
        let mut sctlr: u64;
        core::arch::asm!("mrs {}, SCTLR_EL1", out(reg) sctlr);

        // Set mandatory/reserved bits
        sctlr |= 0xC00800;

        // Clear bits
        sctlr &= !(
            (1u64 << 25) |  // EE - Endianness of translation tables (0 = little endian)
            (1u64 << 24) |  // E0E - Endianness of EL0 data access (0 = little endian)
            (1u64 << 19) |  // WXN - Write XOR Execute (0 = allow writable + executable)
            (1u64 << 12) |  // I - Instruction cache enable (0 = disabled)
            (1u64 << 4) |   // SA0 - Stack Alignment Check for EL0 (0 = disabled)
            (1u64 << 3) |   // SA - Stack Alignment Check for EL1 (0 = disabled)
            (1u64 << 2) |   // C - Data Cache enable (0 = disabled)
            (1u64 << 1)     // A - Alignment Check enable (0 = disabled)
        );

        // Enable MMU
        sctlr |= 1u64 << 0;

        core::arch::asm!("msr SCTLR_EL1, {}", in(reg) sctlr);
        core::arch::asm!("isb");

        let mut new_sctlr: u64;
        core::arch::asm!("mrs {}, SCTLR_EL1", out(reg) new_sctlr);
        let mmu_msg = b"SCTLR_EL1 val: 0x%x\n\0" as *const u8;
        printp(mmu_msg, new_sctlr);
    };
}

#[no_mangle]
pub extern "C" fn call_print_paddr(paddr: *mut u8) {
    let addr = paddr as u64;
    let fmt = b"Rust test: paddr = 0x%llx\n\0".as_ptr();
    unsafe { printp(fmt, addr); }
}
