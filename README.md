※ Luminarch는 Unreal Engine 5에서 조명·퍼즐·상호작용 시스템을 실험하기 위해 제작한 
1인칭 시점 개인 프로젝트입니다.  
C++과 Blueprint를 혼합해 조명 자동제어, 퍼즐 트리거, 문·키패드 상호작용 등을 구현했으며 실시간 빛의 변화와 플레이어 경험을 연결하는 구조를 학습하기 위한 목적의 프로젝트입니다.

## 플레이 영상 링크 (유튜브)
[https://youtu.be/dvbiTv6PgNM](https://youtu.be/otNYKGshHyY?si=2fFQtN9CHd-8x5Rx)


# Luminarch (루미나크)

> “어둠 속, 빛만이 길을 말해준다.”  
> *Luminarch — The Door of Light*

---

##  프로젝트 개요

| 항목 | 내용 |
|------|------|
| **프로젝트명** | Luminarch (루미나크) — 빛(Lumen) + Arch(출구/문) |
| **장르** | 1인칭 퍼즐 어드벤처 / 탈출형 스릴러 |
| **플랫폼** | PC |
| **개발 기간** | 2025.09 ~ 2025.11 (약 2개월) |
| **개발 인원 / 역할** | 1인 개발 (레벨 디자인, 환경 아트, 퍼즐 시스템 블루프린트 및 C++ 구현) |
| **툴 / 엔진** | Unreal Engine 5, SketchUp, Photoshop, Illustrator |

---

##  핵심 컨셉

**한 줄 소개**  
끝없는 어둠 속, 희미한 빛을 따라 탈출을 시도하라.

**기획 의도**  
- 백룸처럼 익명적이고 폐쇄적인 공간에서 오는 긴장감 전달  
- “빛 = 단서”라는 메커니즘으로 탐험과 퍼즐 해결을 연결  
- 공포·심리적 압박감을 통해 공간 자체가 퍼즐이 되는 경험 제공  

**타겟 유저층**  
퍼즐 / 스릴러 / 공포 장르를 즐기는 게이머 (15세 이상)

**차별성**  
어둠 속에서 빛을 찾아 해석하는 심리적 퍼즐 플레이

---

##  게임 플레이 & 조작 방식

### 기본 조작

| 입력 | 기능 |
|------|------|
| **W, A, S, D** | 캐릭터 이동 |
| **마우스 이동** | 시야 / 카메라 회전 |
| **마우스 우클릭** | 줌 In Out | 
| **E** | 상호작용 (문, 스위치, 손전등 등) |
| **Tab** | 수집한 단서 / 암호 확인 |
| **F** | 손전등 ON/OFF (습득 후 활성화) |
| **Esc** | 일시정지 및 메뉴 호출 |

---

### 플레이 흐름
폐쇄된 공간 탐험 → 손전등 습득 → 어둠 속 단서 탐색 → 퍼즐 해결 → 출구(빛의 아치) 탈출


**핵심 시스템**
- 손전등 시스템: 초반엔 사용 불가, 습득 후 F키로 조작  
- UI 시스템: Widget Screen 기반, 단서·상호작용 표시  
- 빛 단서 퍼즐: 어두운 구간에 숨겨진 암호를 손전등으로 찾아 해석  
- 환경 퍼즐: 조명이 단서 역할 
- 최종 구간: 문(Arch)을 열면 강렬한 백색광 등장 → 탈출

---

##  콘텐츠 구성

- **캐릭터**: 이름 없는 실험 대상자 (1인칭 시점)
- **퍼즐 오브젝트**: 조명 스위치, 암호, 손전등, 단서
- **맵 구조**: 폐쇄 복도, 어두운 구간, 출구 ‘빛의 아치’
- **목표**: 빛을 활용해 숨겨진 단서를 찾아내고 어둠의 구조를 해석해 탈출

---

##  기술적 구현 (Unreal C++)

##  C++ 구현

### 1. 플레이어 캐릭터 & 입력 시스템
- 클래스: `AMyCharacter`, `ALuminarchGameMode`  
- **기능:** 이동, 회전, 줌, 손전등, 상호작용 입력 처리  
- **사용:** `Enhanced Input` 시스템  
- `GameMode`에서 `DefaultPawnClass`를 `AMyCharacter`로 지정  

### 2. 카메라 & 줌(Zoom) 시스템
- `SpringArm` + `CameraComponent` 구성  
- FOV를 `FInterpTo`로 보간 → 부드러운 줌 인/아웃  
- 카메라 전방으로 `LineTraceSingle()` 실행 후  
  `DepthOfFieldFocalDistance`를 갱신하여 **자동 초점(Auto Focus)** 구현  

### 3. 손전등 시스템
- `USpotLightComponent`를 카메라에 부착  
- ON/OFF 토글 및 사용 가능 여부(`bCanUseFlashLight`) 제어  
- `UnlockFlashLight()`로 퍼즐 해금 시 손전등 기능 활성화  

### 4. 이동 & 애니메이션 연동
- `UCharacterAnimInstance`에서 이동 속도(`Velocity.Size()`) 및  
  손전등 사용 상태(`bCanUseFlashLight`)를 애니메이션 파라미터로 전달  
- 이동 여부(`bIsMoving`)를 기반으로 걷기/정지 애니메이션 전환  

### 5. 발소리 시스템
- 이동 중 일정 주기로 `FootstepSound` 재생  
- `UGameplayStatics::PlaySoundAtLocation()` 사용  
- `FootstepInterval`로 발소리 간격 조절  

### 6. 게임 상태 제어
- `PauseKey`, `PickKey`, `OpenKey` 입력 감지  
- C++에서 이벤트 트리거만 담당, 실제 동작은 블루프린트에서 확장  

---

###  블루프린트 구현

### 1. 상호작용 시스템
- 인터페이스: `IInteractable`  
- 문, 스위치, 단서 등 오브젝트에 공통 적용  
- C++ 입력 → 인터페이스 호출로 블루프린트 이벤트 실행  

### 2. 단서 & 퍼즐 시스템
- 손전등의 빛에 반응하는 힌트 퍼즐 구현  
- `OnComponentBeginOverlap` 또는 `OnHit` 이벤트 기반  
- 특정 밝기 조건 충족 시 머티리얼 파라미터 변경  
- `PickKey` 입력 시 `UnlockFlashLight()`와 연동  

### 3. UI & 위젯 시스템
- `UMG`로 HUD, 단서 표시, 인터랙션 텍스트 구성  
- C++에서 이벤트 발생 시 블루프린트 위젯으로 데이터 전달  
- 위젯 내부에서 상태 갱신 및 애니메이션 연출 처리 

---

##  아트 & 비주얼

**그래픽 스타일**  
현실적 폐허 + 어둠 대비 강한 빛 연출

**조명 연출**  
- 희미한 천장 조명, 깜빡이는 네온, 청록빛 LED  
- 출구(Arch)는 강렬한 순백광으로 표현  

**UI 디자인**  
인터랙션 메시지 위젯, 일시정지, 가이드 위젯  

---

##  개발 일정 (2025.09.01 ~ 2025.11.14)

| 주차 | 기간 | 주요 작업 | 결과 |
|------|------|-----------|------|
| 1주 | 09.01~09.07 | 콘셉트 아트, 공간 레퍼런스 수집 | 폐쇄적 공간 확립 |
| 2~3주 | 09.08~09.21 | 1인칭 캐릭터 / 카메라 컨트롤 | 이동 및 시야 완성 |
| 4~5주 | 09.22~10.05 | 복도 구조 모델링, 조명 세팅 | 탐험 맵 구성 |
| 6주 | 10.06~10.12 | 손전등 / 상호작용 시스템 구현 | 조작 가능 상태 |
| 7주 | 10.13~10.19 | 빛 단서 퍼즐 로직 완성 | 첫 퍼즐 작동 |
| 8주 | 10.20~10.26 | 구조 테스트 | 동선 체크 |
| 9주 | 10.27~11.02 | 엔딩 문 / 탈출 구간 구현 | 빛의 아치 연출 |
| 10주 | 11.03~11.14 | QA 및 영상 제작 | 최종 빌드 완성 |

---

##  결과물

- 실행 영상: YouTub / Drive 업로드 예정  
- 주요 기여 파트:  
  - 레벨 디자인  
  - 손전등 / 빛 단서 퍼즐 로직  
  - 상호작용 시스템
  - UI / 위젯 시스템  

---

##  회고 & 개선 방향

**배운 점**
- 언리얼 C++ 기반 상호작용 설계  
- 빛·그림자 기반 퍼즐 구조화  
- 긴장감을 주는 레벨 구성 방식  

**아쉬운 점**
- 사운드 및 공포 연출 부족  

**개선 방향**
- 사운드 큐 기반 심리적 공포 강화  
- 멀티 엔딩, 협동 모드 확장  
- “무한 공간 + 빛 퍼즐” 시스템 모듈화  

---

##  참고 이미지

- 시작 화면
<img width="1920" height="1080" alt="플레이화면" src="https://github.com/user-attachments/assets/d264ee4b-e66a-4ddb-b31e-e2ed6ec799f3" />

- 조작 방법
<img width="8001" height="4501" alt="조작방법" src="https://github.com/user-attachments/assets/69b48c4f-09f3-418a-8688-5fe7c6eef115" />

<img width="1920" height="1080" alt="플레이화면1" src="https://github.com/user-attachments/assets/312bdeb3-2529-46b2-ab5b-5b51dd6aa9a5" />
<img width="1920" height="1080" alt="플레이화면2" src="https://github.com/user-attachments/assets/65741d1a-8654-4626-b131-cf81a997450b" />
<img width="1920" height="1080" alt="플레이화면3" src="https://github.com/user-attachments/assets/3d77a463-3332-4b7e-a1d2-9c70d4d20e50" />
<img width="1920" height="1080" alt="플레이화면4" src="https://github.com/user-attachments/assets/c80319aa-4b31-4264-8041-3cfbec844504" />

- 첫번째 퍼즐 공간
<img width="1905" height="1031" alt="첫번째퍼즐공간" src="https://github.com/user-attachments/assets/ce058a59-82cf-4cc9-ae97-ebf9cc44e773" />
<img width="1056" height="421" alt="위젯스크린변경" src="https://github.com/user-attachments/assets/b49c3e20-6a95-4f50-afd8-8073b43b6c95" />
<img width="1920" height="1080" alt="손전등집기" src="https://github.com/user-attachments/assets/2d15b667-da8c-4a20-9e5a-64a3fc8c3538" />

<img width="1920" height="1080" alt="손전등예시" src="https://github.com/user-attachments/assets/f10d9669-1c19-4166-8a56-70bea6eefd22" />
- 손전등 습득 → 사용 가능

<img width="1920" height="1080" alt="환경탐색퍼즐" src="https://github.com/user-attachments/assets/6eb73507-1604-4e7d-bc5d-122d09e7bea7" />
<img width="1549" height="549" alt="힌트벽" src="https://github.com/user-attachments/assets/b4953675-11d9-4a36-85b9-e4536c7142f4" />
<img width="1890" height="1003" alt="암호풀기" src="https://github.com/user-attachments/assets/ed137cdb-93cc-4e7b-b2f1-f54758e31bf2" />
- 어두운 구간 단서 탐색

<img width="1920" height="1080" alt="출구복도" src="https://github.com/user-attachments/assets/973cb6d0-651d-4335-bc6c-e99035280bf6" />
<img width="2547" height="1488" alt="출구직전" src="https://github.com/user-attachments/assets/fbe8396e-0997-48ea-9a3b-723687b7545b" />
<img width="1920" height="1080" alt="출구" src="https://github.com/user-attachments/assets/d318357c-e7e2-437e-bb28-8277686e3bb7" />
- 최종 문 (탈출구 빛)

<img width="1920" height="1080" alt="종료" src="https://github.com/user-attachments/assets/3d28a03d-136d-4647-bc09-0c2fdce52a94" />
- 밝은 공간 진입 → 게임 종료  

---

**Luminarch** is a solo Unreal Engine 5 project exploring psychological tension and spatial storytelling through light.  
Developed by **Chae Sungwoo (2025)**.

