# Write Uncorrectable Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.7  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-WUNC-001 | 동작 | Write Uncorrectable command는 지정된 LBA 범위를 invalid로 표시한다. | 단위 테스트 |
| REQ-WUNC-002 | 동작 | invalid로 표시된 LBA를 이후 Read하면 Unrecovered Read Error가 반환된다. | 단위 테스트 |
| REQ-WUNC-003 | 동작 | invalid 상태는 해당 LBA에 Write 작업이 수행되면 해제된다. | 단위 테스트 |
| REQ-WUNC-004 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-WUNC-005 | 파라미터 제약 | NLB는 0's based 값이다. 실제 invalid 처리 블록 수 = NLB + 1. | 경계값 테스트 |
| REQ-WUNC-006 | 에러 조건 | 지정된 LBA 범위에 read-only block이 포함된 경우, controller는 status 0x82 (Attempted Write to Read Only Range)를 반환해야 한다. | 오류 주입 테스트 |
