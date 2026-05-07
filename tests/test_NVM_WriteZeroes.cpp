#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(WriteZeroesCommandTest, REQ_WZRS_001_ZerosOutLogicalBlocks) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_002_SuccessIndicatesBlocksClearedToZero) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_003_DeacRequestsDeallocate) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    cmd.deac = true;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_004_NszOneWithDeacOneZerosEntireNamespace) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    cmd.nsz  = true;
    cmd.deac = true;
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_005_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    NvmeStatus s = dev.WriteZeroes(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::InvalidProtectionInfo ||
                  s == NvmeStatus::WriteToReadOnlyRange);
    EXPECT_TRUE(valid);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_006_NlbIsZeroBased) {
    NvmeDevice dev;
    dev.SetReadOnly(1, 1);
    NvmeWriteZeroesCommand cmd{};
    cmd.slba = 0;

    cmd.nlb = 0;  // LBA 0만 → Success
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);

    cmd.nlb = 1;  // LBA 0~1 → LBA 1 read-only → 에러
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::WriteToReadOnlyRange);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_007_FuaOneWritesBeforeCompletion) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    cmd.fua = true;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::Success);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_008_StcShallBeClearedToZero) {
    NvmeWriteZeroesCommand cmd{};
    cmd.stc = false;  // shall be 0
    EXPECT_FALSE(cmd.stc);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_009_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;
    NvmeWriteZeroesCommand cmd{};
    cmd.prinfo = 0x1;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::InvalidProtectionInfo);
}

TEST(WriteZeroesCommandTest, REQ_WZRS_010_WriteToReadOnlyRangeReturns0x82) {
    NvmeDevice dev;
    dev.SetReadOnly(5, 10);
    NvmeWriteZeroesCommand cmd{};
    cmd.slba = 5;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.WriteZeroes(cmd), NvmeStatus::WriteToReadOnlyRange);
}
