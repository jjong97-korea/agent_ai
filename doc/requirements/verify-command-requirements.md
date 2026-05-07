# Verify Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.5  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-VFYS-001 | 동작 | Verify command는 저장된 데이터의 integrity를 검증하며, 데이터를 host에 전송하지 않는다. | 단위 테스트 |
| REQ-VFYS-002 | 동작 | FUA=1인 경우, controller는 volatile cache를 flush한 후 non-volatile medium의 데이터에 대해 Verify를 수행해야 한다 (shall). | 단위 테스트 |
| REQ-VFYS-003 | 동작 | LR=1이면 제한된 retry 노력을 적용하며, 이 동작은 Read command와 동일해야 한다 (shall). | 단위 테스트 |
| REQ-VFYS-004 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다. | 단위 테스트 |
| REQ-VFYS-005 | 파라미터 제약 | NLB는 0's based 값이다. 실제 검증 블록 수 = NLB + 1. | 경계값 테스트 |
| REQ-VFYS-006 | 파라미터 제약 | PRINFO의 PRACT bit는 '0'으로 설정해야 한다. PRACT≠0이면 controller는 command를 abort해야 한다 (shall). | 단위 테스트 |
| REQ-VFYS-007 | 에러 조건 | PRINFO 설정이 namespace PI 포맷과 호환되지 않거나 EILBRT field가 유효하지 않은 경우, controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
