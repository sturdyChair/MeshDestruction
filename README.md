# Mesh Destruction   
2025.03.31 ~ 2025.05.23   

## Static Mesh   

### Voronoi Tesselation   
   <img src="https://github.com/sturdyChair/asset/blob/main/Cube%20Fracturing.PNG" width="600" height="400"/>   
   - 정육면체 공간을 비정형적 모양으로 분할하기 위해 구현   
   - 공간 상에 존재하는 입자들로부터 가장 가까운 점들의 집합으로 정육면체 분할
   - 각 입자 쌍 사이의 선분과 수직이고, 중앙을 관통하는 평면을 이용해 공간을 분할하는 식으로 구현
   - 입자 개수에 대해 O(n^2)인 알고리즘이나, 공간을 그리드로 분할해 가까운 입자를 우선 탐색하고, 조건에 부합하지 않는 입자를 빠르게 연산 대상에서 제외하는 것으로 O(n)에 가깝게 최적화

### Keyframe Simulation   

### Destruction System   
   <img src="https://github.com/sturdyChair/asset/blob/main/516/BreakBetter%20-%20Trim_1.gif" width="600" height="400"/>   
   <img src="https://github.com/sturdyChair/asset/blob/main/516/BreakBetter%20-%20Trim_2.gif" width="600" height="400"/>    

   - 최초 구현은 여러 물리 액터를 고정 Constraint로 묶어주는 것이었으나, 출렁임이 심하고 제어가 어려웠음
   - 단일 물리 액터에 모든 조각을 할당한 후, 조각이 피해를 받음에 따라 인접 조각간의 연결이 파괴되는 방식으로 재구현
   - '연결'이 끊어지면 dirty flag를 올리고, 프래임 끝에 각 조각이 정점, 각 연결이 간선인 그래프를 탐색해 고립된 부분을 새로운 물리 액터로 재할당함
   - 물리 엔진 밖에서의 상호작용 구현이 용이하고 출렁임을 제거할 수 있었음   

## Skinned Mesh   

### Dynamic Slice   

### Voronoi + Mesh Swap   

### Mesh Clip
