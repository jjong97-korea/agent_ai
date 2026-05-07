# Compare Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.1  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-COMP-001 | 동작 | Compare command는 NVM에서 지정된 LBA 데이터를 읽어 비교 버퍼(DPTR)와 비교한다. | 단위 테스트 |
| REQ-COMP-002 | 동작 | 데이터가 일치하면 command는 Success로 완료된다. | 단위 테스트 |
| REQ-COMP-003 | 동작 | 데이터 불일치가 발생하면 command는 Compare Failure (0x84) 로 완료된다. | 오류 주입 테스트 |
| REQ-COMP-004 | 동작 | 메타데이터가 제공된 경우, protection information을 제외한 메타데이터도 비교한다. | 단위 테스트 |
| REQ-COMP-005 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-COMP-006 | 파라미터 제약 | NLB는 0's based 값이다. 실제 비교 블록 수 = NLB + 1. | 경계값 테스트 |
| REQ-COMP-007 | 파라미터 제약 | PRINFO의 PRACT bit는 '0'으로 설정해야 한다. | 단위 테스트 |
| REQ-COMP-008 | 에러 조건 | PRINFO 설정이 namespace PI 포맷과 호환되지 않거나 EILBRT field가 유효하지 않은 경우, controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
