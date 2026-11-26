# mono_imu_autonomous_robot

ORB-SLAM3 기반 모노 카메라/IMU 자율주행 로봇 (STM32 + Raspberry Pi).

- 저장소: https://github.com/10-min/mono_imu_autonomous_robot.git
- 로드맵: [`project.md`](project.md) (Phase 0 ~ 7)
- 실행 계획: [`plan.md`](plan.md) (단계별 작업 / 리뷰 게이트 / 결정 레지스트)
- 현재 단계: **Phase 0 — System Design** (`P0-3` 수정 반영, **`R0-3` 검토 대기**)

## 확정 사양 (D-01 ~ D-06)

| 구분 | 사양 |
|---|---|
| MCU | STM32F103RBT6, 3.3V UART |
| SBC | Raspberry Pi 5 8GB, Ubuntu 24.04 |
| ROS | ROS 2 Jazzy |
| 센서 | 별도 UDP 카메라(640×480@30 JPEG), ICM-20948를 STM32 SPI1로 현재 50Hz(200Hz 미결정) |
| 통신 | Pi↔STM32 UART 115200 (USART1 remap) / 카메라 UDP |
| 구동 | 2바퀴 차동 + JGA25-370 12V 130RPM(출력축), 감속비 46.8:1, 엔코더 11PPR |
| 기구 | 바퀴 반지름 32.5mm, 휠베이스 198mm, 섀시 조립 완료 |
| 모터 드라이버 | L298N (2A/ch) |
| 전원 | 3S Li-po 1800mAh 30C → 모터 / 12V→5V/5A 벅 → Pi 5 / 3.3V → MCU·IMU |
| SLAM | ORB-SLAM3 기반 (리모트 초기 README 설명 반영, 상세 구현은 `D-10` 게이트에서 확정) |

## 디렉터리

```
.
├── docs/          # 설계/인터페이스/실험 문서
├── firmware/      # STM32 펌웨어
├── ros2_ws/src/   # ROS 2 패키지
├── scripts/       # 분석/실험 스크립트
└── bags/          # rosbag (git 추적 제외, 로컬 보관)
```

## 문서 색인

- `docs/SYSTEM_DESIGN.md` — 노드 구조/데이터 흐름/TF 권한
- `docs/ENVIRONMENT.md` — 개발/타깃 환경 및 버전
- `docs/REVIEW_LOG.md` — 게이트 승인 기록
- `docs/HARDWARE_BOM.md` — 부품/전원/핀매핑/미확인 항목
- `docs/DECISIONS.md` — 설계 결정과 근거
- 추후 생성: `INTERFACES.md`, `TF.md`, `UART_PROTOCOL.md`, `TEST_PLAN.md`, `EXPERIMENTS.md`

> 작업 규칙: `plan.md`의 게이트가 승인되기 전에는 다음 단계로 넘어가지 않습니다.
