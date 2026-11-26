# 시스템 설계 — 계층 구조 및 데이터 흐름 (P0-3)

- 작성일: 2026-11-24 / 수정: 2026-11-26 (사용자 주석 반영)
- 근거: `project.md` Phase 0, `docs/HARDWARE_BOM.md`, `plan.md` §3/§5
- 상태: **R0-3 승인 완료** (2026-11-26)

## 1. 계층 구조

구체적인 칩 이름 대신 **역할 계층**으로 구조를 기술하고, 실제 부품 매핑은 별도 표로 분리합니다.

| 계층 | 역할 | 포함 요소 |
|---|---|---|
| **Application Layer** | 목표→추정→계획→제어 결정 | `vio_node`, `slam_node`, `nav_node`, `safety_supervisor` |
| **ROS 2 Interface Layer** | 프로토콜 변환, ROS 토픽 발행 | `stm32_bridge`, `camera_udp_node` |
| **Embedded Layer** | 실시간 모터 제어, 센서 샘플링, 안전 | STM32 펌웨어 (PID, 엔코더, IMU, failsafe) |
| **Hardware Layer** | 물리 장치 | IMU, 엔코더, 모터 드라이버, 모터, UDP 카메라 송신기 |

### 부품 ↔ 계층 매핑

| 계층 요소 | 실제 부품 |
|---|---|
| Embedded Layer | STM32F103RBT6 (72MHz, RAM 20KB, FPU 없음) |
| IMU | ICM-20948, **SPI1** (PA5/PA6/PA7), CS=PA4, polling **200Hz** (펌웨어 50Hz → 변경 필요) |
| 엔코더 | JGA25-370 내장 **A/B quadrature 11PPR × 2** (현재 단일 채널 → 변경 필요) |
| 모터 드라이버 | L298N (2A/ch) |
| 모터 | JGA25-370 12V 130RPM(출력축), 감속비 46.8:1 |
| UDP 카메라 송신기 | 별도 송신 장치 → 640×480@30 JPEG UDP 스트림 |
| ROS 2 Interface | Raspberry Pi 5 8GB, Ubuntu 24.04, ROS 2 Jazzy |

## 2. 계층별 노드/컴포넌트

| 계층 | 노드 | 입력 | 출력 | 주기 | 실패 시 영향 |
|---|---|---|---|---|---|
| Hardware | IMU (ICM-20948) | SPI1 polling (CS=PA4) | 가속도/자이로 | **200Hz** (펌웨어 변경 필요) | VIO 자세 열화 |
| Hardware | 엔코더 | 모터 회전 | **A/B quadrature 펄스** | — | odom 정지 |
| Embedded | `firmware` | UART 명령, 엔코더, IMU | L298N PWM/방향, UART 피드백 | **PWM 1kHz**, cmd 이벤트 수신, odom 50Hz, **IMU 200Hz**, PID는 `P1-4`, failsafe는 `P1-7` | 모터 정지 (failsafe 포함) |
| ROS Interface | `stm32_bridge` | `/cmd_vel`, `/emergency_stop`, UART 프레임 | `SET_VEL`/`ESTOP`, `/odom`, `/wheel/odom`, `/imu/data_raw`, `/diagnostics` | 50Hz | 로봇 강제 정지, VIO prior 손실 |
| ROS Interface | `camera_udp_node` | UDP JPEG 스트림 | `/camera/image_raw`, `/camera/camera_info` | 30Hz | VIO/SLAM 중단 |
| Application | `vio_node` | 이미지, `/imu/data_raw`, `/wheel/odom` | `/vio/odom` (이후 `odom→base_link`) | 30Hz | 바퀴 odom fallback |
| Application | `slam_node` | 이미지, `/vio/odom` | `map→odom` TF, `/map`, 맵 파일 | 10~30Hz | 재위치 불가 |
| Application | `nav_node` | `/set_goal`, 맵, odom | `/cmd_vel` | 20~50Hz | 목표 미도착, 정지 |
| Application | `safety_supervisor` | `/diagnostics`, 노드 하트비트 | `/emergency_stop`, 경고 로그 | 10Hz | 소프트웨어 이상 감지 시 정지 |

> `imu_node`는 제거되었습니다. IMU는 MCU가 읽어 UART로 보내고 `stm32_bridge`가 `/imu/data_raw`를 발행합니다.

## 3. 데이터 흐름

```mermaid
flowchart LR
    GOAL["/set_goal"] --> NAV
    NAV["nav_node"] -->|"/cmd_vel"| BR
    SAFE["safety_supervisor"] -->|"/emergency_stop"| BR
    BR -->|"/diagnostics"| SAFE
    BR -->|"/wheel/odom (prior)"| VIO
    BR -->|"/odom"| NAV
    CAMUDP["camera_udp_node"] -->|"/camera/image_raw"| VIO
    BR -->|"/imu/data_raw"| VIO
    VIO -->|"/vio/odom"| SLAM
    SLAM -->|"map→odom TF, /map"| NAV

    subgraph APP["Application Layer"]
        NAV
        VIO["vio_node"]
        SLAM["slam_node"]
        SAFE
    end

    subgraph ROSIF["ROS 2 Interface Layer"]
        BR["stm32_bridge"]
        CAMUDP
    end

    subgraph EMB["Embedded Layer (STM32)"]
        FW["firmware: PID + odom + IMU + failsafe"]
    end

    subgraph HW["Hardware Layer"]
        CAMS(["UDP Camera Sender"])
        IMUHW["ICM-20948 (SPI1)"]
        ENC["엔코더 A/B 11PPR"]
        DRV["L298N"]
        MOT["좌/우 JGA25-370"]
    end

    BR <-->|"UART 230400 (USART1 remap)"| FW
    CAMS -->|"UDP JPEG 30fps"| CAMUDP
    IMUHW --> FW
    ENC --> FW
    FW --> DRV --> MOT --> ENC
```

### 제어 루프

```
goal → nav_node → /cmd_vel → stm32_bridge → UART(SET_VEL)
  → STM32 PWM(1kHz, PID는 P1-4) → L298N → 모터 → 엔코더(A/B quadrature)
  → STM32 odom(50Hz) + IMU(200Hz) → UART → bridge → /odom, /wheel/odom, /imu/data_raw
  → vio_node → slam_node → nav_node (폐루프)
```

## 4. 핀 매핑 및 통신 — 실제 펌웨어 기준

> `firmware/Test/Core/Src/{main.c, motor.c, imu.c, stm32f1xx_hal_msp.c}`에서 확인한 값입니다. 기존 설계안과 다른 부분은 §4.3에 정리했습니다.

### 4.1 핀 배치 (펌웨어 확인값 + 결정된 변경)

| 신호 | STM32 핀 | 현재 펌웨어 | 결정된 목표 |
|---|---|---|---|
| IMU SPI1 | PA5(SCK) / PA6(MISO) / PA7(MOSI) | master, 8bit, prescaler 64, NSS software | 유지 |
| IMU CS | PA4 (GPIO output) | 소프트웨어 CS | 유지 |
| IMU DRDY | — | 사용 안 함 (polling) | 유지, 주기만 **50Hz → 200Hz** |
| Pi↔MCU UART | **USART1 remap PB6(TX)/PB7(RX)** | 115200, RX DMA + TX mutex | **230400** |
| 디버그 UART | USART2 PA2/PA3 | 115200, `printf` retarget | 유지 |
| 좌/우 PWM | **TIM1 CH2(PA9) / CH3(PA10)** | PSC=71, ARR=1000 → 1kHz | 유지 |
| 좌/우 엔코더 | TIM3 remap CH1(PC6)/CH3(PC8) | **단일 채널 IC**, 방향은 명령 부호로 추정 | **A/B quadrature 2채널** (예: CH1/CH2=PC6/PC7, CH3/CH4=PC8/PC9) |
| L298N 방향 | PC0/PC1(좌), PC2/PC3(우) | GPIOC output push-pull | 유지 |
| PC4 | GPIOC output, SET됨 | 용도 미확인 | 확인 필요 |

- USART1은 `__HAL_AFIO_REMAP_USART1_ENABLE()`로 PB6/PB7에, TIM3은 `__HAL_AFIO_REMAP_TIM3_ENABLE()`로 PC6~PC9에 매핑됩니다. PA9/PA10 PWM과 충돌하지 않습니다.
- A/B quadrature 전환 후 `2059.2 count/rev`(11PPR × 4 × 46.8)가 유효해집니다 (`P1-3`에서 실측 검증).

### 4.2 확정 프레임 형식 (R0-3)

`AA55 | SEQ | MSG_ID | LEN | PAYLOAD | CRC16`

| 필드 | 크기 | 규칙 |
|---|---|---|
| SOF | 2B | `0xAA 0x55`, 재동기용 |
| SEQ | 1B | 0~255 순환, 유실 검출 |
| MSG_ID | 1B | 0=IMU, 1=ODOM, 2=VEL, 3=STATUS, 4=ACK, 5=NACK, 6=ESTOP |
| LEN | 1B | PAYLOAD 바이트 수. payload에 `0xFF`가 있어도 끝 구분 가능 |
| PAYLOAD | LEN | 메시지별 데이터 |
| CRC16 | 2B | CRC-16/XMODEM (poly `0x1021`, init `0x0000`, non-reflected, xorout 0). 계산 범위 `SEQ~PAYLOAD`, 전송 순서 **low byte first** |

예시 바이트 열 — IMU (SEQ=`0x07`, LEN=`0x1C`=28):

```
AA 55 07 00 1C [timestamp 4B][gx 4B][gy 4B][gz 4B][ax 4B][ay 4B][az 4B] [CRC_lo] [CRC_hi]
```

- VEL (ID=2): LEN=`0x08`, float 좌/우 → 프레임 15B
- ODOM (ID=1): LEN=`0x08`, int32 좌/우 → 프레임 15B
- IMU 타임스탬프는 MCU tick 또는 unix epoch(§9에서 확정)

| 메시지 | 주기 | 프레임 크기 | 사용량 |
|---|---|---|---|
| IMU (ID=0) | 200Hz | 35B | 7000 B/s |
| ODOM (ID=1) | 50Hz | 15B | 750 B/s |
| VEL (ID=2) | 50Hz | 15B | 750 B/s |
| **합계** | | | **약 8500 B/s (85kbps)** |

→ 230400bps(23040 B/s) 기준 사용률 약 **37%**입니다. 상세 문서 `docs/UART_PROTOCOL.md`는 `P0-6`에서 작성합니다.

### 4.3 R0-3 결정 상태

| 항목 | 사용자 결정 | 반영 위치 |
|---|---|---|
| Pi↔MCU UART | **230400 유지** (USART1 remap PB6/PB7) | `D-07`, `P1-6`, §4.2 |
| IMU 출력률 | **200Hz** (polling 유지) | `D-09`, `P1-1`, §2 |
| 엔코더 | **A/B quadrature** → 2059.2 count/rev | `P0-2`, `P1-3`, §4.1 |
| UART 프레임 | **B 채택**: `AA55\|SEQ\|ID\|LEN\|PAYLOAD\|CRC16` + IMU timestamp 4B | `D-07`, §4.2 |
| 모터 PID | **`P1-4`로 미룸** | `P1-4` |
| MCU failsafe | **포함** (`P1-7`) | `P1-7`, §6 |

> 게이트 `R0-3`은 **승인됨**입니다. 잔여 항목은 `P0-4`에서 다룹니다.

## 5. TF 발행 권한

| 단계 | `odom→base_link` | `map→odom` | 엔코더 odom |
|---|---|---|---|
| Phase 2 | `stm32_bridge` (바퀴 odom) | 없음 | `/odom` |
| **Phase 4** | `vio_node` | 없음 | **`/wheel/odom`으로 분리 → VIO prior** |
| Phase 5+ | `vio_node` | `slam_node` | 동일 |

- 엔코더 odom은 VIO의 **스케일/속도 prior**로 투입해 SLAM 성능을 높입니다. 단 `odom→base_link` 발행자는 VIO **하나만** 유지해 TF 충돌을 막습니다.
- ORB-SLAM3은 wheel odom을 직접 입력받지 못하므로 **VIO 퓨전 결과를 경유**합니다.
- Phase 2 → 4 전환 시점은 `R4-1`에서 재확인합니다.

## 6. 안전 2계층

| 계층 | 주체 | 감지 대상 | 대응 |
|---|---|---|---|
| 소프트웨어 | `safety_supervisor` (Pi) | 노드 하트비트, VIO/SLAM 실패, 목표 미도착 무한 반복 | `/emergency_stop` 발행, 로그 |
| 하드웨어 | STM32 failsafe | **UART 연결 끊김**, 명령 timeout 500ms, ESTOP 프레임, 저전압 | 모터 PWM 즉시 0 |

- UART가 끊기면 Pi의 소프트웨어 supervisor는 명령을 보낼 수 없으므로 **MCU가 독립적으로 정지**해야 합니다. 두 계층은 서로를 대체하지 않고 상호 보완합니다.

## 7. Phase별 노드 활성화

| Phase | 활성 계층/노드 | 검증 초점 |
|---|---|---|
| 1 | Embedded + Hardware | 주행, 엔코더, IMU, PID, failsafe |
| 2 | + `stm32_bridge` + URDF/RViz | ROS 제어, odom 시각화 |
| 3 | + `camera_udp_node`, IMU 파이프라인 | 센서 동기화, rosbag 재현 |
| 4 | + `vio_node` | 궤적 추정, ATE/RPE |
| 5 | + `slam_node` | 맵, 루프 클로저, 재위치 |
| 6 | + `nav_node`, `safety_supervisor` | Goal → 주행 → 도착, 안전 |
| 7 | 전체 | 통합/성능/재현 |

## 8. R0-3 주석 처리 결과

| 주석 | 결정 |
|---|---|
| "MCU에 IMU 달거임" / "SPI1임" | 반영. SPI1 유지 + 펌웨어 실제 핀맵 반영(§4). UART는 USART1 remap PB6/PB7, **230400으로 변경**. 타임스탬프/CRC는 프레임 결정 후 `P0-6` 과제 |
| "카메라는 UDP 통신으로 할거임" | 반영. `camera_udp_node`가 UDP JPEG 수신. 세부 포트/패킷 형식은 `P0-4`에서 확정 |
| "Hardware Layer 식으로" | 반영. 4계층 구조 + 부품 매핑 표(§1) |
| "노드 분리 괜찮아" | 노드 분리 유지 |
| "IMU MCU 연결 지연?" | **200Hz로 결정**. polling 유지, IMU 프레임에 MCU timestamp 4B 포함. 지연 약 3~6ms + 지터 보상 필요 |
| "엔코더 odom도 같이 쓰면?" | 반영. `/wheel/odom` → VIO prior → SLAM (§5) |
| "안전 둘 다 있어야지" | 반영. Pi supervisor + MCU failsafe 2계층(§6) |
| "제어 100Hz까지 안가도 될듯" | 반영. odom 50Hz + IMU 200Hz. **PID는 `P1-4`로 미룸**, MCU failsafe는 `P1-7`에 포함 |
| "프레임은 뭐지" → 선택 | **B 채택**: `AA55\|SEQ\|ID\|LEN\|PAYLOAD\|CRC16`, CRC-16/XMODEM, IMU timestamp 4B (§4.2) |

## 9. 다음 단계 및 미확정 사항

`P0-4`(토픽/메시지/서비스/QoS 명세)에서 아래를 확정합니다.

- [ ] UDP 카메라 송신 장치명 / 포트 번호
- [ ] UDP 패킷 형식: `frame_id | frag_idx | frag_cnt | timestamp | payload` (JPEG는 MTU 초과 → 분할 필수)
- [ ] `camera_info` 캘리브레이션 값 전달 방법 (UDP 내 전송 vs ROS 파라미터 파일)
- [ ] IMU 타임스탬프 단위/에포크 (MCU tick vs unix epoch) 및 오프셋 보정 방법

`R0-3` 결정 완료 (§4.3 참조):

- [x] UART 230400 — 사용자 결정 반영
- [x] IMU 200Hz — 사용자 결정 반영 (polling 유지)
- [x] 엔코더 A/B quadrature — 사용자 결정 반영 (2059.2 count/rev)
- [x] UART 프레임 B — `AA55|SEQ|ID|LEN|PAYLOAD|CRC16` + IMU timestamp 4B
- [x] PID `P1-4`로 미룸 / failsafe `P1-7` 포함 — 사용자 결정 반영

남은 확인 항목:

- [ ] PC4 GPIO 용도 확인 (코드에서 SET만 됨)
- [ ] IMU 타임스탬프 에포크 (MCU tick vs unix epoch) — `P0-4`에서 결정
