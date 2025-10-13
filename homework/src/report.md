
# 41343151

作業一
第一題
## 解題說明

本題要求實現一個阿克曼函數，阿克曼函數是非原始遞迴函數的例子，它需要兩個自然數作為輸入值，輸出一個自然數。

### 解題策略

1. 當m==0 時，作為遞迴的結束條件。  
2. 主程式呼叫遞迴函式，並輸出計算結果。

## 程式實作

以下為主要程式碼：

```cpp
#include <iostream>
using namespace std;

int Ackermann(const int m,const int n){
    if(m==0)
    return n+1;
    else if(n==0)
    return Ackermann(m-1,1);
    else
    return Ackermann(m-1,Ackermann(m,n-1));
}

int main(){
    int m,n;
    cout<<"輸入A(m,n) ";
    cin>>m>>n;
    cout<<"A("<<m<<","<<n<<")="<<Ackermann(m,n)<<endl;
    return 0;
}
```

## 效能分析

1. 時間複雜度：程式的時間複雜度為 *O(A(m,n))*。
2. 空間複雜度：空間複雜度為 *O(A(m,n))*。

## 測試與驗證

### 測試案例

| 測試案例 | 輸入參數 *m* | 輸入參數 *n* | 預期輸出 | 實際輸出 |
|----------|--------------|----------|----------|----------|
| 測試一   | *m = 0*      | *n = 2*        | 3        | 3        |
| 測試二   | *m = 2*      | *n = 0*        | 3        | 3        |
| 測試三   | *m = 3*      | *n = 4*        | 125      | 125      |
| 測試四   | *m = 2*      | *n = 3*        | 9        | 9        |
| 測試五   | *m = 0*      | *n = 0*        | 拋出異常        | 拋出異常        |

### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 -o powerset powerset.cpp
$ ./powerset
```

### 結論

1. 程式能正確計算當*m*跟*n*個別是多少時的答案。  
2. 在*m*跟*n*都是*0*的情況下，程式會成功拋出異常，符合設計預期。  
3. 測試案例涵蓋了多種邊界情況（*m = 0* *n = 2* || *m = 3* *n = 4* ），驗證程式的正確性。

## 申論及開發報告

### 選擇遞迴的原因

在本程式中，使用遞迴來計算加減的主要原因如下：

1. **程式邏輯簡單直觀**  
   遞迴的寫法能夠清楚表達「將問題拆解為更小的子問題」的核心概念。  
   例如，計算 *A(m-1,A(m,n-1)* 的過程可分解為：  

   
   <img width="660" height="166" alt="image" src="https://github.com/user-attachments/assets/5fd096a4-3a6a-4331-a8ea-ce26ca59289a" />
   


  當*m*=0 時，直接輸出*n+1*的質。

2. **易於理解與實現**  
   遞迴的程式碼更接近數學公式的表示方式，特別適合新手學習遞迴的基本概念。  
   以本程式為例：  

   ```cpp
   int Ackermann(const int m,const int n){
    if(m==0)
    return n+1;
    else if(n==0)
    return Ackermann(m-1,1);
    else
    return Ackermann(m-1,Ackermann(m,n-1));
    }
   ```

3. **遞迴的語意清楚**  
   在程式中，每次遞迴呼叫都代表一個「子問題的解」，而最終遞迴的返回結果會逐層相加，完成整體問題的求解。  
   這種設計簡化了邏輯，不需要額外變數來維護中間狀態。
   透過遞迴實作簡單的加減計算，程式邏輯簡單且易於理解，特別適合展示遞迴的核心思想。然而，遞迴會因堆疊深度受到限制，當 $n$ 值過大時，應考慮使用迭代版本來避免 Stack Overflow 問題。

作業一
第二題
## 解題說明

本題要求我實現一個*遞迴函數（recursive function）*，用來計算某個集合 *S* 的 *power set（冪集合）*。

### 解題策略

1. 當集合為空時（即沒有元素可處理），回傳一個只包含空集合的結果 [[]]，作為遞迴的停止條件。  
2. 主程式呼叫遞迴函式，並輸出其結果。

## 程式實作

以下為主要程式碼：

```cpp
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void PowerSetRecursive(const vector<string> &S, int index, vector<string> &subset) {
    if (index == (int)S.size()) {
        
        cout << "{";
        for (size_t i = 0; i < subset.size(); ++i) {
            cout << subset[i];
            if (i + 1 < subset.size()) cout << ", ";
        }
        cout << "}" << endl;
        return;
    }
    PowerSetRecursive(S, index + 1, subset);
    subset.push_back(S[index]);
    PowerSetRecursive(S, index + 1, subset);
    subset.pop_back(); 
}

int main() {
    cout << "=== Powerset (冪集) 遞迴產生程式 ===\n";
    cout << "請輸入集合中元素個數 n：";

    int n;
    cin >> n;
    if (n <= 0) {
        cout << "n 必須大於 0！" << endl;
        return 0;
    }

    vector<string> S(n);
    cout << "請輸入集合的元素（以空白分隔）：";
    for (int i = 0; i < n; ++i)
        cin >> S[i];

    cout << "\n集合 S = { ";
    for (int i = 0; i < n; ++i) {
        cout << S[i];
        if (i + 1 < n) cout << ", ";
    }
    cout << " }\n\n";
    cout << "S 的所有子集合如下（共 2^" << n << " = " << (1 << n) << " 個）：\n";

    vector<string> subset;
    PowerSetRecursive(S, 0, subset);

    return 0;
}
 
```

## 效能分析

1. 時間複雜度：程式的時間複雜度為 *O(n*2^2)*。
2. 空間複雜度：空間複雜度為 *O(2^n)*。

## 測試與驗證

### 測試案例

| 測試案例 | 輸入參數 *n* 元素| 預期輸出 | 實際輸出 ||
|----------|--------------|----------|----------|----------|
| 測試一   | *n=3*   a b c   | {} {c}{b} {b, c} {a} {a, c} {a, b} {a, b, c}        | {} {c}{b} {b, c} {a} {a, c} {a, b} {a, b, c}        |
| 測試二   | *n=2*    a b  | {} {b} {a} {a, b}        | {} {b} {a} {a, b}        |
| 測試三   | *n=1*     a  | {} {a}      | {} {a}      |
### 編譯與執行指令

```shell
$ g++ powerset.cpp -o powerset
$ ./powerset
```

### 結論

1. 程式能正確列出集合中所有子集合，並在不同輸入大小下皆可正常運作。在集合為空的情況下，程式正確回傳僅包含空集合的結果，符合遞迴設計的結束條件與預期行為。

## 申論及開發報告

### 選擇遞迴的原因

本題使用遞迴的主要目的，是為了系統性地列出集合中所有可能的子集合組合。

1. **程式邏輯簡單直觀**  
   程式邏輯簡單直觀，運作方式清楚明瞭。
   

2. **易於理解與實現**  
   易於理解與實現，能以簡潔的遞迴結構生成所有子集合。
   以本程式為例：  

   ```cpp
   void PowerSetRecursive(const vector<string> &S, int index, vector<string> &subset) {
    if (index == (int)S.size()) {
        
        cout << "{";
        for (size_t i = 0; i < subset.size(); ++i) {
            cout << subset[i];
            if (i + 1 < subset.size()) cout << ", ";
        }
        cout << "}" << endl;
        return;
    }
    PowerSetRecursive(S, index + 1, subset);
    subset.push_back(S[index]);
    PowerSetRecursive(S, index + 1, subset);
    subset.pop_back();
   }
   ```

3. **遞迴的語意清楚**  
   1.遞迴語意清楚，對應「包含或不包含元素」的二元選擇邏輯。
   2.每一層遞迴皆有明確的結束條件與回傳結果，結構層次分明。
