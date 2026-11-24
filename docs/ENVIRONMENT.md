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
| 모터 | JGA25-370 12V 130RPM, 엔코더 2채널 11PPR × 2 | D-06 |

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

- [ ] JGA25-370의 **감속비(기어비)** — 11PPR은 모터축 기준이므로 tick→m 변환에 필요
- [ ] JGA25-370의 **정지 전류** — 모터 드라이버 선정 기준
- [ ] Pi 5 전원 공급 방법 (5V/5A USB-C PD 또는 12V→5V 벅컨버터) 및 공통 GND
