# 빈번하게 변경되는 좌표 스트림에서 일정 반경에 속하는 좌표 구하기
## Summary
이 과제는 고급자료구조론 기말 고사 텀 프로젝트로, 알고리즘 문제를 주고 성능 경쟁을 했습니다. \
문제에 대한 자세한 내용은 [문제.pdf](./Advan%2013%20-%20Final%20Challenge-b.pdf)를 참고해주세요.

직접 구현해도 되고, 외부 라이브러리를 써도 되도 되는 과제였습니다. \
저는 전체에서 **2위**를 했으며 **직접 구현한 사람들 중에서는 1위**를 했습니다.

## 문제 해결 방법
- 문제 해결을 위해 `1-Dimension Range tree`를 적용하였습니다. 이는 binary search tree(bst)와 1D range search로 구현됩니다. 
- 힙 할당을 줄이고 캐시 히트를 늘리기 위해, 포인터가 아닌 int 배열과 인덱스로 bst 자료구조와 1d range search 알고리즘을 고안하였습니다.
- 높은 성능을 위해 C로 프로그램을 작성하였습니다. 
- C로 작성한 모듈을 object 파일로 만든 뒤, Python으로 짠 테스트 스크립트에서 ctypes로 불러 왔습니다. 그리고 Property Based Testing(PBT) 라이브러리인 Hypothesis를 활용하여 C 모듈을 테스트하였습니다.

이 레포를 통해 다음을 알 수 있습니다.
- 컴퓨터 내부 구조를 알며, 성능 최적화를 위한 자료구조와 알고리즘을 고안할 수 있습니다
- C로 매우 복잡한 로직을 작성할 수 있습니다
- PBT를 적용하여 무결한 프로그램을 작성할 수 있습니다

코드에 대한 개괄은 [보고서](./final_report.pdf)에서 확인할 수 있습니다.

## Result
| ID               | DS                         | g1-1     | g1-2     | g1-3      | g2-1     | g2-2     | g2-3      | u-1      | u-2      | u-3       | 비고  |
| ---------------- | -------------------------- | -------- | -------- | --------- | -------- | -------- | --------- | -------- | -------- | --------- | --- |
| dsaf1595         | Red-Black tree+Linked List | 4.06     | 0.00     | 0.00      | 2.85     | 12.17    | 62.25     | 0.00     | 15.63    | 99.70     |     |
| **gndprop**      | **R-Tree**                 | **1.61** | **5.83** | **35.08** | **1.03** | **3.32** | **10.90** | **1.54** | **4.82** | **17.30** | 1<sup>st</sup> |
| harisw           | Red-Black tree             | 5.01     | 0.00     | 0.00      | 4.98     | 18.26    | 76.63     | 0.00     | 19.09    | 74.33     |     |
| kkuang           | Hash Table+Linked List     | 4.98     | 18.90    | 130.56    | 2.87     | 11.74    | 47.80     | 3.44     | 13.77    | 55.33     |     |
| ninebell         | K-D tree                   | 13.14    | 54.88    | 395.17    | 5.77     | 26.11    | 115.87    | 9.39     | 39.83    | 166.73    |     |
| **(mine)** | **Range tree**             | **2.28** | **9.68** | **73.18** | **1.42** | **5.97** | **25.33** | **2.15** | **8.97** | **35.64** | 2<sup>nd</sup> |
| tjwldn.      | STL-map, multimap          | 302.74   | 1133.82  | 7833.72   | 175.23   | 716.57   | 2930.07   | 206.46   | 842.01   | 3419.38   |     |

표의 수는 초 단위 수행시간입니다. `0.00`은 segmentation fault를 의미합니다. \
데이터는 다음과 같습니다.
- g1: 중심점이 하나인 gauss 분포
- g2: 중심점이 두개인 gauss 분포
- u: uniform 분포

자세한 내용은 [xlsx파일](./ADS%20test/AvDS2020%20-%20결과취합(공개).xlsx)에서 확인할 수 있습니다.

## Run
### Install libs and Set project
`cd proj` \
`git clone https://github.com/emil-e/rapidcheck.git` \
`cmake CMakeLists.txt` \
`make` 

### After installation
Update src file list in ./CMakeLists.txt \
`make  # build` \
`./test   # unit test` \
`./ptest  # prop test` \
`./bench  # running time check ` \
`./main   # actual solution` 
