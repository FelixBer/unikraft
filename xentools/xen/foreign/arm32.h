
/*
 * public xen defines and struct for arm32
 * generated by mkheader.py -- DO NOT EDIT
 */

#ifndef __FOREIGN_ARM32_H_TMP__
#define __FOREIGN_ARM32_H_TMP__ 1


#define __arm___ARM32 1
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
# define __DECL_REG(n64, n32) union { __align8__ uint64_t n64; uint32_t n32; }
# define __align8__ __attribute__((aligned (8)))
#else
# define __DECL_REG(n64, n32) __align8__ uint64_t n64
# define __align8__ FIXME
#endif

#define XEN_LEGACY_MAX_VCPUS_ARM32 1
#define _VGCF_online_ARM32                   0
#define VGCF_online_ARM32                    (1<<_VGCF_online_ARM32)
#define MAX_GUEST_CMDLINE_ARM32 1024

#define arm32_has_no_vcpu_cr_regs 1

#define arm32_has_no_vcpu_ar_regs 1

#define arm32_has_no_start_info 1

#define arm32_has_no_trap_info 1

#define arm32_has_no_cpu_user_regs 1

struct vcpu_guest_core_regs_arm32 {
    __DECL_REG(x0,           r0_usr);
    __DECL_REG(x1,           r1_usr);
    __DECL_REG(x2,           r2_usr);
    __DECL_REG(x3,           r3_usr);
    __DECL_REG(x4,           r4_usr);
    __DECL_REG(x5,           r5_usr);
    __DECL_REG(x6,           r6_usr);
    __DECL_REG(x7,           r7_usr);
    __DECL_REG(x8,           r8_usr);
    __DECL_REG(x9,           r9_usr);
    __DECL_REG(x10,          r10_usr);
    __DECL_REG(x11,          r11_usr);
    __DECL_REG(x12,          r12_usr);
    __DECL_REG(x13,          sp_usr);
    __DECL_REG(x14,          lr_usr);
    __DECL_REG(x15,          __unused_sp_hyp);
    __DECL_REG(x16,          lr_irq);
    __DECL_REG(x17,          sp_irq);
    __DECL_REG(x18,          lr_svc);
    __DECL_REG(x19,          sp_svc);
    __DECL_REG(x20,          lr_abt);
    __DECL_REG(x21,          sp_abt);
    __DECL_REG(x22,          lr_und);
    __DECL_REG(x23,          sp_und);
    __DECL_REG(x24,          r8_fiq);
    __DECL_REG(x25,          r9_fiq);
    __DECL_REG(x26,          r10_fiq);
    __DECL_REG(x27,          r11_fiq);
    __DECL_REG(x28,          r12_fiq);
    __DECL_REG(x29,          sp_fiq);
    __DECL_REG(x30,          lr_fiq);
    __DECL_REG(pc64,         pc32);             
    uint32_t cpsr;                              
    union {
        uint32_t spsr_el1;       
        uint32_t spsr_svc;       
    };
    uint32_t spsr_fiq, spsr_irq, spsr_und, spsr_abt;
    __align8__ uint64_t sp_el0;
    __align8__ uint64_t sp_el1, elr_el1;
};
typedef struct vcpu_guest_core_regs_arm32 vcpu_guest_core_regs_arm32_t;

struct vcpu_guest_context_arm32 {
    uint32_t flags;                         
    struct vcpu_guest_core_regs_arm32 user_regs;  
    __align8__ uint64_t sctlr;
    __align8__ uint64_t ttbcr, ttbr0, ttbr1;
};
typedef struct vcpu_guest_context_arm32 vcpu_guest_context_arm32_t;

struct arch_vcpu_info_arm32 {
};
typedef struct arch_vcpu_info_arm32 arch_vcpu_info_arm32_t;

struct vcpu_time_info_arm32 {
    uint32_t version;
    uint32_t pad0;
    __align8__ uint64_t tsc_timestamp;   
    __align8__ uint64_t system_time;     
    uint32_t tsc_to_system_mul;
    int8_t   tsc_shift;
#if __XEN_INTERFACE_VERSION__ > 0x040600
    uint8_t  flags;
    uint8_t  pad1[2];
#else
    int8_t   pad1[3];
#endif
};
typedef struct vcpu_time_info_arm32 vcpu_time_info_arm32_t;

struct vcpu_info_arm32 {
    uint8_t evtchn_upcall_pending;
#ifdef XEN_HAVE_PV_UPCALL_MASK
    uint8_t evtchn_upcall_mask;
#else 
    uint8_t pad0;
#endif 
    __align8__ uint64_t evtchn_pending_sel;
    struct arch_vcpu_info_arm32 arch;
    struct vcpu_time_info_arm32 time;
};
typedef struct vcpu_info_arm32 vcpu_info_arm32_t;

struct arch_shared_info_arm32 {
};
typedef struct arch_shared_info_arm32 arch_shared_info_arm32_t;

struct shared_info_arm32 {
    struct vcpu_info_arm32 vcpu_info[XEN_LEGACY_MAX_VCPUS_ARM32];
    __align8__ uint64_t evtchn_pending[sizeof(uint64_t) * 8];
    __align8__ uint64_t evtchn_mask[sizeof(uint64_t) * 8];
    uint32_t wc_version;      
    uint32_t wc_sec;
    uint32_t wc_nsec;
#if !defined(__i386___ARM32)
    uint32_t wc_sec_hi;
# define xen_wc_sec_hi wc_sec_hi
#elif !defined(__XEN__) && !defined(__XEN_TOOLS__)
# define xen_wc_sec_hi arch.wc_sec_hi
#endif
    struct arch_shared_info_arm32 arch;
};
typedef struct shared_info_arm32 shared_info_arm32_t;


#undef __DECL_REG

#endif /* __FOREIGN_ARM32_H_TMP__ */
