# Write Zeroes Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.8  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-WZRS-001 | 동작 | Write Zeroes command는 지정된 LBA 범위를 0으로 초기화한다. | 단위 테스트 |
| REQ-WZRS-002 | 동작 | 성공 완료 후 해당 LBA 범위를 Read하면 all bytes 0h를 반환해야 한다 (shall). | 단위 테스트 |
| REQ-WZRS-003 | 동작 | DEAC=1이면 controller가 지정된 LBA를 deallocate 할 수 있다. | 단위 테스트 |
| REQ-WZRS-004 | 동작 | NSZ=1 && DEAC=1이면 namespace 내 모든 LBA를 0으로 초기화 요청. SLBA와 NLB는 무시된다. | 단위 테스트 |
| REQ-WZRS-005 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-WZRS-006 | 파라미터 제약 | NLB는 0's based 값이다. NSZ=1인 경우 controller는 NLB를 무시한다. | 경계값 테스트 |
| REQ-WZRS-007 | 파라미터 제약 | FUA=1인 경우, controller는 completion 반환 전에 non-volatile medium에 데이터를 기록해야 한다 (shall). | 단위 테스트 |
| REQ-WZRS-008 | 파라미터 제약 | STC bit는 '0'으로 설정해야 한다 (shall). | 단위 테스트 |
| REQ-WZRS-009 | 에러 조건 | PRINFO 설정이 namespace PI 포맷과 호환되지 않거나 ILBRT field가 유효하지 않은 경우, controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
| REQ-WZRS-010 | 에러 조건 | 지정된 LBA 범위에 read-only block이 포함된 경우, controller는 status 0x82 (Attempted Write to Read Only Range)를 반환해야 한다. | 오류 주입 테스트 |
