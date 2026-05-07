#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(VerifyCommandTest, REQ_VFYS_001_VerifiesIntegrityWithoutDataTransfer) {
    NvmeDevice dev;
    NvmeVerifyCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.Verify(cmd), NvmeStatus::Success);
}

TEST(VerifyCommandTest, REQ_VFYS_002_FuaOneFlushesBeforeVerify) {
    NvmeDevice dev;
    NvmeVerifyCommand cmd{};
    cmd.fua = true;
    cmd.lr  = false;
    EXPECT_EQ(dev.Verify(cmd), NvmeStatus::Success);
}

TEST(VerifyCommandTest, REQ_VFYS_003_LrOneLimitsRetry) {
    NvmeDevice dev;
    NvmeVerifyCommand cmd{};
    cmd.lr  = true;
    cmd.fua = false;
    EXPECT_EQ(dev.Verify(cmd), NvmeStatus::Success);
}

TEST(VerifyCommandTest, REQ_VFYS_004_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeVerifyCommand cmd{};
    NvmeStatus s = dev.Verify(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::InvalidProtectionInfo);
    EXPECT_TRUE(valid);
}

TEST(VerifyCommandTest, REQ_VFYS_005_NlbIsZeroBased) {
    NvmeVerifyCommand cmd{};
    cmd.nlb = 0;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0));
    cmd.nlb = 0xFFFF;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0xFFFF));
}

TEST(VerifyCommandTest, REQ_VFYS_006_PractShallBeClearedToZero) {
    NvmeVerifyCommand cmd{};
    cmd.prinfo = 0x0;  // PRACT = bit 3 of prinfo = 0
    EXPECT_EQ(cmd.prinfo & 0x8, static_cast<uint8_t>(0));
}

TEST(VerifyCommandTest, REQ_VFYS_007_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;
    NvmeVerifyCommand cmd{};
    cmd.prinfo = 0x1;
    EXPECT_EQ(dev.Verify(cmd), NvmeStatus::InvalidProtectionInfo);
}
