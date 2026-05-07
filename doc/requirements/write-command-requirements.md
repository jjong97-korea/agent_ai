# Write Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.6  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-WRITE-001 | 동작 | Write command는 지정된 logical block에 데이터 및 메타데이터(applicable한 경우)를 I/O controller에 기록한다. | 단위 테스트 |
| REQ-WRITE-002 | 동작 | FUA=1인 경우, controller는 command completion을 반환하기 전에 데이터 및 메타데이터를 non-volatile medium에 기록해야 한다 (shall). | 단위 테스트 |
| REQ-WRITE-003 | 동작 | FUA=0인 경우, FUA 비트는 아무런 효과가 없다. | 단위 테스트 |
| REQ-WRITE-004 | 동작 | LR=1인 경우, controller는 제한된 retry 노력을 적용해야 한다 (should). | 단위 테스트 |
| REQ-WRITE-005 | 동작 | LR=0인 경우, controller는 데이터를 NVM에 기록하기 위해 사용 가능한 모든 error recovery 수단을 적용해야 한다 (should). | 단위 테스트 |
| REQ-WRITE-006 | 동작 | command가 성공 또는 실패로 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-WRITE-007 | 파라미터 제약 | NLB (Number of Logical Blocks) 는 0's based 값이다. 실제 기록 블록 수 = NLB + 1. | 경계값 테스트 |
| REQ-WRITE-008 | 파라미터 제약 | CDW13의 레이아웃은 CETYPE 값에 따라 결정된다. CETYPE=0h이면 Figure 71 형식, CETYPE≠0h이면 Figure 72 형식을 따른다. | 단위 테스트 |
| REQ-WRITE-009 | 파라미터 제약 | DSM.AF (bits 3:0) 의 유효 범위는 0h~6h이다. 7h~Fh는 Reserved이다. | 경계값 테스트 |
| REQ-WRITE-010 | 파라미터 제약 | PRINFO 필드(CDW12[29:26])는 Figure 11에 정의된 protection information action 및 check field를 지정한다. | 단위 테스트 |
| REQ-WRITE-011 | 파라미터 제약 | STC=1인 경우, Storage Tag field는 Figure 12에 정의된 end-to-end data protection processing의 일부로 검사되어야 한다 (shall). | 단위 테스트 |
| REQ-WRITE-012 | 파라미터 제약 | LBTU(CDW2/3 bits 47:0), LBTL(CDW14), LBAT(CDW15[15:0]), LBATM(CDW15[31:16]) 필드는 namespace가 end-to-end protection information으로 포맷되지 않은 경우 controller에 의해 무시된다. | 단위 테스트 |
| REQ-WRITE-013 | 에러 조건 | command의 속성이 충돌하는 경우, controller는 status 0x80 (Conflicting Attributes)를 반환해야 한다. | 오류 주입 테스트 |
| REQ-WRITE-014 | 에러 조건 | PRINFO 설정이 namespace가 포맷된 Protection Information과 호환되지 않거나 ILBRT field가 유효하지 않은 경우, controller는 status 0x81 (Invalid Protection Information)을 반환해야 한다. | 오류 주입 테스트 |
| REQ-WRITE-015 | 에러 조건 | 지정된 LBA 범위에 read-only block이 포함된 경우, controller는 status 0x82 (Attempted Write to Read Only Range)를 반환해야 한다. 단, read-only 상태가 namespace write protection state 변경으로 인한 경우는 이 status를 반환하지 않는다. | 오류 주입 테스트 |
