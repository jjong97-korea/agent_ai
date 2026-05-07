# Dataset Management Command Requirements

**출처:** NVM Express NVM Command Set Specification, Revision 1.1 §3.3.3  
**생성일:** 2026-05-07

---

## 요구사항 목록

| ID | 분류 | 요구사항 | 검증 방법 |
|---|---|---|---|
| REQ-DSMG-001 | 동작 | Dataset Management command는 advisory command로, controller는 제공된 정보에 기반하여 no-action을 선택할 수 있다. | 단위 테스트 |
| REQ-DSMG-002 | 동작 | command가 완료될 때, controller는 associated I/O Completion Queue에 completion queue entry를 post해야 한다 (shall). | 단위 테스트 |
| REQ-DSMG-003 | 동작 | AD=1이면 NVM subsystem이 지정된 range를 deallocate 할 수 있다. | 단위 테스트 |
| REQ-DSMG-004 | 동작 | IDW=1이면 해당 dataset이 write 최적화 대상임을 나타낸다. | 단위 테스트 |
| REQ-DSMG-005 | 동작 | IDR=1이면 해당 dataset이 read 최적화 대상임을 나타낸다. | 단위 테스트 |
| REQ-DSMG-006 | 파라미터 제약 | NR은 0's based 값이다. 실제 range 수 = NR + 1. 최대 256개 range를 지원한다. | 경계값 테스트 |
| REQ-DSMG-007 | 파라미터 제약 | Figure 45에 명시된 모든 속성 조합(AD, IDW, IDR)이 허용된다. | 단위 테스트 |
| REQ-DSMG-008 | 에러 조건 | AD=1이고 deallocate 대상 범위에 read-only block이 포함된 경우, controller는 status 0x82 (Attempted Write to Read Only Range)를 반환할 수 있다 (optional). | 오류 주입 테스트 |
| REQ-DSMG-009 | 에러 조건 | Dataset Management processing limit(DMRL, DMRSL, DMSL)을 초과한 경우, controller는 status 0x83 (Command Size Limit Exceeded)를 반환해야 한다 (NVMDSMSV=0일 때). | 오류 주입 테스트 |
