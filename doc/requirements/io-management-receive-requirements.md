# I/O Management Receive Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.2.1  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-IOMR-001 | 동작 | I/O Management Receive command는 Management Operation(MO) 필드가 지정하는 데이터를 반환한다. | 단위 테스트 |
| REQ-IOMR-002 | 동작 | MO=01h (Reclaim Unit Handle Status)인 경우, Reclaim Unit Handle Status Descriptor List를 반환한다. Descriptor는 Placement Handle 오름차순, Reclaim Group Identifier 오름차순으로 정렬된다. | 단위 테스트 |
| REQ-IOMR-003 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다. | 단위 테스트 |
| REQ-IOMR-004 | 파라미터 제약 | NUMD는 0's based 값으로, 반환할 데이터의 Dword 수를 지정한다. | 단위 테스트 |
