/*
 Sample code showing Checking the cpuid and XSAVE information for AMX support

 To Build: g++ -o amx-cpuid amx-cpuid

 Note: there are OS specific ways to obtain cpuid and xgetbv.
 for example cpuid.h on linux and xsaveintrin.h in the intrensics library.

 This code is for demonstration and has only been tested on linux.

 The Intel 64 and IA-32 Architectures Software Developer's Manual was referenced
 https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4

 Specifically:
 Chapter 13.1 XSAVE-Supported Features and State-component Bitmaps (pages 319-321)
 Table 3-8. Information Returned by CPUID Instruction (pages 814-835)
*/

#include <iostream>

// Define the XCR0 register index
#define XCR0_INDEX 0

#define XFEATURE_XTILECFG 17
#define XFEATURE_XTILEDATA 18
#define XFEATURE_MASK_XTILECFG (1 << XFEATURE_XTILECFG)
#define XFEATURE_MASK_XTILEDATA (1 << XFEATURE_XTILEDATA)
#define XFEATURE_MASK_XTILE (XFEATURE_MASK_XTILECFG | XFEATURE_MASK_XTILEDATA)


#define CPUID_AMX_BF16 22 // leaf 7 sub-leaf 0, edx
#define CPUID_AMX_TILE 24 // leaf 7 sub-leaf 0, edx
#define CPUID_AMX_INT8 25 // leaf 7 sub-leaf 0, edx
#define CPUID_MASK_AMX_BF16 (1 << CPUID_AMX_BF16)
#define CPUID_MASK_AMX_TILE (1 << CPUID_AMX_TILE)
#define CPUID_MASK_AMX_INT8 (1 << CPUID_AMX_INT8)

#define CPUID_AMX_FP16 21 // leaf 7 sub-leaf 1, eax
#define CPUID_AMX_COMPLEX 8 // leaf 7 sub-leaf 1, edx
#define CPUID_MASK_AMX_FP16 (1 << CPUID_AMX_FP16)
#define CPUID_MASK_AMX_COMPLEX (1 << CPUID_AMX_COMPLEX)

void cpuid(uint32_t leaf, uint32_t subleaf, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ volatile("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(subleaf));
}

uint64_t xgetbv(uint32_t index) {
    uint32_t eax, edx;
    __asm__ volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
    return ((uint64_t)edx << 32) | eax;
}

int main() {
    uint32_t eax, ebx, ecx, edx;

    // Get leaf 7 of the cpuid
    cpuid(7, 0, &eax, &ebx, &ecx, &edx);
    bool amx_tile_supported = (edx & CPUID_MASK_AMX_TILE) != 0;
    if (amx_tile_supported) {
        std::cout << "CPUID reports AMX-TILE support.\n";
    }
    bool amx_bf16_supported = (edx & CPUID_MASK_AMX_BF16) != 0;
    if (amx_bf16_supported) {
        std::cout << "CPUID reports AMX-BF16 support.\n";
    }
    bool amx_int8_supported = (edx & CPUID_MASK_AMX_INT8) != 0;
    if (amx_int8_supported) {
        std::cout << "CPUID reports AMX-INT8 support.\n";
    }

    // Get leaf 7 subleaf 1 of cpuid
    cpuid(7, 1, &eax, &ebx, &ecx, &edx);
    bool amx_fp16_supported = (eax & CPUID_MASK_AMX_FP16) != 0;
    if (amx_fp16_supported) {
        std::cout << "CPUID reports AMX-FP16 support.\n";
    }
    bool amx_complex_supported = (edx & CPUID_MASK_AMX_COMPLEX) != 0;
    if (amx_complex_supported) {
        std::cout << "CPUID reports AMX-COMPLEX support.\n";
    }

    uint64_t xcr0 = xgetbv(XCR0_INDEX);
    bool xsave_shows_amx_support = (xcr0 & XFEATURE_MASK_XTILE) == XFEATURE_MASK_XTILE;
    if (xsave_shows_amx_support) {
        std::cout << "The TILECFG and TILEDATA bits are set in the Extended Control Register 0 (XCR0).\n";
    }

}