#pragma once
#include <cstdint>
#include <set>
#include <vector>

// NVMe command status values (command-specific SCT=1h, simplified)
enum class NvmeStatus : uint8_t {
    Success                       = 0x00,
    ConflictingAttributes         = 0x80,
    InvalidProtectionInfo         = 0x81,
    WriteToReadOnlyRange          = 0x82,
    CommandSizeLimitExceeded      = 0x83,
    CompareFailure                = 0x84,
    IncompatibleNamespaceOrFormat = 0x85,
    FastCopyNotPossible           = 0x86,
    OverlappingIORange            = 0x87,
    InsufficientResources         = 0x89,
    NamespaceNotReachable         = 0x8A,
};

// Dataset Management field (CDW13[7:0], when CETYPE=0h)
struct NvmeDsm {
    uint8_t access_freq    : 4;  // bits 3:0 - AF
    uint8_t access_latency : 2;  // bits 5:4 - AL
    uint8_t sequential_req : 1;  // bit 6    - SEQREQ
    uint8_t incompressible : 1;  // bit 7    - INCPRS
};
static_assert(sizeof(NvmeDsm) == 1);

// NVMe Read command parameters (NVM Command Set Spec §3.3.4)
struct NvmeReadCommand {
    uint64_t mptr;   // Metadata Pointer (MPTR)
    uint64_t prp1;   // PRP Entry 1 – data destination
    uint64_t prp2;   // PRP Entry 2
    uint64_t elbtu;  // Expected LBA Tags Upper (CDW2/3 bits 47:0)
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  cetype; // Command Extension Type (CDW12[19:16])
    uint8_t  prinfo; // Protection Information (CDW12[29:26])
    bool     stc;    // Storage Tag Check (CDW12[24])
    bool     fua;    // Force Unit Access (CDW12[30])
    bool     lr;     // Limited Retry (CDW12[31])
    NvmeDsm  dsm;    // Dataset Management (CDW13[7:0], cetype=0)
    uint16_t cev;    // Command Extension Value (CDW13[15:0], cetype!=0)
    uint32_t elbtl;  // Expected LBA Tags Lower (CDW14)
    uint16_t elbat;  // Expected LBA App Tag (CDW15[15:0])
    uint16_t elbatm; // Expected LBA App Tag Mask (CDW15[31:16])
};

// NVMe Write command parameters (NVM Command Set Spec §3.3.6)
struct NvmeWriteCommand {
    uint64_t mptr;   // Metadata Pointer (MPTR)
    uint64_t prp1;   // PRP Entry 1 – data source
    uint64_t prp2;   // PRP Entry 2
    uint64_t lbtu;   // Logical Block Tags Upper (CDW2/3 bits 47:0)
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  cetype; // Command Extension Type (CDW12[19:16])
    uint8_t  dtype;  // Directive Type (CDW12[23:20])
    uint8_t  prinfo; // Protection Information (CDW12[29:26])
    bool     stc;    // Storage Tag Check (CDW12[24])
    bool     fua;    // Force Unit Access (CDW12[30])
    bool     lr;     // Limited Retry (CDW12[31])
    NvmeDsm  dsm;    // Dataset Management (CDW13[7:0], cetype=0)
    uint16_t dspec;  // Directive Specific (CDW13[31:16])
    uint16_t cev;    // Command Extension Value (CDW13[15:0], cetype!=0)
    uint32_t lbtl;   // Logical Block Tags Lower (CDW14)
    uint16_t lbat;   // Logical Block App Tag (CDW15[15:0])
    uint16_t lbatm;  // Logical Block App Tag Mask (CDW15[31:16])
};

// NVMe Compare command parameters (NVM Command Set Spec §3.3.1)
struct NvmeCompareCommand {
    uint64_t mptr;   // Metadata Pointer (MPTR)
    uint64_t prp1;   // PRP Entry 1 – comparison data buffer
    uint64_t prp2;   // PRP Entry 2
    uint64_t elbtu;  // Expected LBA Tags Upper (CDW2/3 bits 47:0)
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  cetype; // Command Extension Type (CDW12[19:16])
    uint8_t  prinfo; // Protection Information (CDW12[29:26]), PRACT shall be 0
    bool     stc;    // Storage Tag Check (CDW12[24])
    bool     fua;    // Force Unit Access (CDW12[30])
    bool     lr;     // Limited Retry (CDW12[31])
    uint16_t cev;    // Command Extension Value (CDW13[15:0], cetype!=0)
    uint32_t elbtl;  // Expected LBA Tags Lower (CDW14)
    uint16_t elbat;  // Expected LBA App Tag (CDW15[15:0])
    uint16_t elbatm; // Expected LBA App Tag Mask (CDW15[31:16])
};

// NVMe Verify command parameters (NVM Command Set Spec §3.3.5)
struct NvmeVerifyCommand {
    uint64_t elbtu;  // Expected LBA Tags Upper (CDW2/3 bits 47:0)
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  cetype; // Command Extension Type (CDW12[19:16])
    uint8_t  prinfo; // Protection Information (CDW12[29:26]), PRACT shall be 0
    bool     stc;    // Storage Tag Check (CDW12[24])
    bool     fua;    // Force Unit Access (CDW12[30])
    bool     lr;     // Limited Retry (CDW12[31])
    uint16_t cev;    // Command Extension Value (CDW13[15:0], cetype!=0)
    uint32_t elbtl;  // Expected LBA Tags Lower (CDW14)
    uint16_t elbat;  // Expected LBA App Tag (CDW15[15:0])
    uint16_t elbatm; // Expected LBA App Tag Mask (CDW15[31:16])
};

// NVMe Write Uncorrectable command parameters (NVM Command Set Spec §3.3.7)
struct NvmeWriteUncorrectableCommand {
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  dtype;  // Directive Type (CDW12[23:20])
    uint16_t dspec;  // Directive Specific (CDW13[31:16])
};

// Dataset Management range entry (NVM Command Set Spec §3.3.3)
struct NvmeDsmRange {
    uint32_t cattr;  // Context Attributes
    uint32_t llb;    // Length in Logical Blocks (1-based)
    uint64_t slba;   // Starting LBA
};

// NVMe Dataset Management command parameters (NVM Command Set Spec §3.3.3)
struct NvmeDatasetManagementCommand {
    uint64_t prp1;   // PRP Entry 1 – range list data
    uint64_t prp2;   // PRP Entry 2
    uint8_t  nr;     // Number of Ranges, 0's based (CDW10[7:0])
    bool     ad;     // Attribute – Deallocate (CDW11[2])
    bool     idw;    // Attribute – Integral Dataset for Write (CDW11[1])
    bool     idr;    // Attribute – Integral Dataset for Read (CDW11[0])
    std::vector<NvmeDsmRange> ranges;
};

// NVMe Write Zeroes command parameters (NVM Command Set Spec §3.3.8)
struct NvmeWriteZeroesCommand {
    uint64_t lbtu;   // Logical Block Tags Upper (CDW2/3 bits 47:0)
    uint64_t slba;   // Starting LBA (CDW10/11)
    uint16_t nlb;    // Number of Logical Blocks, 0's based (CDW12[15:0])
    uint8_t  cetype; // Command Extension Type (CDW12[19:16])
    uint8_t  dtype;  // Directive Type (CDW12[22:20])
    uint8_t  prinfo; // Protection Information (CDW12[29:26])
    bool     stc;    // Storage Tag Check (CDW12[24]) — shall be 0
    bool     fua;    // Force Unit Access (CDW12[30])
    bool     lr;     // Limited Retry (CDW12[31])
    bool     deac;   // Deallocate (CDW12[25])
    bool     nsz;    // Namespace Zeroes (CDW12[23])
    uint16_t dspec;  // Directive Specific (CDW13[31:16])
    uint16_t cev;    // Command Extension Value (CDW13[15:0], cetype!=0)
    uint32_t lbtl;   // Logical Block Tags Lower (CDW14)
    uint16_t lbat;   // Logical Block App Tag (CDW15[15:0])
    uint16_t lbatm;  // Logical Block App Tag Mask (CDW15[31:16])
};

// Copy source range entry (Format 0h/2h simplified)
struct NvmeCopySourceRange {
    uint32_t snsid;  // Source Namespace ID (Format 2h/3h; 0 = same namespace)
    uint64_t slba;   // Source Starting LBA
    uint16_t nlb;    // Number of Logical Blocks, 0's based
    uint8_t  cetype; // Command Extension Type
};

// NVMe Copy command parameters (NVM Command Set Spec §3.3.2)
struct NvmeCopyCommand {
    uint64_t prp1;    // PRP Entry 1 – source range descriptor list
    uint64_t prp2;    // PRP Entry 2
    uint64_t lbtu;    // Logical Block Tags Upper (CDW2/3 bits 47:0)
    uint64_t sdlba;   // Starting Destination LBA (CDW10/11)
    uint8_t  nr;      // Number of Ranges, 0's based (CDW12[7:0])
    uint8_t  desfmt;  // Descriptor Format (CDW12[11:8])
    uint8_t  prinfor; // Protection Information Read (CDW12[15:12])
    uint8_t  cetype;  // Command Extension Type (CDW12[19:16])
    uint8_t  dtype;   // Directive Type (CDW12[23:20])
    bool     stcw;    // Storage Tag Check Write (CDW12[24])
    bool     stcr;    // Storage Tag Check Read (CDW12[25])
    uint8_t  prinfow; // Protection Information Write (CDW12[29:26])
    bool     fua;     // Force Unit Access (CDW12[30])
    bool     lr;      // Limited Retry (CDW12[31])
    uint16_t cev;     // Command Extension Value (CDW13[15:0])
    uint16_t dspec;   // Directive Specific (CDW13[31:16])
    uint32_t lbtl;    // Logical Block Tags Lower (CDW14)
    uint16_t lbat;    // Logical Block App Tag (CDW15[15:0])
    uint16_t lbatm;   // Logical Block App Tag Mask (CDW15[31:16])
    std::vector<NvmeCopySourceRange> source_ranges;
};

// NVMe I/O Management Receive command parameters (NVM Command Set Spec §3.2.1)
struct NvmeIoManagementReceiveCommand {
    uint32_t nsid;   // Namespace Identifier
    uint8_t  mo;     // Management Operation
    uint32_t numd;   // Number of Dwords, 0's based
    uint64_t prp1;   // PRP Entry 1 – data destination
    uint64_t prp2;   // PRP Entry 2
};

class NvmeDevice {
public:
    NvmeDevice();

    NvmeStatus Read(const NvmeReadCommand& cmd);
    NvmeStatus Write(const NvmeWriteCommand& cmd);
    NvmeStatus Compare(const NvmeCompareCommand& cmd);
    NvmeStatus Verify(const NvmeVerifyCommand& cmd);
    NvmeStatus WriteUncorrectable(const NvmeWriteUncorrectableCommand& cmd);
    NvmeStatus DatasetManagement(const NvmeDatasetManagementCommand& cmd);
    NvmeStatus WriteZeroes(const NvmeWriteZeroesCommand& cmd);
    NvmeStatus Copy(const NvmeCopyCommand& cmd);
    NvmeStatus IoManagementReceive(const NvmeIoManagementReceiveCommand& cmd);

    // Fake device configuration helpers
    void SetReadOnly(uint64_t start_lba, uint64_t end_lba);
    void SetE2EProtection(bool enabled);
    void SetUncorrectable(uint64_t start_lba, uint64_t end_lba);
    void SetCompareFailure(bool fail);
    void SetDsmRangeLimit(uint8_t max_nr);  // 0 = no limit

private:
    std::set<uint64_t> read_only_lbas_;
    std::set<uint64_t> uncorrectable_lbas_;
    bool               e2e_protection_enabled_ = false;
    bool               compare_failure_        = false;
    uint8_t            dsm_range_limit_        = 0;
};
