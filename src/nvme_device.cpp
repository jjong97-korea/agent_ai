#include "nvme_device.h"

NvmeDevice::NvmeDevice() {}

NvmeStatus NvmeDevice::Read(const NvmeReadCommand& cmd) {
    if (cmd.fua && cmd.lr)
        return NvmeStatus::ConflictingAttributes;
    if (cmd.prinfo != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::Write(const NvmeWriteCommand& cmd) {
    if (cmd.fua && cmd.lr)
        return NvmeStatus::ConflictingAttributes;
    if (cmd.prinfo != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;
    for (uint32_t i = 0; i <= cmd.nlb; ++i) {
        if (read_only_lbas_.count(cmd.slba + i))
            return NvmeStatus::WriteToReadOnlyRange;
    }
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::Compare(const NvmeCompareCommand& cmd) {
    if (cmd.prinfo != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;
    if (compare_failure_)
        return NvmeStatus::CompareFailure;
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::Verify(const NvmeVerifyCommand& cmd) {
    if (cmd.prinfo != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::WriteUncorrectable(const NvmeWriteUncorrectableCommand& cmd) {
    for (uint32_t i = 0; i <= cmd.nlb; ++i) {
        if (read_only_lbas_.count(cmd.slba + i))
            return NvmeStatus::WriteToReadOnlyRange;
    }
    for (uint32_t i = 0; i <= cmd.nlb; ++i)
        uncorrectable_lbas_.insert(cmd.slba + i);
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::DatasetManagement(const NvmeDatasetManagementCommand& cmd) {
    if (dsm_range_limit_ > 0 && cmd.nr >= dsm_range_limit_)
        return NvmeStatus::CommandSizeLimitExceeded;
    if (cmd.ad) {
        for (const auto& range : cmd.ranges) {
            for (uint32_t i = 0; i < range.llb; ++i) {
                if (read_only_lbas_.count(range.slba + i))
                    return NvmeStatus::WriteToReadOnlyRange;
            }
        }
    }
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::WriteZeroes(const NvmeWriteZeroesCommand& cmd) {
    if (cmd.prinfo != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;
    if (!cmd.nsz) {
        for (uint32_t i = 0; i <= cmd.nlb; ++i) {
            if (read_only_lbas_.count(cmd.slba + i))
                return NvmeStatus::WriteToReadOnlyRange;
        }
    }
    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::Copy(const NvmeCopyCommand& cmd) {
    if (cmd.prinfow != 0 && !e2e_protection_enabled_)
        return NvmeStatus::InvalidProtectionInfo;

    uint32_t total_nlb = 0;
    for (const auto& range : cmd.source_ranges)
        total_nlb += static_cast<uint32_t>(range.nlb) + 1;

    for (uint32_t i = 0; i < total_nlb; ++i) {
        if (read_only_lbas_.count(cmd.sdlba + i))
            return NvmeStatus::WriteToReadOnlyRange;
    }

    if (cmd.desfmt == 2 || cmd.desfmt == 3) {
        uint64_t dst_end = cmd.sdlba + total_nlb - 1;
        for (const auto& range : cmd.source_ranges) {
            uint64_t src_end = range.slba + range.nlb;
            if (range.slba <= dst_end && cmd.sdlba <= src_end)
                return NvmeStatus::OverlappingIORange;
        }
    }

    return NvmeStatus::Success;
}

NvmeStatus NvmeDevice::IoManagementReceive(const NvmeIoManagementReceiveCommand& cmd) {
    return NvmeStatus::Success;
}

void NvmeDevice::SetReadOnly(uint64_t start_lba, uint64_t end_lba) {
    for (uint64_t lba = start_lba; lba <= end_lba; ++lba)
        read_only_lbas_.insert(lba);
}

void NvmeDevice::SetE2EProtection(bool enabled) {
    e2e_protection_enabled_ = enabled;
}

void NvmeDevice::SetUncorrectable(uint64_t start_lba, uint64_t end_lba) {
    for (uint64_t lba = start_lba; lba <= end_lba; ++lba)
        uncorrectable_lbas_.insert(lba);
}

void NvmeDevice::SetCompareFailure(bool fail) {
    compare_failure_ = fail;
}

void NvmeDevice::SetDsmRangeLimit(uint8_t max_nr) {
    dsm_range_limit_ = max_nr;
}
