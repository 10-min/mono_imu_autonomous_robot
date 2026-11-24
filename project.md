11. 프로젝트 개발 순서 (Top-down Roadmap)
이 순서로 프로젝트를 진행하면 시스템 설계부터 최종 자율주행까지 자연스럽게 연결돼.
Phase 0 — System Design
현재 단계
목표: 로봇의 전체 시스템 구조 정의
- 하드웨어 구성 결정
- 시스템 아키텍처 설계
- Node 구성 및 데이터 흐름
- Topic / Message 정의
- TF Frame 설계
- UART Protocol 설계
- GitHub repository 구성
완료 기준: 시스템 설계 문서와 인터페이스 명세서 작성 완료

Phase 1 — Embedded Robot Base
목표: STM32를 이용한 안정적인 주행 플랫폼
- Motor Driver 제어
- Encoder 측정
- PWM 제어
- Wheel PID
- Odometry
- UART 통신
- Failsafe / Watchdog
완료 기준: 직진, 후진, 회전, 속도 제어가 가능하고 명령 Timeout 시 자동 정지

Phase 2 — ROS 2 Integration
목표: Pi와 STM32를 ROS 2 기반으로 통합
- ROS 2 Workspace
- stm32_bridge_node
- UART Protocol Parser
- /cmd_vel
- /odom
- Robot URDF
- TF
- RViz2
완료 기준: ROS 2 명령으로 로봇을 제어하고 Odometry를 시각화

Phase 3 — Camera & IMU
목표: 센서 데이터를 안정적으로 확보
- Camera Driver
- Image Pipeline
- IMU Driver
- Calibration
- Timestamp
- Sensor Logging
- Camera-IMU Synchronization
완료 기준: 동기화된 Camera / IMU 데이터 기록 및 반복 실험 가능

Phase 4 — Visual Odometry / VIO
목표: 카메라와 IMU를 이용한 로봇 이동 추정
- Feature Detection
- Feature Tracking
- Optical Flow
- Monocular VO
- IMU Preintegration
- Sensor Fusion
- VIO
- Trajectory Evaluation
완료 기준: 로봇이 이동할 때 추정 trajectory가 실제 이동과 비교해 합리적으로 유지됨

Phase 5 — Visual SLAM & Localization
목표: 지도 생성 및 지도 내 위치 추정
- Visual SLAM
- Keyframe
- Map Point
- Loop Closure
- Drift 분석
- Map 저장
- Relocalization
완료 기준: 반복 이동하는 환경에서 지도 생성 및 재방문 위치 인식 검증

Phase 6 — Autonomous Navigation
목표: 지정된 Goal까지 이동
- Goal Interface
- Path Planning
- Path Following
- Obstacle Detection
- Velocity Control
- Recovery Behavior
- Safety Supervisor
완료 기준: 제한된 실험 공간에서 Goal 지정 → 경로 계획 → 주행 → 도착

Phase 7 — Reliability & Portfolio
목표: 실험 가능한 완성형 프로젝트로 정리
- System Integration Test
- Failure Handling
- CPU / Memory profiling
- SLAM 성능 평가
- 주행 성공률
- 반복 주행 실험
- ROS Bag 분석
- GitHub 문서 및 영상
완료 기준: 재현 가능한 실험 결과와 기술적 근거가 있는 포트폴리오 완성