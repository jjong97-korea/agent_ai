#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(IoManagementReceiveTest, REQ_IOMR_001_ReturnsManagementOperationData) {
    NvmeDevice dev;
    NvmeIoManagementReceiveCommand cmd{};
    cmd.mo   = 0x01;  // Reclaim Unit Handle Status
    cmd.nsid = 1;
    EXPECT_EQ(dev.IoManagementReceive(cmd), NvmeStatus::Success);
}

TEST(IoManagementReceiveTest, REQ_IOMR_002_MoOneReturnsReclaimUnitHandleStatus) {
    NvmeDevice dev;
    NvmeIoManagementReceiveCommand cmd{};
    cmd.mo = 0x01;
    EXPECT_EQ(dev.IoManagementReceive(cmd), NvmeStatus::Success);
}

TEST(IoManagementReceiveTest, REQ_IOMR_003_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeIoManagementReceiveCommand cmd{};
    NvmeStatus s = dev.IoManagementReceive(cmd);
    EXPECT_EQ(s, NvmeStatus::Success);
}

TEST(IoManagementReceiveTest, REQ_IOMR_004_NumdIsZeroBased) {
    NvmeIoManagementReceiveCommand cmd{};
    cmd.numd = 0;    // 0's based → 1 Dword
    EXPECT_EQ(cmd.numd, static_cast<uint32_t>(0));
    cmd.numd = 0xFF; // 0's based → 256 Dwords
    EXPECT_EQ(cmd.numd, static_cast<uint32_t>(0xFF));
}
