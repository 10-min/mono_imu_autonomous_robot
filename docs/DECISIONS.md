# 설계 결정과 근거

`plan.md` §3 결정 레지스트의 확정 항목을 상세화한 문서. 권장안과 다른 선택을 할 때는 반드시 근거를 남긴다.

## D-01 MCU / 보드 — STM32F103RBT6

- 3.3V UART 지원, Pi 5 GPIO와 레벨 직접 호환.
- Cortex-M3 / 72MHz, FPU 없음 → 모터 PID + odom(100Hz) 수준은 처리 가능, VIO/SLAM은 Pi에서 수행.
- 확인 필요: 사용 가능한 TIM 채널 수(PWM 2ch + Encoder 2ch)와 UART 핀 배정.

## D-02 SBC / OS — Raspberry Pi 5 8GB, Ubuntu 24.04

- 8GB 메모리로 VIO/SLAM 연산 여유 확보.
- Ubuntu 24.04는 ROS 2 Jazzy의 공식 지원 배포판.

## D-03 ROS 2 배포판 — Jazzy

- Ubuntu 24.04와 조합이 공식 지원되며, 호스트에도 `/opt/ros/jazzy` 설치 확인됨.

## D-04 센서 — 별도 UDP 카메라 + ICM-20948 (STM32 SPI1)

- 카메라: Pi CSI 대신 **별도 송신기에서 UDP로 JPEG 스트림** 수신 (640×480@30). `camera_udp_node`가 재조립 후 `/camera/image_raw` 발행. 송신 장치/포트/PDU 형식은 `P0-4`에서 확정.
- IMU: **STM32 SPI1**(PA5/PA6/PA7) + DRDY 인터럽트로 200Hz 샘플링. MCU가 타임스탬프를 붙여 `IMU_DATA` 프레임으로 전송하고 `stm32_bridge`가 `/imu/data_raw` 발행.
- SPI1 사용에 따른 핀 충돌을 피하려 USART1→USART2, PWM TIM2→TIM1, 좌 엔코더 TIM3→TIM2로 재배치 (`docs/SYSTEM_DESIGN.md` §4).
- UART 대역 확보를 위해 baud 115200→**230400** (`D-07`). 예상 IMU 지연 3~6ms.

## D-05 저장소 전략 — GitHub public, main + feature 브랜치

- `git init` 후 `main` 브랜치 사용.
- 리모트: `origin = https://github.com/10-min/mono_imu_autonomous_robot.git` (연결 및 최초 push 완료)
- 커밋 메시지는 `[P<phase>-<n>] ...` 형식.

## D-06 구동 방식 — 2바퀴 차동구동 + JGA25-370

- JGA25-370 12V 130RPM(출력축), 감속비 46.8:1, 엔코더 2채널 11PPR → 2059.2 count/바퀴 회전.
- 모터 드라이버: **L298N** (2A/ch, 방열판 + 5V 로직 필요, 전압 강하 큼).
- Pi 5 전원: **12V → 5V/5A 벅 컨버터**.
- 배터리 3S Li-po 1800mAh 30C, 바퀴 반지름 32.5mm, 휠베이스 198mm, 카메라 640×480@30fps, IMU ODR 200Hz, 섀시 조립 완료.
- 정지 전류와 로봇 질량은 미정이므로 `docs/HARDWARE_BOM.md` §5에 실측 항목으로 남겨 둠.

## D-07 UART 프로토콜 — 230400bps

- IMU 200Hz 전송(약 60kbps) + odom 50Hz + 명령을 수용하려면 115200bps는 여유가 부족(약 73% 사용).
- **230400bps, 8N1, 3.3V TTL**로 확정. 예상 사용량 약 84.6kbps(여유 63%).
- IMU 프레임에는 MCU 타임스탬프를 포함해 ROS 시간 오프셋을 보정한다.

## D-09 제어 주기 — 50Hz 기본

- 최대 속도 0.3 m/s, 휠베이스 198mm 기준 100Hz까지 필요하지 않음.
- MCU 제어 **50Hz**(파라미터로 100Hz 전환 가능), 피드백 50Hz, IMU 200Hz, ROS bridge 50Hz로 확정.
