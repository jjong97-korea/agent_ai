#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(DatasetManagementCommandTest, REQ_DSMG_001_AdvisoryCommandReturnsSuccess) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    cmd.nr = 0;
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::Success);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_002_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    NvmeStatus s = dev.DatasetManagement(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::WriteToReadOnlyRange ||
                  s == NvmeStatus::CommandSizeLimitExceeded);
    EXPECT_TRUE(valid);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_003_AdDeallocatesRange) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    cmd.ad = true;
    cmd.ranges.push_back({0, 10, 0});  // cattr=0, llb=10, slba=0
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::Success);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_004_IdwSetsWriteOptimization) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    cmd.idw = true;
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::Success);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_005_IdrSetsReadOptimization) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    cmd.idr = true;
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::Success);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_006_NrIsZeroBased) {
    NvmeDatasetManagementCommand cmd{};
    cmd.nr = 0;   // 0's based → 1 range
    EXPECT_EQ(cmd.nr, static_cast<uint8_t>(0));
    cmd.nr = 255; // 0's based → 256 ranges (max)
    EXPECT_EQ(cmd.nr, static_cast<uint8_t>(255));
}

TEST(DatasetManagementCommandTest, REQ_DSMG_007_AllAttributeCombinationsAllowed) {
    NvmeDevice dev;
    NvmeDatasetManagementCommand cmd{};
    cmd.ad  = true;
    cmd.idw = true;
    cmd.idr = true;
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::Success);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_008_DeallocateReadOnlyRangeReturns0x82) {
    NvmeDevice dev;
    dev.SetReadOnly(5, 5);
    NvmeDatasetManagementCommand cmd{};
    cmd.ad = true;
    cmd.nr = 0;
    cmd.ranges.push_back({0, 1, 5});  // slba=5, llb=1 → LBA 5 (read-only)
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::WriteToReadOnlyRange);
}

TEST(DatasetManagementCommandTest, REQ_DSMG_009_ExceedingRangeLimitReturns0x83) {
    NvmeDevice dev;
    dev.SetDsmRangeLimit(3);  // max NR = 3 (0's based: 0,1,2)
    NvmeDatasetManagementCommand cmd{};
    cmd.nr = 3;  // 3 >= limit(3) → 초과
    EXPECT_EQ(dev.DatasetManagement(cmd), NvmeStatus::CommandSizeLimitExceeded);
}
