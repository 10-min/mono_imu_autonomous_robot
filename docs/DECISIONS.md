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

## D-04 센서 — RPi Camera Module 3 + ICM-20948 (SPI)

- 카메라 모듈 3은 libcamera 기반이므로 ROS 2에서는 `camera_ros` 계열 드라이버 사용 (`v4l2_camera`는 모듈 3 지원 제한).
- ICM-20948을 SPI로 연결해 IMU 출력률과 지연을 확보. 3.3V 레벨은 Pi와 호환.

## D-05 저장소 전략 — GitHub public, main + feature 브랜치

- `git init` 후 `main` 브랜치 사용.
- 리모트: `origin = https://github.com/10-min/mono_imu_autonomous_robot.git` (연결 완료, 최초 push는 인증 설정 후 수행)
- 커밋 메시지는 `[P<phase>-<n>] ...` 형식.

## D-06 구동 방식 — 2바퀴 차동구동 + JGA25-370

- JGA25-370 12V 130RPM, 엔코더 2채널 11PPR.
- 미확인: 감속비, 정지 전류, 드라이버 선정, 배터리/벅컨버터 사양 → `P0-2`에서 확정.
