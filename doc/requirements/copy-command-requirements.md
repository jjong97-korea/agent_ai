# Copy Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.2  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-COPY-001 | 동작 | Copy command는 하나 이상의 source range 데이터를 destination LBA range에 복사한다. | 단위 테스트 |
| REQ-COPY-002 | 동작 | source range의 데이터는 Source Range entry 순서대로 destination LBA에 순차 기록된다. | 단위 테스트 |
| REQ-COPY-003 | 동작 | FUA=1이면 controller는 completion 전에 non-volatile medium에 기록해야 한다 (shall). | 단위 테스트 |
| REQ-COPY-004 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-COPY-005 | 동작 | 실패 완료 시 CQE Dword 0에는 복사에 실패한 최초 Source Range entry 번호가 기록된다. | 단위 테스트 |
| REQ-COPY-006 | 파라미터 제약 | NR은 0's based 값이다. 실제 source range 수 = NR + 1. | 경계값 테스트 |
| REQ-COPY-007 | 파라미터 제약 | DESFMT는 사용할 Source Range Entries Copy Descriptor Format 유형을 지정한다. 지원되지 않는 포맷이면 controller는 abort해야 한다 (shall). | 단위 테스트 |
| REQ-COPY-008 | 파라미터 제약 | Format 2h/3h에서 SNSID가 0h 또는 FFFFFFFFh인 경우 controller는 Invalid Namespace or Format으로 abort해야 한다 (shall). | 단위 테스트 |
| REQ-COPY-009 | 에러 조건 | PRINFOW/PRINFOR 설정이 유효하지 않거나 ILBRT/EILBRT field가 유효하지 않은 경우, controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
| REQ-COPY-010 | 에러 조건 | destination LBA 범위에 read-only block이 포함된 경우, controller는 status 0x82 (Attempted Write to Read Only Range)를 반환해야 한다. | 오류 주입 테스트 |
| REQ-COPY-011 | 에러 조건 | Copy processing limit(NR > MSRC, NLB > MSSRL, total > MCL)을 초과한 경우, controller는 status 0x83 (Command Size Limit Exceeded)를 반환해야 한다. | 오류 주입 테스트 |
| REQ-COPY-012 | 에러 조건 | Format 2h/3h에서 source LBA range가 destination LBA range와 겹치는 경우, controller는 status 0x87 (Overlapping I/O Range)를 반환해야 한다 (shall). | 오류 주입 테스트 |
