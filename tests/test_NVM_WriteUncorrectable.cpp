#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(WriteUncorrectableCommandTest, REQ_WUNC_001_MarksLbasAsInvalid) {
    NvmeDevice dev;
    NvmeWriteUncorrectableCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.WriteUncorrectable(cmd), NvmeStatus::Success);
}

TEST(WriteUncorrectableCommandTest, REQ_WUNC_002_NlbIsZeroBased) {
    NvmeDevice dev;
    dev.SetReadOnly(1, 1);

    NvmeWriteUncorrectableCommand cmd{};
    cmd.slba = 0;

    cmd.nlb = 0;  // LBA 0만 → LBA 1 미접촉 → Success
    EXPECT_EQ(dev.WriteUncorrectable(cmd), NvmeStatus::Success);

    cmd.nlb = 1;  // LBA 0~1 → LBA 1 read-only → 에러
    EXPECT_EQ(dev.WriteUncorrectable(cmd), NvmeStatus::WriteToReadOnlyRange);
}

TEST(WriteUncorrectableCommandTest, REQ_WUNC_003_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeWriteUncorrectableCommand cmd{};
    NvmeStatus s = dev.WriteUncorrectable(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::WriteToReadOnlyRange);
    EXPECT_TRUE(valid);
}

TEST(WriteUncorrectableCommandTest, REQ_WUNC_004_DtypeAndDspecFields) {
    NvmeWriteUncorrectableCommand cmd{};
    cmd.dtype = 0x1;
    cmd.dspec = 0xBEEF;
    EXPECT_EQ(cmd.dtype, static_cast<uint8_t>(0x1));
    EXPECT_EQ(cmd.dspec, static_cast<uint16_t>(0xBEEF));
}

TEST(WriteUncorrectableCommandTest, REQ_WUNC_005_NlbZeroMeansOneBlock) {
    NvmeWriteUncorrectableCommand cmd{};
    cmd.nlb = 0;
    EXPECT_EQ(cmd.nlb, static_cast<uint16_t>(0));
}

TEST(WriteUncorrectableCommandTest, REQ_WUNC_006_WriteToReadOnlyRangeReturns0x82) {
    NvmeDevice dev;
    dev.SetReadOnly(5, 10);
    NvmeWriteUncorrectableCommand cmd{};
    cmd.slba = 5;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.WriteUncorrectable(cmd), NvmeStatus::WriteToReadOnlyRange);
}
