
# 41343151

作業二
## 解題說明

本題要求設計一個多項式類別的類別介面與基本運算功能。

### 解題策略

1. 定義 Term 儲存係數與指數，Polynomial 用動態陣列管理多項式項目。  
2. 以 operator+、operator* 完成多項式加法與乘法，用 Eval() 計算多項式值。

## 程式實作

以下為主要程式碼：

```cpp
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

class Polynomial; // forward

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostrcam& os , const Polynomial& p);
private:
    float coef; // 係數
    int   exp;  // 指數
};

class Polynomial {
public:
    // 建構/拷貝/指定/解構
    explicit Polynomial(int cap = 4) : capacity(max(1, cap)), terms(0) {
        termArray = new Term[capacity];
    }
    Polynomial(const Polynomial& rhs) : capacity(rhs.capacity), terms(rhs.terms) {
        termArray = new Term[capacity];
        std::copy(rhs.termArray, rhs.termArray + terms, termArray);
    }
    Polynomial& operator=(const Polynomial& rhs) {
        if (this == &rhs) return *this;
        Term* na = new Term[rhs.capacity];
        std::copy(rhs.termArray, rhs.termArray + rhs.terms, na);
        delete[] termArray;
        termArray = na;
        capacity  = rhs.capacity;
        terms     = rhs.terms;
        return *this;
    }
    ~Polynomial() { delete[] termArray; }

    // 給 >> 使用：把 (exp, coef) 加進陣列（會自動合併同 exp）
    void setTerm(int exp, float coef) { addInPlace(exp, coef); normalize(); }

    // 求值
    float Eval(float x) const {
        float s = 0.0f;
        for (int i = 0; i < terms; ++i)
            s += termArray[i].coef * std::pow(x, termArray[i].exp);
        return s;
    }

    // ---- 友元運算子（非成員）----
    friend istream& operator>>(istream& is, Polynomial& p);
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend Polynomial operator+(const Polynomial& a, const Polynomial& b);
    friend Polynomial operator*(const Polynomial& a, const Polynomial& b);

private:
    // 內部資料
    Term* termArray; // 非零項陣列
    int   capacity;  // 陣列容量
    int   terms;     // 已用項數

    // 小工具
    void ensureCapacity(int need) {
        if (need <= capacity) return;
        int nc = max(need, capacity * 2);
        Term* na = new Term[nc];
        std::copy(termArray, termArray + terms, na);
        delete[] termArray;
        termArray = na;
        capacity  = nc;
    }
    void append(int exp, float coef) { // 未必合併、未必排序
        if (std::fabs(coef) == 0.0f) return;
        ensureCapacity(terms + 1);
        termArray[terms++] = Term{coef, exp};
    }
    void addInPlace(int exp, float coef) { // 對既有 exp 累加
        if (std::fabs(coef) == 0.0f) return;
        for (int i = 0; i < terms; ++i) {
            if (termArray[i].exp == exp) {
                termArray[i].coef += coef;
                if (std::fabs(termArray[i].coef) == 0.0f) {
                    for (int k = i + 1; k < terms; ++k) termArray[k - 1] = termArray[k];
                    --terms;
                }
                return;
            }
        }
        append(exp, coef);
    }
    void normalize() { // 依 exp 大到小排序、合併、去 0
        if (terms <= 1) return;
        sort(termArray, termArray + terms,
            [](const Term& A, const Term& B){ return A.exp > B.exp; });
        int w = 0;
        for (int i = 0; i < terms; ) {
            int   e = termArray[i].exp;
            float c = 0.0f;
            while (i < terms && termArray[i].exp == e) c += termArray[i++].coef;
            if (std::fabs(c) > 0.0f) termArray[w++] = Term{c, e};
        }
        terms = w;
    }
};

// ---- friend operators 的定義 ----

// 輸入格式：先 n，接著 n 組 (exp coef)
// 例如：3  4 3  2 2  0 1  代表 3x^4 + 2x^2 + 1
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;
    Polynomial tmp(n + 4);
    for (int i = 0; i < n; ++i) {
        int e; float c;
        is >> e >> c;
        tmp.addInPlace(e, c);
    }
    tmp.normalize();
    p = tmp; // 使用拷貝指定
    return is;
}

// 以數學友善的格式輸出（省略 1*x、處理 ±）
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) { os << "0"; return os; }
    for (int i = 0; i < p.terms; ++i) {
        float c = p.termArray[i].coef;
        int   e = p.termArray[i].exp;

        if (i > 0) os << (c >= 0 ? " + " : " - ");
        else if (c < 0) os << "-";

        float ac = std::fabs(c);
        bool printCoef = !(ac == 1.0f && e != 0);
        if (printCoef) os << ac;

        if (e > 0) {
            if (printCoef) os << "*";
            os << "x";
            if (e > 1) os << "^" << e;
        }
    }
    return os;
}

// a + b：以 merge 方式合併（兩者皆視為已 normalize）Polyonmial add
Polynomial operator+(const Polynomial& a, const Polynomial& b) {
    Polynomial r(max(a.capacity, b.capacity));
    int i = 0, j = 0;
    while (i < a.terms || j < b.terms) {
        if (j == b.terms || (i < a.terms && a.termArray[i].exp > b.termArray[j].exp))
            r.append(a.termArray[i].exp, a.termArray[i++].coef);
        else if (i == a.terms || b.termArray[j].exp > a.termArray[i].exp)
            r.append(b.termArray[j].exp, b.termArray[j++].coef);
        else {
            float c = a.termArray[i].coef + b.termArray[j].coef;
            if (std::fabs(c) > 0.0f) r.append(a.termArray[i].exp, c);
            ++i; ++j;
        }
    }
    r.normalize();
    return r;
}

// a * b：逐項相乘再合併 Polynomial mult
Polynomial operator*(const Polynomial& a, const Polynomial& b) {
    Polynomial r(a.terms + b.terms + 4);
    for (int i = 0; i < a.terms; ++i)
        for (int j = 0; j < b.terms; ++j)
            r.addInPlace(a.termArray[i].exp + b.termArray[j].exp,
                         a.termArray[i].coef * b.termArray[j].coef);
    r.normalize();
    return r;
}

// ---- 示範 ----
int main() {
    cout << "請輸入多項式 p 的項數與 (exp coef)：\n";
    Polynomial p; cin >> p;
    cout << "請輸入多項式 q 的項數與 (exp coef)：\n";
    Polynomial q; cin >> q;

    cout << "p(x) = " << p << "\n";
    cout << "q(x) = " << q << "\n";

    cout << "p(x) + q(x) = " << (p + q) << "\n";
    cout << "p(x) * q(x) = " << (p * q) << "\n";
    cout << "p(2) = " << p.Eval(2.0f) << "\n";
    return 0;
}

```

## 效能分析

1. 時間複雜度：程式的時間複雜度為 *Eval:O(t) Add:O(n+m) Mult:O((nm)^2) *。
2. 空間複雜度：空間複雜度為 *Eval:O(1) Add:O(n+m) Mult:O((nm)) *。

## 測試與驗證

### 測試案例

<img width="392" height="156" alt="image" src="https://github.com/user-attachments/assets/a444156d-fe6d-4ae9-b4b7-18cc2f015b3b" />

<img width="595" height="156" alt="image" src="https://github.com/user-attachments/assets/c40ff859-b5e0-459e-9803-2ebb83c43db9" />

<img width="432" height="152" alt="image" src="https://github.com/user-attachments/assets/d5562730-7d0b-4ba9-88d1-d3571196fa34" />



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

