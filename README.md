# Mesh Destruction   
2025.03.31 ~ 2025.05.23   

## Static Mesh   

### Voronoi Tesselation   
   <img src="https://github.com/sturdyChair/asset/blob/main/Cube%20Fracturing.PNG" width="600" height="400"/>   
   
   - 정육면체 공간을 비정형적 모양으로 분할하기 위해 구현   
   - 공간 상에 존재하는 입자들로부터 가장 가까운 점들의 집합으로 정육면체 분할   
   - 각 입자 쌍 사이의 선분과 수직이고, 중앙을 관통하는 평면을 이용해 공간을 분할하는 식으로 구현   
   - 입자 개수에 대해 O(n^2)인 알고리즘이나, 공간을 그리드로 분할해 가까운 입자를 우선 탐색하고, 조건에 부합하지 않는 입자를 빠르게 연산 대상에서 제외하는 것으로 O(n)에 가깝게 최적화   

### Mesh Boolean   
   <img src="https://github.com/sturdyChair/asset/blob/main/Intersect.PNG" width="600" height="400"/>   

   - 두 메쉬 간의 교집합, 차집합, 합집합 메쉬를 계산하는 기능을 구현   
   - 보로노이 테셀레이션으로 분할한 입방체 공간과 다양한 모양의 메쉬를 교차 연산, non convex 메쉬 분할할 수 있음
   - 메쉬를 구성하는 면에 대해 다른 면들이 정면에 있는지 후면에 있는지를 기준으로 나눈 BSP 트리를 이용해 기능을 구현/최적화 함

### Keyframe Simulation   
   <img src="https://github.com/sturdyChair/asset/blob/main/dynamic_destroy-ezgif.com-video-to-gif-converter.gif" width="600" height="400"/>   
   <img src="https://github.com/sturdyChair/asset/blob/main/Simul_destroy-ezgif.com-video-to-gif-converter.gif" width="600" height="400"/>   

   - 조각난 메쉬를 nvidia physx 엔진에 연동, 시뮬레이트 결과를 키프레임 애니메이션으로 저장   
   - 이후 물리 엔진 없이 공격 방향에 따라 파괴되는 애니메이션을 재생   

### Destruction System   
   <img src="https://github.com/sturdyChair/asset/blob/main/516/BreakBetter%20-%20Trim_1.gif" width="600" height="400"/>   
   <img src="https://github.com/sturdyChair/asset/blob/main/516/BreakBetter%20-%20Trim_2.gif" width="600" height="400"/>    

   - 최초 구현은 여러 물리 액터를 고정 Constraint로 묶어주는 것이었으나, 출렁임이 심하고 제어가 어려웠음
   - 단일 물리 액터에 모든 조각을 할당한 후, 조각이 피해를 받음에 따라 인접 조각간의 연결이 파괴되는 방식으로 재구현
   - '연결'이 끊어지면 dirty flag를 올리고, 프래임 끝에 각 조각이 정점, 각 연결이 간선인 그래프를 탐색해 고립된 부분을 새로운 물리 액터로 재할당함
   - 물리 엔진 밖에서의 상호작용 구현이 용이하고 출렁임을 제거할 수 있었음   

## Skinned Mesh   

### Dynamic Slice   
  <img src="https://github.com/sturdyChair/asset/blob/main/cut_0.gif" width="600" height="400"/>   

  - 메쉬의 정점이 평면의 법선 방향에 있는지, 아닌지를 기준으로 나누어 새로운 정점 버퍼를 생성   
  - DX 11의 Compute Shader를 이용해 GPU 가속   
  - 절단 이후 래그돌 적용, 절단된 조각 또한 다시 절단 가능   

  <img src="https://github.com/sturdyChair/asset/blob/main/430/static_slice.gif" width="600" height="400"/>   

   - 절단 타이밍에 스킨드 메쉬의 상태를 스태틱 메쉬로 변환 후 절단   
   - 절단 후 새로 생긴 절단면 정점을 기준으로 파티클 생성   

### Voronoi + Mesh Swap   
   <img src="https://github.com/sturdyChair/asset/blob/main/516/SkinnedFracture%20-%20Trim.gif" width="600" height="400"/>   

   - 스키닝 이전의 메쉬를 사전 분할    
   - 피격시 사전 분할된 조각을 가장 가까운 Bone 위치에 스폰   

### Mesh Clip
   <img src="https://github.com/sturdyChair/asset/blob/main/516/wound%20-%20Trim.gif" width="600" height="400"/>   

   - 메쉬의 일부분을 타원체 모양으로 클리핑
   - 스키닝 이전의 정점을 타원체의 로컬 기저로 변환, 원점으로부터 거리가 1 이하면 해당 정점을 그리지 않도록 함
   - 캐릭터 메쉬의 뒷면을 스텐실에 기록, 이후 타원체의 뒷면을 스텐실에 기록된 부분에만 그려 절단면을 표현
   
