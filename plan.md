# 실행 계획서 (plan.md)

- 기준 문서: `project.md` (Phase 0 ~ Phase 7 Top-down Roadmap)
- 작성일: 2026-11-24
- 원칙: **한 번에 한 단계만 진행**하고, 단계가 끝나면 반드시 사람 검토 게이트를 통과한 뒤 다음 단계로 넘어간다.

---

## 0. 이 문서 사용법

1. 모든 작업은 `P<phase>-<n>` ID를 가진다. (예: `P1-4` = Phase 1의 4번째 작업)
2. 각 작업은 항상 `목적 → 선행 조건 → 작업 → 산출물 → 검증 → 리뷰 포인트` 순서로 기술한다.
3. 작업이 끝나면 `R<phase>-<n>` 게이트에서 **사용자 검토/승인**을 받는다. 승인 전에는 다음 작업을 시작하지 않는다.
4. 설계 선택이 필요한 항목은 `D-xx`로 등록한다. **권장안은 제시하되 최종 결정은 해당 게이트에서 사용자가 확정**한다.
5. 진행 상태는 `§5 게이트 요약` 표의 체크박스로 갱신한다.
6. 하드웨어/ROS 버전 등이 아직 확정되지 않았으므로, 이 문서의 값은 **기본안(권장안)** 이다. 게이트에서 수정하면 이후 모든 문서에 반영한다.

---

## 1. 현재 상태

| 항목 | 상태 |
|---|---|
| 작업 경로 | `/home/tm/dev_ws/mono_imu_autonomous_robot` |
| 기존 파일 | `project.md` 1개, `.github/` (빈 디렉터리) |
| git 저장소 | 아직 초기화되지 않음 (D-05에서 결정) |
| 진행 단계 | Phase 0 시작 전 |
| 하드웨어/소프트웨어 사양 | `D-01`~`D-06` 확정 (STM32F103RBT6 / Pi 5 8GB / Jazzy / RPi Cam 3 / ICM-20948 / JGA25-370) |
| 확정된 인터페이스 | 없음 (`D-06` ~ `D-09` 미결정) |

---

## 2. 진행 규칙 (리뷰 프로토콜)

- **리뷰 단위**: Phase 전체가 아니라 개별 `P-step`. 단, 게이트는 Phase 경계와 주요 분기점에 둔다.
- **검토 방법**: 산출물 파일을 열어 보고, 검증 명령의 출력을 확인하고, `리뷰 포인트`에 답한다.
- **승인 표현**: `§5` 표에서 `[ ]` → `[x]`로 변경 + 게이트별 한 줄 코멘트를 `docs/REVIEW_LOG.md`에 기록.
- **보류/수정 요청**: 해당 `P-step`을 되돌리고 같은 ID로 재진행한다. 다음 단계로 넘어가지 않는다.
- **재현 원칙**: 모든 실험/검증은 명령, Parameters, rosbag 경로, 날짜를 `docs/EXPERIMENTS.md`에 남긴다.

---

## 3. 결정 레지스트 (Decision Register)

| ID | 결정 항목 | 권장안 (미확정) | 확정 게이트 | 상태 |
|---|---|---|---|---|
| D-01 | MCU / 보드 | STM32F103RBT6, 3.3V UART 지원 | R0-2 | 확정 |
| D-02 | SBC / OS | Pi 5 8GB, Ubuntu 24.04 Jazzy | R0-2 | 확정 |
| D-03 | ROS 2 배포판 | Ubuntu 24.04 + Jazzy | R0-1 | 확정 |
| D-04 | 센서 | **별도 UDP 카메라 송신기**(640×480@30 JPEG) + IMU(ICM-20948, **STM32 SPI1**) | R0-2 / R0-3 | 확정 |
| D-05 | 저장소 전략 | git init → GitHub public, main + feature 브랜치 | R0-1 | 확정 |
| D-06 | 구동 방식 | 2바퀴 차동 + JGA25-370 12V 130RPM(출력축), 감속비 46.8:1, 엔코더 11PPR, L298N 드라이버, Pi 5 전원 12V→5V/5A | R0-2 | 확정 |
| D-07 | UART 물리/프레임 | **USART1 remap PB6/PB7, 230400, 8N1, 3.3V TTL**. 프레임 형식은 검토 중 (현재 `0xAA\|CMD\|payload\|0xFF` / 목표안 `SOF\|SEQ\|ID\|LEN\|CRC16`) | R0-6 | 미결정 |
| D-08 | 단위/좌표계 | REP-103( rad, m, s ), REP-105( map→odom→base_link→sensor ) | R0-5 | 미결정 |
| D-09 | 제어 주기 | **IMU 200Hz**, odom/피드백 50Hz, PWM 1kHz. **PID는 `P1-4`로 미룸**, MCU failsafe는 `P1-7`에 포함 | R0-4 | 미결정 |
| D-10 | VIO/SLAM 구현 | 오프라인 검증 후 ORB-SLAM3 or VINS-Fusion 중 선택 | R4-1 / R5-1 | 미결정 |
| D-11 | 내비게이션 구현 | Nav2 vs 경량 커스텀 스택 (지도/센서 구성에 따라 결정) | R6-1 | 미결정 |
| D-12 | 장애물 감지 센서 | 모노 카메라만 vs 초음파/ToF 추가 | R6-4 | 미결정 |

> 게이트에서 결정할 때는 `권장안`을 그대로 승인하거나, 변경 사항을 `docs/DECISIONS.md`에 근거와 함께 기록한다.

---

## 4. 단계별 실행 계획

### Phase 0 — System Design (현재 단계)

목표: 하드웨어/소프트웨어 구조와 인터페이스를 문서로 확정한다.
완료 기준: `docs/SYSTEM_DESIGN.md`, `docs/INTERFACES.md`, `docs/UART_PROTOCOL.md` 승인.

#### P0-1 저장소 및 개발 환경 초기화
- **목적**: 코드/문서/실험 데이터를 관리할 골격과 버전 기준을 만든다.
- **선행 조건**: 없음
- **작업**:
  1. `git init`, `.gitignore` 작성 (`build/`, `install/`, `log/`, `*.bag`, `firmware/build/` 등)
  2. 디렉터리 골격 생성: `docs/`, `ros2_ws/src/`, `firmware/`, `scripts/`, `bags/`(로컬만)
  3. `README.md`에 프로젝트 한 줄 요약 + 현재 단계(Phase 0) 기록
  4. ROS 2 배포판/Ubuntu 버전을 `D-03`으로 확정하고 문서화
- **산출물**: `.gitignore`, `README.md`, `docs/ENVIRONMENT.md`, 디렉터리 구조
- **검증**: `git status`, `git log -1`, `ls -R`로 구조 확인
- **리뷰 포인트**: "Pi 5 8GB + Ubuntu 24.04 + ROS 2 Jazzy 조합이 실제 설치 가능한가? (`D-02`, `D-03`)"

#### P0-2 하드웨어 구성 및 제약 확정
- **목적**: 실제 부품 없이 설계가 흔들리지 않도록 BOM과 물리 제약을 고정한다.
- **선행 조건**: P0-1
- **작업**:
  1. `D-01`, `D-02`, `D-04`, `D-06` 확정 (MCU, SBC, 카메라/IMU, 구동계)
  2. `docs/HARDWARE_BOM.md` 작성: 부품명, 수량, 인터페이스, 전원, 데이터시트 링크
  3. 제약 명시: 배터리 전압, 모터 최대 전류, 카메라 해상도/fps, IMU 출력률, UART 레벨
  4. **전원 예산 표 작성**: 모터 12V / Pi 5 5V·5A(USB-C PD 또는 12V→5V 벅) / STM32·IMU 3.3V, 공통 GND 확인
  5. **JGA25-370 감속비(기어비)와 정지 전류 확인** → `P1-3` tick→m 변환과 드라이버 선정의 입력값
  6. "부품 미보유 시 대체품"도 함께 기록
- **산출물**: `docs/HARDWARE_BOM.md`
- **검증**: BOM의 모든 부품이 인터페이스 측면에서 서로 맞는지 교차 확인 (전압/핀수/프로토콜)
- **리뷰 포인트**: "정말 보유한 부품인가? 3.3V/5V 레벨 문제는 없는가? L298N 전압 강하/발열은 감당 가능한가?"

> **P0-2 확정 제약값**: `r=32.5mm`, `L=198mm`, 감속비 46.8:1, `2059.2 count/rev` = `10084.1 count/m`, `v_max` 이론 0.442 m/s(초기 제한 0.3 m/s), `omega_robot_max` 이론 4.47 rad/s. 상세는 `docs/HARDWARE_BOM.md` §4. 미정: 모터 정지 전류, 로봇 질량. **A/B quadrature 사용 확정** — 현재 펌웨어의 단일 채널 IC를 A/B로 변경해야 함 (`P1-3`).

#### P0-3 시스템 아키텍처 및 데이터 흐름 (게이트 R0-3)
- **목적**: 노드 경계와 데이터 흐름을 그림으로 확정한다.
- **선행 조건**: P0-2
- **작업**:
  1. 노드 목록 정의: `stm32_bridge`(UART + IMU 발행), `camera_udp_node`(UDP 수신), `vio_node`, `slam_node`, `nav_node`, `safety_supervisor` (`imu_node`는 제거)
  2. 데이터 흐름도 작성: `cmd_vel → bridge → MCU → 모터 → 엔코더 → odom → VIO → SLAM → Nav → cmd_vel`
  3. 각 노드의 실행 위치(Pi / MCU)와 실패 시 영향 기록
  4. mermaid 다이어그램을 `docs/SYSTEM_DESIGN.md`에 삽입
- **산출물**: `docs/SYSTEM_DESIGN.md`
- **검증**: 다른 사람이 다이어그램만 보고 "명령이 어디로 흐르는가"를 설명할 수 있는지
- **리뷰 포인트**: "노드 분리가 과하지 않은가? MCU가 맡은 역할이 과도하지 않은가?"

#### P0-4 토픽/메시지/서비스/QoS 명세
- **목적**: 코드 작성 전에 인터페이스를 고정해 재작업을 막는다.
- **선행 조건**: P0-3
- **작업**:
  1. 토픽 표 작성: 이름, 타입, 발행자/구독자, 주기, QoS(reliable/best_effort, depth), 프레임 ID
     - `/cmd_vel` (geometry_msgs/Twist), `/odom` (nav_msgs/Odometry), `/wheel/odom`, `/imu/data_raw`(**stm32_bridge 발행**), `/camera/image_raw`(**UDP 수신 노드 발행**), `/camera/camera_info`, `/tf`, `/diagnostics`
  2. **UDP 카메라 인터페이스 명세**: 송신 장치/포트, JPEG 프레임 분할(frame_id + frag_idx/frag_cnt + timestamp), 재조립 및 드롭 로깅
  3. 서비스/액션 정의: `/emergency_stop`, `/reset_odometry`, `/set_goal` (D-11 결정 전까지는 인터페이스만)
  4. 파라미터 목록: 최대 속도(0.3 m/s), 가/감속, wheel_base(0.198m), wheel_radius(0.0325m), UART 포트/타임아웃, **baud 230400**
  5. `D-09` 제어 주기 확정: **IMU 200Hz / odom·피드백 50Hz / bridge 50Hz**, PID는 `P1-4`, failsafe는 `P1-7`
- **산출물**: `docs/INTERFACES.md`
- **검증**: 토픽 이름/타입/프레임 ID가 `P0-3` 다이어그램과 1:1 대응하는지 확인
- **리뷰 포인트**: "나중에 바꾸기 어려운 토픽 이름/타입/단위가 여기 확정된다. 이상 없는가?"

#### P0-5 TF 프레임 트리 설계
- **목적**: 센서/로봇 좌표계를 REP-105에 맞춰 고정한다.
- **선행 조건**: P0-4
- **작업**:
  1. 트리 확정: `map → odom → base_link → {camera_link → camera_optical_frame, imu_link, wheel_*_link}`
  2. 각 변환의 발행자(static vs dynamic), 단위, 축 방향 명시 (`D-08` 확정)
  3. `map→odom`은 SLAM/VIO가 나중에 갱신한다는 점을 명시 (초기에는 identity)
  4. `docs/TF.md`에 트리 다이어그램 + 변환 표 작성
- **산출물**: `docs/TF.md`
- **검증**: RViz에서 `base_link` 기준 카메라/IMU 프레임 위치가 실제 로봇과 일치하는지 나중에 확인할 체크리스트 작성
- **리뷰 포인트**: "카메라 optical frame 축 규약(REP-103: x-right, y-down, z-forward)을 지켰는가?"

#### P0-6 UART 프로토콜 v0.1 설계
- **목적**: Pi↔STM32 사이의 바이트 단위 규약을 정의한다.
- **선행 조건**: P0-4, P0-5
- **작업**:
  1. 프레임 구조 확정: 현재 펌웨어 `0xAA | CMD(1B) | payload | 0xFF` (SEQ/CRC 없음) vs 목표안 `SOF(0xAA55) | SEQ | MSG_ID | LEN | PAYLOAD | CRC16` — **사용자 설명 대기 중**
  2. 메시지 표: `CMD_VEL`, `CMD_ODOM`, **`CMD_IMU`(200Hz, MCU 타임스탬프 포함 예정)**, 목표안 `STATUS`, `ACK/NACK`, `ESTOP`
  3. 규칙 정의: **baud 230400**, 타임아웃(예: 500ms), 재전송/시퀀스 처리, 바이트 스터핑 필요 여부
  4. 실패 시 동작 명시: 명령 끊김 → 모터 정지, CRC 오류 → 프레임 폐기
  5. `docs/UART_PROTOCOL.md` 작성 + 예시 바이트 열 추가
- **산출물**: `docs/UART_PROTOCOL.md`
- **검증**: 예시 바이트 열을 손으로 CRC 계산해 문서 값과 일치 확인
- **리뷰 포인트**: "ACK가 필요한 명령은 어떤 것인가? 타임아웃 500ms가 제어 주기에 맞는가?"

#### P0-7 검증 전략 및 테스트 하네스 설계
- **목적**: 다음 Phase부터 "검증 가능한 단계"로 진행하기 위한 규칙을 만든다.
- **선행 조건**: P0-4
- **작업**:
  1. 테스트 피라미드 정의: MCU 단위 테스트(파서/PID) → HIL(UART) → ROS 노드 테스트 → 실주행
  2. 재현 프로토콜: rosbag 파일 명명 규칙, 실험 메타데이터(날짜/부품/파라미터) 양식
  3. 성능 지표 초안 정의: ATE/RPE, 주행 성공률, control loop z, CPU 사용률
  4. `docs/TEST_PLAN.md`, `docs/EXPERIMENTS.md` 템플릿 작성
- **산출물**: `docs/TEST_PLAN.md`, `docs/EXPERIMENTS.md`
- **검증**: 템플릿으로 빈 실험 한 건을 작성해 보기
- **리뷰 포인트**: "각 Phase의 완료 기준이 이 지표로 판정 가능한가?"

#### P0-8 Phase 0 설계 확정 (게이트 R0)
- **목적**: Phase 0 산출물 전체를 승인받는다.
- **작업**: `docs/SYSTEM_DESIGN.md`, `docs/INTERFACES.md`, `docs/TF.md`, `docs/UART_PROTOCOL.md`, `docs/DECISIONS.md` 최종 검토
- **검증**: 결정 레지스트 `D-01`~`D-09`가 모두 "확정"으로 변경됨
- **리뷰 포인트**: "이 문서들을 3개월 뒤의 내가 읽고 다시 만들 수 있는가?"

---

### Phase 1 — Embedded Robot Base

목표: STM32로 안정적인 주행 플랫폼을 만든다.

#### P1-1 STM32 프로젝트 골격 및 핀맵/클록 설정
- **목적**: CubeMX 기반 프로젝트를 생성하고 PWM/엔코더/UART 핀을 배정한다.
- **선행**: R0 승인, P0-6
- **작업**: 펌웨어 현재값 확인 — TIM1 CH2/CH3(PA9/PA10, PWM 1kHz), TIM3 remap CH1/CH3(PC6/PC8, 단일 채널 IC), USART1 remap PB6/PB7, USART2(PA2/PA3, printf), SPI1(PA5/PA6/PA7, CS=PA4), L298N 방향 PC0~PC3. **변경 필요**: baud 230400, 엔코더 A/B 2채널, IMU 200Hz. PID는 `P1-4`, failsafe는 `P1-7`
- **산출물**: `firmware/` 프로젝트, `docs/firmware/PINMAP.md`
- **검증**: 보드 전원 인가 후 GPIO 토글/LED 확인, UART로 "hello" 출력
- **리뷰**: "핀 충돌 없는가? 보드 클록이 UART baud 오차 범위 내인가?"

#### P1-2 PWM 및 모터 드라이버 방향 검증
- **작업**: L298N ENA PWM(1kHz부터) + IN1/IN2 방향 제어, 듀티 0→25→50% 인가, 좌/우 회전 방향 및 최소 유효 듀티 측정, 5V 로직 점퍼/레벨 확인
- **산출물**: `docs/EXPERIMENTS.md`에 모터 방향 표
- **검증**: 바퀴가 "앞" 명령에 둘 다 앞으로 도는지 육안/마커로 확인, 정지 전류 측정
- **리뷰**: "반대로 도는 바퀴가 있는가? L298N 발열/전압 강하는 없는가? 3.3V GPIO가 IN/ENA를 정상 구동하는가?"

#### P1-3 엔코더 측정 및 단위 변환
- **작업**: 엔코더 카운트 읽기, tick→rad→m 변환 상수 계산. **A/B quadrature 확정** → `11PPR × 4 × 46.8 = 2059.2 count/rev`, `rad_per_count ≈ 0.003052`. 현재 펌웨어는 단일 채널 IC이므로 **A/B 2채널 입력으로 수정** 필요 (TIM3 CH1=PC6, CH2=PC7 / CH3=PC8, CH4=PC9 중 배정). wheel_radius 실측 시 반영
- **산출물**: `firmware/Motor/` 소스, `docs/firmware/ENCODER_CALIB.md`
- **검증**: 수동으로 바퀴 1회전 → 카운트 값이 이론치와 ±2% 이내
- **리뷰**: "카운트 증가 방향이 모터 방향과 일치하는가?"

#### P1-4 Wheel PID (속도 폐루프)
- **작업**: 목표 rpm → PWM 변환, PI/PID 구현, 샘플 주기 10ms, anti-windup
- **산출물**: PID 소스 + 튜닝 로그(CSV)
- **검증**: 목표 0.2/0.5 m/s에서 정상 상태 오차 ±5% 이내, 오버슈트 기록
- **리뷰**: "튜닝 근거가 로그에 남았는가? 포화(PWM 100%) 처리 했는가?"

#### P1-5 Odometry 계산
- **작업**: 차동 구동 운동학으로 `(x, y, theta)` 적분, `P0-5` 좌표계/단위 준수
- **산출물**: `firmware/Odometry/`, `docs/firmware/ODOM_MATH.md`
- **검증**: 1m 직진 후 보고값 0.95~1.05m, 제자리 90° 회전 후 ±3°
- **리뷰**: "회전 방향 부호가 ROS 좌표계(x-forward, z-up)와 맞는가?"

#### P1-6 UART 링크 및 프로토콜 구현
- **작업**: 현재 115200/`0xAA-CMD-0xFF` 구현을 **230400 + `P0-6` 확정 프레임**으로 업그레이드 (SEQ/CRC/타임스탬프/IMU 200Hz). 참고: UART 송신 mutex·DMA 수신은 이미 구현됨
- **산출물**: `firmware/UART/`, 파서 단위 테스트
- **검증**: PC 시리얼 모니터로 프레임 전송 → ACK 확인, CRC 오류 주입 시 폐기 확인
- **리뷰**: "받은 명령이 실제 모터로 전달되는가? Pi 5의 `/dev/serial0`(GPIO 14/15) 활성화 + STM32와 3.3V 직접 연결/공통 GND가 맞는가?"

#### P1-7 Failsafe / Watchdog
- **작업**: 명령 timeout 타이머, 시리얼 끊김 감지, 배터리 전압 모니터링, IWDG 설정
- **산출물**: `firmware/Safety/`, `docs/firmware/FAILSAFE.md`
- **검증**: 명령 중단 후 500ms 이내 정지, 시리얼 케이블 뽑기 테스트, 리셋 후 안전 상태 확인
- **리뷰**: "전원 재인가 시 모터가 즉시 돌지 않는가?"

#### P1-8 Phase 1 통합 주행 테스트 (게이트 R1)
- **검증 시나리오**: 직진 1m → 후진 1m → 제자리 회전 → 속도 유지 → timeout 정지
- **산출물**: `docs/EXPERIMENTS.md` 결과, 영상 1개
- **리뷰**: "`project.md` Phase 1 완료 기준(직진/후진/회전/속도제어/timeout 정지)을 모두 만족하는가?"

---

### Phase 2 — ROS 2 Integration

목표: Pi에서 ROS 2로 로봇을 제어하고 odom을 시각화한다.

#### P2-1 ROS 2 workspace 및 패키지 골격
- **작업**: `ros2_ws/src/`에 `stm32_bridge`, `robot_description`, `robot_bringup` 패키지 생성, `colcon build` 확인
- **산출물**: `ros2_ws/` 골격
- **검증**: `colcon build && source install/setup.bash && ros2 pkg list | grep stm32_bridge`
- **리뷰**: "패키지 이름/의존성이 `P0-3` 노드 구조와 맞는가?"

#### P2-2 stm32_bridge 노드 (시리얼 I/O + 파서)
- **작업**: 시리얼 열기/재연결, 프레임 파싱, ACK/SEQ 처리, ROS 파라미터로 포트/ baud 설정
- **산출물**: `stm32_bridge` 노드
- **검증**: MCU 없이 시리얼 루프백으로 가짜 프레임 주입 → 파서 로그 확인
- **리뷰**: "포트가 없을 때 노드가 죽지 않고 재시도 하는가?"

#### P2-3 /cmd_vel → 모터 명령 변환
- **작업**: Twist의 `linear.x`, `angular.z`를 좌/우 바퀴 속도로 변환, 초기 제한 `linear.x ≤ 0.3 m/s`, `angular.z ≤ 2.5 rad/s` (`P0-2` 유도값, 실측 후 조정)
- **산출물**: 변환 로직 + 단위 테스트
- **검증**: `ros2 topic pub /cmd_vel ...` 로 전송 후 MCU ACK 및 실제 바퀴 회전 확인
- **리뷰**: "제한 속도를 넘는 명령이 들어오면 클리핑 되는가?"

#### P2-4 /odom 퍼블리셔 및 TF 발행
- **작업**: MCU 피드백을 `nav_msgs/Odometry`로 변환, `odom→base_link` 발행, 공분산 설정
- **산출물**: odom 퍼블리셔
- **검증**: `ros2 topic echo /odom`, `ros2 topic hz /odom`, 로봇 밀었을 때 값 변화 확인
- **리뷰**: "공분산 값이 근거 없는 숫자는 아닌가? frametype/child_frame_id가 맞는가?"

#### P2-5 URDF 및 RViz2 시각화
- **작업**: `robot_description` URDF 작성, static transform(`base_link→camera_link`, `imu_link`) 설정, RViz config 저장
- **산출물**: URDF, RViz config
- **검증**: RViz에서 TF 트리 에러 없이 모델 표시, `ros2 run tf2_tools view_frames`
- **리뷰**: "`P0-5` TF 설계와 실제 URDF가 일치하는가?"

#### P2-6 teleop + rosbag 기록/재생 검증
- **작업**: teleop_keyboard로 주행, bag 기록, `ros2 bag play`로 odom 재현
- **산출물**: `bags/` 샘플, `docs/EXPERIMENTS.md` 기록
- **검증**: `ros2 bag info`로 토픽/주기 확인, 재생 시 RViz에서 동일 궤적
- **리뷰**: "재생만으로 검증 가능한 환경이 갖춰졌는가?"

#### P2-7 Phase 2 게이트 (R2)
- **검증**: "`project.md` Phase 2 완료 기준: ROS 명령 제어 + Odometry 시각화" 확인
- **리뷰**: "Phase 3 센서 부착 전에 고쳐야 할 아키텍처 문제가 있는가?"

---

### Phase 3 — Camera & IMU

목표: 동기화된 Camera/IMU 데이터를 안정적으로 얻는다.

#### P3-1 UDP 카메라 수신 노드 및 이미지 파이프라인
- **작업**: 별도 UDP 송신기로부터 JPEG 스트림 수신, frame_id/frag_idx/frag_cnt 기반 재조립, 드롭 프레임 로깅, `/camera/image_raw`, `/camera/camera_info` 발행 (640×480@30)
- **검증**: 송신기→수신 fps/드롭률 측정, rqt_image_view로 실시간 확인, 패킷 손실 주입 시 재조립 동작 확인
- **리뷰**: "송신 장치/포트/패킷 형식이 확정되었는가? 노출이 고정되어 반복 실험이 가능한가?"

#### P3-2 카메라 캘리브레이션
- **작업**: 체스보드/칼라보드로 `camera_calibration` 수행, `camera_info` yaml 저장 후 런치에 적용
- **검증**: 왜곡 보정 이미지에서 직선이 휘지 않음, 재투영 오류 수치 기록
- **리뷰**: "캘리브레이션 오류가 VO에 영향을 줄 수준은 아닌가?"

#### P3-3 IMU 드라이버 및 캘리브레이션 (STM32 측)
- **작업**: STM32 SPI1 드라이버 + DRDY 인터럽트(200Hz), IMU_DATA 프레임 전송, `stm32_bridge`가 `/imu/data_raw` 발행, bias/noise density 측정
- **검증**: 정지 상태에서 1분 수집 → bias 평균/표준편차 계산, 축 방향 확인(REP-103)
- **리뷰**: "IMU 축이 로봇 축과 맞는가? 중력 방향이 +z/-z 중 어느 쪽인가?"

#### P3-4 타임스탬프 및 Camera-IMU 동기화
- **작업**: MCU 타임스탬프(IMU) ↔ ROS 시간 오프셋 보정, UDP 패킷 타임스탬프(카메라) ↔ ROS 시간 보정, 지연 histogram 작성
- **검증**: `/imu/data_raw`와 `/camera/image_raw`의 time 차이 분포, 고정 지연 상수 기록 (예상 IMU 지연 3~6ms)
- **리뷰**: "MCU/UDP 양쪽 타임스탬프 오프셋 보정이 재현 가능한가? 지터가 VIO에 영향을 주는가?"

#### P3-5 센서 로깅 및 반복 실험 프로토콜
- **작업**: rosbag 기록 프로파일(토픽/압축 설정), 실험 메타데이터 양식 적용, 실패 재현용 bag 보관
- **검증**: 동일 조건 2회 기록 → bag info/타임스탬프 분포 비교
- **리뷰**: "`project.md` Phase 3 완료 기준(동기화된 기록 + 반복 실험)을 만족하는가?" (게이트 R3)

---

### Phase 4 — Visual Odometry / VIO

목표: 카메라+IMU로 이동을 추정하고 평가한다.

#### P4-1 오프라인 파이프라인 골격 및 데이터셋 준비
- **작업**: bag → 이미지/IMU/바퀴 odom 추출 스크립트, 타임스탬프 정렬, ground truth 정의
- **검증**: 동일 bag를 두 번 처리해 결과가 동일한지 확인
- **리뷰**: "GT(참값)를 무엇으로 잡았는가? (바퀴 odom/외부 측정/수동)"

#### P4-2 Feature Detection / Tracking / Optical Flow
- **작업**: 특징점 검출(KAZE/GFTT/ORB 등), LK optical flow, outlier 제거(RANSAC)
- **검증**: 이미지 sequence에서 tracking 길이/드롭률 시각화, 조명 변화 케이스 확인
- **리뷰**: "실내 조명/무질감 바닥에서 특징점이 충분한가?"

#### P4-3 Monocular VO 및 스케일 처리
- **작업**: Essential/Homography 분해로 상대 포즈, 바퀴 odom으로 스케일 초기화/보정
- **검증**: 직진/회전 각각에 대해 추정 궤적 시각화, 스케일 드리프트 수치화
- **리뷰**: "단안 스케일을 바퀴 odom에 의존해도 되는가? 슬립 상황은?"

#### P4-4 IMU Preintegration
- **작업**: 자이로/가속도 적분, 바이어스 보정, delta quaternion/position 계산
- **검증**: 정지/회전 구간에서 적분 오차가 이론 범위 내인지 확인
- **리뷰**: "preintegration이 VO 프레임과 시간 동기되는가?"

#### P4-5 Sensor Fusion (VIO)
- **작업**: EKF 또는 factor graph로 VO+IMU+wheel odom 융합 (구현 선택은 R4-1에서 승인)
- **검증**: 빠른 회전/급가감속에서 자세 발산 여부 확인, 융합 전/후 비교 그래프
- **리뷰**: "fusion이 실패할 때 fallback(바퀴 odom)이 있는가?"

#### P4-6 Trajectory Evaluation
- **작업**: EVO 등으로 ATE/RPE 계산, 실패 구간을 영상/그래프로 분석
- **검증**: 최소 3회 반복 주행에 대해 수치 표 작성
- **리뷰**: "`project.md` Phase 4 완료 기준(합리적인 trajectory 유지)을 수치로 판정했는가?" (게이트 R4)

---

### Phase 5 — Visual SLAM & Localization

목표: 지도 생성과 재방문 위치 인식을 검증한다.

#### P5-1 SLAM 구현 선택 (D-10 확정)
- **작업**: ORB-SLAM3 / VINS-Fusion / RTAB-Map 중 선택 기준표 작성 (의존성, 라이선스, IMU 지원, ROS 2 지원, 로봇 자원)
- **검증**: 선택안을 Pi에서 빌드 가능한지 사전 확인
- **리뷰**: "선택 근거가 문서화되었는가? 나중에 바꿀 비용은?"

#### P5-2 Keyframe / Map Point 관리 및 로컬 맵
- **작업**: 키프레임 선택 기준, 리소컬 최적화, 로컬 맵 시각화
- **검증**: 주행 중 키프레임/맵 포인트 수가 안정적으로 증가하는지 확인
- **리뷰**: "메모리 증가가 Pi에서 감당 가능한가?"

#### P5-3 Loop Closure 및 Drift 분석
- **작업**: 재방문 감지, pose graph 최적화, 루프 전/후 드리프트 비교
- **검증**: 같은 장소 2회 통과 시 궤적이 닫히는지, ATE 개선 수치 확인
- **리뷰**: "잘못된 루프( false loop )가 발생하면 복구되는가?"

#### P5-4 맵 저장 / 재사용 / Relocalization
- **작업**: 맵 파일 저장·불러오기, 저장된 맵에서 초기 위치 찾기
- **검증**: 로봇 재시작 후 맵 로드 → 30초 내 위치 수렴
- **리뷰**: "맵 버전 관리(파일명/날짜/파라미터)가 되는가?"

#### P5-5 반복 이동 검증 (게이트 R5)
- **검증**: 동일 경로 3회 주행 → 지도 일관성 및 재위치 성공률 기록
- **리뷰**: "`project.md` Phase 5 완료 기준(지도 생성 + 재방문 인식) 충족 여부"

---

### Phase 6 — Autonomous Navigation

목표: Goal을 주면 경로를 계획하고 주행해 도착한다.

#### P6-1 Goal Interface 및 미션 정의 (D-11)
- **작업**: `/set_goal` (2D pose 또는 waypoint 리스트) 정의, 성공/실패 판정 기준(예: 위치 오차 0.2m 이하 1초 유지)
- **검증**: goal 발행 → ROS 로그에 수신/상태 전이 기록
- **리뷰**: "도착 판정 기준이 실험 공간 크기에 맞는가?"

#### P6-2 Path Planning 선택 및 구현
- **작업**: Nav2 사용 시 costmap/planner 파라미터, 커스텀 시 grid/A* 구현. 선택 근거 문서화
- **검증**: 장애물이 있는 맵에서 출발→목표 경로가 그려지는지 RViz 확인
- **리뷰**: "정적 맵만 쓰는가, 동적 장애물도 고려하는가?"

#### P6-3 Path Following 및 속도 프로파일
- **작업**: Pure Pursuit 등 경로 추종, 코너 감속, `/cmd_vel` 출력 연동
- **검증**: S자 경로 추종 오차(최대/평균) 기록, 속도 명령이 제한 내인지 확인
- **리뷰**: "경로 이탈 시 재계획 트리거가 있는가?"

#### P6-4 장애물 감지 전략 (D-12 확정)
- **작업**: 센서 선택(모노 깊이 추정 vs ToF/초음파 추가), 안전 거리/정지 로직 구현
- **검증**: 사람/박스 접근 시 감지→감속→정지까지 거리 측정
- **리뷰**: "모노만으로 감지 신뢰성이 부족하면 센서 추가가 필요한가?"

#### P6-5 Recovery Behavior 및 Safety Supervisor
- **작업**: 정지/후진/재계획 등 recovery, `/emergency_stop` 서비스, MCU failsafe와 양방향 연동
- **검증**: estop 발행 즉시 모터 정지, 장애물 제거 후 재개 동작 확인
- **리뷰**: "복구 동작이 무한 반복되지 않는가? 시도 횟수 제한 있는가?"

#### P6-6 제한 공간 통합 주행 테스트 (게이트 R6)
- **검증**: Goal 지정 → 경로 계획 → 주행 → 도착 시나리오 5회, 성공률 기록
- **리뷰**: "`project.md` Phase 6 완료 기준 충족 여부 + 실패 케이스 목록"

---

### Phase 7 — Reliability & Portfolio

목표: 재현 가능한 결과와 기술 근거를 정리한다.

#### P7-1 시스템 통합 테스트 시나리오 확정
- **작업**: `docs/TEST_PLAN.md`를 실제 시나리오로 채움(센서 부팅 순서 포함)
- **검증**: 콜드 스타트 → 자율주행 → 종료까지 체크리스트 통과

#### P7-2 Failure Handling 및 Fault Injection
- **작업**: 센서 끊김, UART 단절, SLAM 트래킹 lose, 목표 미도착 주입 테스트
- **검증**: 각 케이스에서 로봇이 안전 상태로 전이하는지 기록

#### P7-3 CPU / Memory Profiling
- **작업**: `ros2 topic hz`, `top/htop`, `perf` 등으로 노드별 자원 사용량 측정
- **검증**: 10분 주행 중 메모리 누수/드롭 프레임 여부 확인

#### P7-4 SLAM/VIO 성능 평가 및 반복 실험
- **작업**: ATE/RPE, 주행 성공률, 도착 오차를 최소 5회 실험에 대해 표로 정리
- **검증**: 평균/표준편차와 실패 원인 분류

#### P7-5 ROS Bag 분석 및 결과 재현 문서
- **작업**: 대표 bag 선정, 분석 스크립트/명령을 `scripts/`에 정리, `docs/RESULTS.md` 작성
- **검증**: 다른 사람이 `README`만 보고 분석 재현 가능한지

#### P7-6 GitHub 정리 및 포트폴리오 (게이트 R7)
- **작업**: README/다이어그램/영상/성능 표 통합, `.github`에 이슈 템플릿 추가
- **검증**: "`project.md` Phase 7 완료 기준: 재현 가능한 실험 결과 + 기술적 근거" 확인

---

## 5. 게이트 요약 및 승인 상태

| 게이트 | 시점 | 승인 기준 | 상태 |
|---|---|---|---|
| R0-1 | P0-1 | 개발 환경/버전, git 전략 확정 | [x] |
| R0-2 | P0-2 | 하드웨어 BOM/제약 확정 | [x] |
| R0-3 | P0-3 | 노드 구조/데이터 흐름/TF 권한 확정 | [ ] |
| R0-4 | P0-4 | 토픽/주기/파라미터 확정 | [ ] |
| R0-5 | P0-5 | TF 트리/단위/축 확정 | [ ] |
| R0-6 | P0-6 | UART 프레임/타임아웃 확정 | [ ] |
| R0 | P0-8 | Phase 0 문서 전체 승인 | [ ] |
| R1 | P1-8 | 직진/후진/회전/속도제어/timeout 정지 | [ ] |
| R2 | P2-7 | ROS 제어 + odom/RViz 시각화 | [ ] |
| R3 | P3-5 | 동기화 센서 기록 + 반복 실험 가능 | [ ] |
| R4 | P4-6 | VO/VIO 궤적 수치 평가 완료 | [ ] |
| R5 | P5-5 | 지도 생성 + 재방문 인식 검증 | [ ] |
| R6 | P6-6 | Goal → 계획 → 주행 → 도착 (5회) | [ ] |
| R7 | P7-6 | 재현 가능한 포트폴리오 완성 | [ ] |

> 게이트 통과 시 `docs/REVIEW_LOG.md`에 `날짜 / 게이트 / 결정 / 코멘트`를 한 줄 기록한다.

---

## 6. 공통 작업 규칙

- **커밋 메시지**: `[P0-4] Add topic and QoS specification` 처럼 step ID를 접두어로 쓴다.
- **문서 우선**: 코드보다 인터페이스 문서를 먼저 갱신하고, 코드가 문서와 어긋나면 문서를 고친다.
- **버전 고정**: OS/ROS/주요 라이브러리 버전을 `docs/ENVIRONMENT.md`에 기록하고 업그레이드는 별도 step으로 처리한다.
- **실험 기록**: 모든 검증은 `docs/EXPERIMENTS.md`에 명령·파라미터·결과·영상/bag 경로를 남긴다.
- **안전 최우선**: 어느 Phase에서든 모터가 예기치 않게 움직이면 즉시 전원 차단 후 `P1-7` failsafe를 재검토한다.
- **한 단계씩**: `R` 게이트 미승인 상태에서 다음 `P-step`을 시작하지 않는다.

---

## 7. 바로 시작할 일

1. **프레임 형식 결정** (사용자 질문 "프레임은 뭐지" 답변 대기) → `P0-6`에 반영
2. `P0-4` 토픽/메시지/서비스/QoS 명세 작성 → `docs/INTERFACES.md`
3. UDP 카메라 송신 장치/포트/PDU 형식과 IMU 타임스탬프 에포크 확정 (`SYSTEM_DESIGN.md` §9)
4. `R0-3` 게이트 승인 → `P0-5`(TF 프레임 설계)로 진행
