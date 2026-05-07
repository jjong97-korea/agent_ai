# Read Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.4  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-READ-001 | 동작 | Read command는 지정된 logical block의 데이터 및 메타데이터(applicable한 경우)를 I/O controller에서 읽는다. | 단위 테스트 |
| REQ-READ-002 | 동작 | FUA=1인 경우, controller는 non-volatile medium에서 커밋된 데이터를 읽어 반환해야 한다. | 단위 테스트 |
| REQ-READ-003 | 동작 | FUA=0인 경우, FUA 비트는 아무런 효과가 없다. | 단위 테스트 |
| REQ-READ-004 | 동작 | LR=1인 경우, controller는 제한된 retry 노력을 적용해야 한다 (should). | 단위 테스트 |
| REQ-READ-005 | 동작 | LR=0인 경우, controller는 사용 가능한 모든 error recovery 수단을 적용해야 한다 (should). | 단위 테스트 |
| REQ-READ-006 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-READ-007 | 파라미터 제약 | NLB (Number of Logical Blocks)는 0's based 값이다. 실제 읽기 블록 수 = NLB + 1. | 경계값 테스트 |
| REQ-READ-008 | 파라미터 제약 | CDW13의 레이아웃은 CETYPE 값에 따라 결정된다. CETYPE=0h이면 CDW13은 Reserved, CETYPE≠0h이면 CEV[15:0] 형식을 따른다. | 단위 테스트 |
| REQ-READ-009 | 파라미터 제약 | ELBTU(CDW2/3 bits 47:0), ELBTL(CDW14), ELBAT(CDW15[15:0]), ELBATM(CDW15[31:16]) 필드는 namespace가 E2E protection으로 포맷되지 않은 경우 무시된다. | 단위 테스트 |
| REQ-READ-010 | 에러 조건 | FUA=1 && LR=1인 경우 controller는 status 0x80 (Conflicting Attributes)를 반환해야 한다. | 오류 주입 테스트 |
| REQ-READ-011 | 에러 조건 | PRINFO 설정이 namespace PI 포맷과 호환되지 않는 경우 controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
