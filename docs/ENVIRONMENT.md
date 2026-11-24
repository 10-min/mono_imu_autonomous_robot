# 개발/타깃 환경

## 타깃 런타임 (로봇)

| 항목 | 값 | 결정 ID |
|---|---|---|
| SBC | Raspberry Pi 5, 8GB | D-02 |
| OS | Ubuntu 24.04 (arm64) | D-02 |
| ROS | ROS 2 Jazzy | D-03 |
| MCU | STM32F103RBT6 (Cortex-M3, 72MHz, 3.3V UART) | D-01 |
| 카메라 | Raspberry Pi Camera Module 3 (CSI, libcamera) | D-04 |
| IMU | ICM-20948 (SPI) | D-04 |
| 모터 | JGA25-370 12V 130RPM(출력축), 감속비 46.8:1, 엔코더 11PPR × 2 | D-06 |
| 모터 드라이버 | L298N (2A/ch, 5V 로직) | D-06 |
| Pi 5 전원 | 12V → 5V/5A 벅 컨버터 | D-06 |

## 개발 호스트 (현재 작업 환경)

| 항목 | 확인된 값 |
|---|---|
| 아키텍처 | x86_64 |
| OS | Ubuntu 24.04.5 LTS |
| git | 2.43.0 |
| Python | 3.12.3 |
| ROS 2 | Jazzy 설치됨 (`/opt/ros/jazzy`) |
| colcon | `/usr/bin/colcon` |

빌드 전 환경 로드:

```bash
source /opt/ros/jazzy/setup.bash
```

## 버전 고정 원칙

- OS/ROS/주요 라이브러리(OpenCV, libcamera, SLAM 패키지 등)의 버전을 이 문서에 기록한다.
- 업그레이드는 별도 `P-step`으로 다루고, 변경 시 `docs/DECISIONS.md`에 근거를 남긴다.

## 미확인 사항 (R0-2 전에 확인)

- [x] JGA25-370 감속비 46.8:1, 130RPM은 출력축 기준
- [x] 모터 드라이버 L298N, Pi 5 전원 12V→5V/5A 벅
- [ ] JGA25-370의 **정지 전류** 실측 — L298N 2A/ch 초과 여부
- [ ] 배터리 사양, 바퀴 반지름/휠베이스/질량 실측
- [ ] L298N 5V 로직 점퍼 및 3.3V GPIO 인식 확인
