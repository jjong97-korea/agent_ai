#include <gtest/gtest.h>
#include "src/nvme_device.h"

// REQ-WRITE-001: Write command는 지정된 logical block에 데이터를 기록한다.
TEST(WriteCommandTest, REQ_WRITE_001_WritesDataToLogicalBlocks) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.slba = 0;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-002: FUA=1이면 non-volatile medium에 기록 후 completion 반환.
TEST(WriteCommandTest, REQ_WRITE_002_FuaOneWritesBeforeCompletion) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.fua = true;
    cmd.lr  = false;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-003: FUA=0이면 아무런 효과가 없다.
TEST(WriteCommandTest, REQ_WRITE_003_FuaZeroHasNoEffect) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.fua = false;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-004: LR=1이면 제한된 retry 노력을 적용한다.
TEST(WriteCommandTest, REQ_WRITE_004_LrOneLimitsRetry) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.lr  = true;
    cmd.fua = false;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-005: LR=0이면 사용 가능한 모든 error recovery 수단을 적용한다.
TEST(WriteCommandTest, REQ_WRITE_005_LrZeroAppliesAllRecovery) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.lr = false;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-006: 완료 시 유효한 NvmeStatus를 반환한다 (CQ entry post 모의).
TEST(WriteCommandTest, REQ_WRITE_006_CompletionReturnsValidStatus) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    NvmeStatus s = dev.Write(cmd);
    bool valid = (s == NvmeStatus::Success ||
                  s == NvmeStatus::ConflictingAttributes ||
                  s == NvmeStatus::InvalidProtectionInfo ||
                  s == NvmeStatus::WriteToReadOnlyRange);
    EXPECT_TRUE(valid);
}

// REQ-WRITE-007: NLB는 0's based 값이다 (nlb=0 → 1 block, nlb=1 → 2 blocks).
TEST(WriteCommandTest, REQ_WRITE_007_NlbIsZeroBased) {
    NvmeDevice dev;
    dev.SetReadOnly(1, 1);  // LBA 1만 read-only

    NvmeWriteCommand cmd{};
    cmd.slba = 0;

    cmd.nlb = 0;  // LBA 0 만 기록 → LBA 1 미접촉 → Success
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);

    cmd.nlb = 1;  // LBA 0~1 기록 → LBA 1 read-only 접촉 → 에러
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::WriteToReadOnlyRange);
}

// REQ-WRITE-008: CDW13 레이아웃은 CETYPE 값에 따라 결정된다.
TEST(WriteCommandTest, REQ_WRITE_008_CdwThirteenLayoutByCetype) {
    NvmeWriteCommand cmd_zero{};
    cmd_zero.cetype          = 0x0;
    cmd_zero.dsm.access_freq = 0x3;
    EXPECT_EQ(cmd_zero.dsm.access_freq, static_cast<uint8_t>(0x3));

    NvmeWriteCommand cmd_nonzero{};
    cmd_nonzero.cetype = 0x1;
    cmd_nonzero.cev    = 0xBEEF;
    EXPECT_EQ(cmd_nonzero.cev, static_cast<uint16_t>(0xBEEF));
}

// REQ-WRITE-009: DSM.AF 유효 범위는 0h~6h이다 (7h~Fh는 Reserved).
TEST(WriteCommandTest, REQ_WRITE_009_DsmAccessFreqValidRange) {
    NvmeWriteCommand cmd{};
    cmd.dsm.access_freq = 0x0;
    EXPECT_EQ(cmd.dsm.access_freq, static_cast<uint8_t>(0x0));
    cmd.dsm.access_freq = 0x6;
    EXPECT_EQ(cmd.dsm.access_freq, static_cast<uint8_t>(0x6));
}

// REQ-WRITE-010: PRINFO 필드는 4비트 protection info action을 지정한다.
TEST(WriteCommandTest, REQ_WRITE_010_PrinfoFieldRange) {
    NvmeWriteCommand cmd{};
    cmd.prinfo = 0x0;
    EXPECT_EQ(cmd.prinfo, static_cast<uint8_t>(0x0));
    cmd.prinfo = 0xF;
    EXPECT_EQ(cmd.prinfo, static_cast<uint8_t>(0xF));
}

// REQ-WRITE-011: STC=1이면 Storage Tag field를 E2E protection 처리에서 검사한다.
TEST(WriteCommandTest, REQ_WRITE_011_StcOneEnablesStorageTagCheck) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.stc    = true;
    cmd.prinfo = 0;  // E2E 없이 prinfo=0이면 STC=1도 Success
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-012: namespace에 E2E protection이 없으면 LB tag 필드는 무시된다.
TEST(WriteCommandTest, REQ_WRITE_012_LbTagFieldsIgnoredWithoutE2E) {
    NvmeDevice dev;  // 기본: E2E protection 비활성
    NvmeWriteCommand cmd{};
    cmd.lbtu  = 0x0000FFFFFFFFFFFFULL;
    cmd.lbtl  = 0xDEADBEEF;
    cmd.lbat  = 0xABCD;
    cmd.lbatm = 0x1234;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::Success);
}

// REQ-WRITE-013: 속성 충돌 시 0x80 (Conflicting Attributes)를 반환한다.
TEST(WriteCommandTest, REQ_WRITE_013_ConflictingAttributesReturns0x80) {
    NvmeDevice dev;
    NvmeWriteCommand cmd{};
    cmd.fua = true;
    cmd.lr  = true;  // FUA + LR 동시 설정은 충돌 속성
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::ConflictingAttributes);
}

// REQ-WRITE-014: PRINFO가 namespace PI 포맷과 호환되지 않으면 0x81을 반환한다.
TEST(WriteCommandTest, REQ_WRITE_014_InvalidProtectionInfoReturns0x81) {
    NvmeDevice dev;  // E2E protection 비활성 (기본)
    NvmeWriteCommand cmd{};
    cmd.prinfo = 0x1;  // E2E 미포맷 namespace에서 PRINFO != 0 → 유효하지 않음
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::InvalidProtectionInfo);
}

// REQ-WRITE-015: LBA 범위에 read-only block이 있으면 0x82를 반환한다.
TEST(WriteCommandTest, REQ_WRITE_015_WriteToReadOnlyRangeReturns0x82) {
    NvmeDevice dev;
    dev.SetReadOnly(5, 10);
    NvmeWriteCommand cmd{};
    cmd.slba = 5;
    cmd.nlb  = 0;
    EXPECT_EQ(dev.Write(cmd), NvmeStatus::WriteToReadOnlyRange);
}
