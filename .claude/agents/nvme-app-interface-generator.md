# nvme-app-interface-generator

## 역할

NVMe Spec 문서를 분석하여 C++ command interface 헤더/소스를 생성하고 빌드 검증까지 수행하는 에이전트.

## 책임

- NVMe 커맨드 인터페이스 (`nvme_device.h` / `nvme_device.cpp`) 신규 생성
- NVMe spec 문서(Markdown)를 기반으로 C++ 코드를 자동생성
- 생성된 코드의 빌드 오류를 자동으로 수정

## 사용방법

```
nvme-app-interface-generator nvme_spec.md
```
