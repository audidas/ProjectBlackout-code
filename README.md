# Project Blackout

> 코드 전용 발췌입니다. 에셋(`Content/`)·서드파티 플러그인을 제외했으며 그대로는 빌드되지 않습니다(코드 열람용). 전체 프로젝트는 학원 4인 팀 + 기업 협약으로 제작했고, 이 저장소는 **본인(NET·인프라 담당)이 작성한 C++ 코드**를 담았습니다.

## 담당 역할 — Core Systems & Infrastructure (NET)

백엔드(Nest.js / Django / FastAPI) 경험을 게임 서버 인프라로 옮겨 네트워크·매치 플로우·운영 안정성·데이터 파이프라인을 담당했습니다.

### 매치메이킹 · 데디케이티드 서버
- 매치메이킹 서버 연동(Nest.js · Redis · WebSocket): 서버 상태 전이를 단일 권위로 관리
- 30초 Heartbeat + Redis 키 만료로 크래시 자동 감지(신호의 부재 = dead)
- 멱등키(event_id)로 중복 보고·재시도 안전 처리, API 키는 환경변수로 분리
- 데디 자동 등록(ServerId), ServerTravel 기반 로비↔보스 전환(동일 데디 유지), 빈 서버 복구

### 매치 플로우 · 재접속
- 로비→보스 전환 로딩 게이트(전원 로딩 완료 후 전투 시작)
- 크래시-재실행 후 진행 중 세션 복귀, 재매칭 WebSocket 재연결 안정화
- 인게임 메인 화면 이탈(본인만 이탈, 잔류 인원 매치 유지)

### 텔레메트리 (풀스택)
- 데디 전용 1Hz 위치/이벤트 샘플러 → 비동기 best-effort 백엔드 적재 → deck.gl 웹 뷰어
- 백엔드/웹 뷰어는 별도 저장소

### 전투 · 시스템
- 보스 HP 정원 스케일링(1인/4인, 서버 권위), 매치 통계(서버 권위 집계)
- 오브젝트 풀 프리로드, 3인칭 카메라 충돌 폴리싱

## 기술 스택
- 게임: Unreal Engine 5 (C++), GAS, StateTree
- 백엔드: Nest.js, Redis, Prisma, WebSocket
- 인프라: Dedicated Server, AWS / 분석: deck.gl

## 코드 위치
- `Source/ProjectBlackout/Framework/` — GameMode 계층 · 매치 플로우 · 데디 세션 · 매치메이킹 서브시스템
- `Source/ProjectBlackout/` — 게임플레이 / 네트워크 코드
