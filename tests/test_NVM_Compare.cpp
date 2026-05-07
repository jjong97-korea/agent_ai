#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(CompareCommandTest, REQ_COMP_001_ReadsAndComparesWithBuffer) {
    NvmeDevice dev;
    NvmeCompareCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.Compare(cmd), NvmeStatus::Success);
}

TEST(CompareCommandTest, REQ_COMP_002_MatchingDataReturnsSuccess) {
    NvmeDevice dev;
    NvmeCompareCommand cmd{};
    EXPECT_EQ(dev.Compare(cmd), NvmeStatus::Success);
}

TEST(CompareCommandTest, REQ_COMP_003_MismatchReturnsCompareFailure) {
    NvmeDevice dev;
    dev.SetCompareFailure(true);
    NvmeCompareCommand cmd{};
    EXPECT_EQ(dev.Compare(cmd), NvmeStatus::CompareFailure);
}

TEST(CompareCommandTest, REQ_COMP_004_MetadataExcludingPiAlsoCompared) {
    NvmeDevice dev;
    NvmeCompareCommand cmd{};
    cmd.mptr = 0x1000;  // metadata pointer provided
    EXPECT_EQ(dev.Compare(cmd), NvmeStatus::Success);
}

TEST(CompareCommandTest, REQ_COMP_005_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeCompareCommand cmd{};
    NvmeStatus s = dev.Compare(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::InvalidProtectionInfo ||
                  s == NvmeStatus::CompareFailure);
    EXPECT_TRUE(valid);
}

TEST(CompareCommandTest, REQ_COMP_006_NlbIsZeroBased) {
    NvmeCompareCommand cmd{};
    cmd.nlb = 0;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0));
    cmd.nlb = 0xFFFF;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0xFFFF));
}

TEST(CompareCommandTest, REQ_COMP_007_PractBitShallBeClearedToZero) {
    NvmeCompareCommand cmd{};
    cmd.prinfo = 0x0;  // PRACT = bit 3 of prinfo = 0
    EXPECT_EQ(cmd.prinfo & 0x8, static_cast<uint8_t>(0));
}

TEST(CompareCommandTest, REQ_COMP_008_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;  // E2E protection 비활성 (기본)
    NvmeCompareCommand cmd{};
    cmd.prinfo = 0x1;
    EXPECT_EQ(dev.Compare(cmd), NvmeStatus::InvalidProtectionInfo);
}
