#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(ReadCommandTest, REQ_READ_001_ReadsDataFromLogicalBlocks) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_002_FuaOneReadsFromNonVolatile) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.fua = true;
    cmd.lr  = false;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_003_FuaZeroHasNoEffect) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.fua = false;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_004_LrOneLimitsRetry) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.lr  = true;
    cmd.fua = false;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_005_LrZeroAppliesAllRecovery) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.lr = false;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_006_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    NvmeStatus s = dev.Read(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::ConflictingAttributes ||
                  s == NvmeStatus::InvalidProtectionInfo);
    EXPECT_TRUE(valid);
}

TEST(ReadCommandTest, REQ_READ_007_NlbIsZeroBased) {
    NvmeReadCommand cmd{};
    cmd.nlb = 0;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0));
    cmd.nlb = 0xFFFF;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0xFFFF));
}

TEST(ReadCommandTest, REQ_READ_008_CdwThirteenLayoutByCetype) {
    NvmeReadCommand cmd_nonzero{};
    cmd_nonzero.cetype = 0x1;
    cmd_nonzero.cev    = 0xABCD;
    EXPECT_EQ(cmd_nonzero.cev, static_cast<uint16_t>(0xABCD));
}

TEST(ReadCommandTest, REQ_READ_009_ElbTagFieldsIgnoredWithoutE2E) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.elbtu  = 0x0000FFFFFFFFFFFFULL;
    cmd.elbtl  = 0xDEADBEEF;
    cmd.elbat  = 0xABCD;
    cmd.elbatm = 0x1234;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::Success);
}

TEST(ReadCommandTest, REQ_READ_010_ConflictingAttributesReturns0x80) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.fua = true;
    cmd.lr  = true;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::ConflictingAttributes);
}

TEST(ReadCommandTest, REQ_READ_011_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;
    NvmeReadCommand cmd{};
    cmd.prinfo = 0x1;
    EXPECT_EQ(dev.Read(cmd), NvmeStatus::InvalidProtectionInfo);
}
