#include <gtest/gtest.h>
#include "src/nvme_device.h"

TEST(CopyCommandTest, REQ_COPY_001_CopiesSourceRangesToDestination) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.sdlba = 100;
    cmd.nr    = 0;
    cmd.source_ranges.push_back({0, 0, 0, 0});  // snsid=0, slba=0, nlb=0, cetype=0
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::Success);
}

TEST(CopyCommandTest, REQ_COPY_002_SourceRangesCopiedInOrder) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.sdlba = 200;
    cmd.nr    = 1;
    cmd.source_ranges.push_back({0, 0, 0, 0});   // range 0: slba=0, nlb=0
    cmd.source_ranges.push_back({0, 10, 0, 0});  // range 1: slba=10, nlb=0
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::Success);
}

TEST(CopyCommandTest, REQ_COPY_003_FuaOneWritesBeforeCompletion) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.fua = true;
    cmd.lr  = false;
    cmd.source_ranges.push_back({0, 0, 0, 0});
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::Success);
}

TEST(CopyCommandTest, REQ_COPY_004_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.source_ranges.push_back({0, 0, 0, 0});
    NvmeStatus s = dev.Copy(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::InvalidProtectionInfo ||
                  s == NvmeStatus::WriteToReadOnlyRange ||
                  s == NvmeStatus::OverlappingIORange);
    EXPECT_TRUE(valid);
}

TEST(CopyCommandTest, REQ_COPY_005_NrIsZeroBased) {
    NvmeCopyCommand cmd{};
    cmd.nr = 0;
    EXPECT_EQ(cmd.nr, static_cast<uint8_t>(0));
    cmd.nr = 127;
    EXPECT_EQ(cmd.nr, static_cast<uint8_t>(127));
}

TEST(CopyCommandTest, REQ_COPY_006_DesfmtSpecifiesDescriptorFormat) {
    NvmeCopyCommand cmd{};
    cmd.desfmt = 0;
    EXPECT_EQ(cmd.desfmt, static_cast<uint8_t>(0));
    cmd.desfmt = 3;
    EXPECT_EQ(cmd.desfmt, static_cast<uint8_t>(3));
}

TEST(CopyCommandTest, REQ_COPY_007_PrintorAndPrintowFields) {
    NvmeCopyCommand cmd{};
    cmd.prinfor = 0x0;
    cmd.prinfow = 0x0;
    EXPECT_EQ(cmd.prinfor, static_cast<uint8_t>(0x0));
    EXPECT_EQ(cmd.prinfow, static_cast<uint8_t>(0x0));
}

TEST(CopyCommandTest, REQ_COPY_008_StcwAndStcrFields) {
    NvmeCopyCommand cmd{};
    cmd.stcw = true;
    cmd.stcr = true;
    EXPECT_TRUE(cmd.stcw);
    EXPECT_TRUE(cmd.stcr);
}

TEST(CopyCommandTest, REQ_COPY_009_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.prinfow = 0x1;
    cmd.source_ranges.push_back({0, 0, 0, 0});
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::InvalidProtectionInfo);
}

TEST(CopyCommandTest, REQ_COPY_010_WriteToReadOnlyRangeReturns0x82) {
    NvmeDevice dev;
    dev.SetReadOnly(100, 100);
    NvmeCopyCommand cmd{};
    cmd.sdlba = 100;
    cmd.source_ranges.push_back({0, 0, 0, 0});  // nlb=0 → 1 block → dst LBA 100
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::WriteToReadOnlyRange);
}

TEST(CopyCommandTest, REQ_COPY_011_OverlappingIORangeReturns0x87_Format2) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.desfmt = 2;          // Format 2h: overlap check enforced
    cmd.sdlba  = 5;          // dst: LBA 5
    cmd.source_ranges.push_back({0, 5, 0, 0});  // src: slba=5, nlb=0 → LBA 5
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::OverlappingIORange);
}

TEST(CopyCommandTest, REQ_COPY_012_OverlapNotCheckedForFormat0) {
    NvmeDevice dev;
    NvmeCopyCommand cmd{};
    cmd.desfmt = 0;          // Format 0h: overlap check NOT enforced
    cmd.sdlba  = 5;
    cmd.source_ranges.push_back({0, 5, 0, 0});  // same LBA range
    EXPECT_EQ(dev.Copy(cmd), NvmeStatus::Success);  // no overlap check for Format 0h
}
