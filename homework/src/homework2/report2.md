
# 41343151

作業二
## 解題說明

本題要求設計一個多項式類別的類別介面與基本運算功能。

### 解題策略

1. 定義 Term 儲存係數與指數，Polynomial 用動態陣列管理多項式項目。  
2. 以 operator+、operator* 完成多項式加法與乘法，用 Eval() 計算多項式值。
3. *Add()*是把兩個多項式 p(x) 和 q(x) 相加，得到新多項式 r(x) = p(x) + q(x)，邏輯上是「指數相同的項合併、指數不同的直接保留」。
4. *Mult()*將兩個多項式 p(x) 和 q(x) 相乘。
5. *Eval()*是將變數 x 代入多項式 p(x) 中，計算其數值結果。

## 程式實作

以下為主要程式碼：

```cpp
/*
class Polynomial{
public:
    Polynomial();
    // Construct the polynomial p(x)=0
    Polynomial Add(Polynomial poly);
    // Return the sum of the polynomials this and poly
    Polynomial Mult(Polynomial poly);
    // Return the product of the polynomials this and poly
    float Eval(float f);
    // Evaluate the polynomial this at f and return the result
};
*/

#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

class Polynomial; // 前置宣告，讓 Term 可宣告 friend

// ---- Term：一個非零項 (coef * x^exp) ----
class Term {
    friend class Polynomial;           // 讓 Polynomial 可直接訪問 coef/exp
private:
    float coef;                        // 係數
    int   exp;                         // 指數（建議維持為非負整數）
    Term(float c = 0.0f, int e = 0) : coef(c), exp(e) {}
public:
    // 提供唯讀 getters（友善排序/比較/輸出流程）
    float getCoef() const { return coef; }
    int   getExp()  const { return exp;  }
};

// ---- Polynomial：以動態陣列存非零項（不保證稀疏度，但只存非零）----
class Polynomial {
public:
    // 建構子：capacity 至少為 1；terms=0 表示目前沒有項
    explicit Polynomial(int cap = 4) : capacity(std::max(1, cap)), terms(0) {
        termArray = new Term[capacity];
    }

    // 拷貝建構：深拷貝 termArray
    Polynomial(const Polynomial& rhs) : capacity(rhs.capacity), terms(rhs.terms) {
        termArray = new Term[capacity];
        std::copy(rhs.termArray, rhs.termArray + terms, termArray);
    }

    // 指定運算子：深拷貝並避免自我指定
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

    // 解構子：釋放動態陣列
    ~Polynomial() { delete[] termArray; }

    // 設定/覆寫或累加某一個 exp 的係數（自動合併後 normalize）
    void setTerm(int exp, float coef) { addInPlace(exp, coef); normalize(); }

    // 求值：直接代入 Horner 亦可；此處用 Σ coef * x^exp
    float Eval(float x) const {
        float s = 0.0f;
        for (int i = 0; i < terms; ++i)
            s += termArray[i].coef * std::pow(x, termArray[i].exp);
        return s;
    }

    // 作業要求的公開介面（內部轉呼叫 operator）
    Polynomial Add(const Polynomial& p)  const { return *this + p; }
    Polynomial Mult(const Polynomial& p) const { return *this * p; }

    // ---- 友元運算子（讓非成員函式能存取內部結構）----
    friend istream& operator>>(istream& is, Polynomial& p);
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend Polynomial operator+(const Polynomial& a, const Polynomial& b);
    friend Polynomial operator*(const Polynomial& a, const Polynomial& b);

private:
    // 內部資料成員
    Term* termArray;   // 存放非零項
    int   capacity;    // 陣列容量
    int   terms;       // 目前有效項數（termArray[0..terms-1]）

    // ---- 內部小工具 ----

    // 確保容量足夠（不足即倍增）
    void ensureCapacity(int need) {
        if (need <= capacity) return;
        int nc = std::max(need, capacity * 2);
        Term* na = new Term[nc];
        std::copy(termArray, termArray + terms, na);
        delete[] termArray;
        termArray = na;
        capacity  = nc;
    }

    // 直接在尾端附加一項（不處理同 exp 合併、不排序）
    void append(int exp, float coef) {
        if (std::fabs(coef) == 0.0f) return; // 0 項不存
        ensureCapacity(terms + 1);
        termArray[terms] = Term(coef, exp);
        ++terms;
    }

    // 就地累加（若 exp 已存在則累加；為 0 則刪除；否則 append）
    void addInPlace(int exp, float coef) {
        if (std::fabs(coef) == 0.0f) return;
        for (int i = 0; i < terms; ++i) {
            if (termArray[i].exp == exp) {
                termArray[i].coef += coef;
                if (std::fabs(termArray[i].coef) == 0.0f) {
                    // 係數變 0：把尾巴往前搬動，並縮短 terms
                    for (int k = i + 1; k < terms; ++k) termArray[k - 1] = termArray[k];
                    --terms;
                }
                return;
            }
        }
        append(exp, coef);
    }

    // 正規化：依 exp 由大到小排序 + 合併同 exp + 移除 0 項
    void normalize() {
        if (terms <= 1) return;

        // 1) sort by exp desc
        std::sort(termArray, termArray + terms,
                  [](const Term& A, const Term& B){ return A.getExp() > B.getExp(); });

        // 2) linear scan 合併同 exp 並去零
        int w = 0; // write index
        for (int i = 0; i < terms; ) {
            int   e = termArray[i].getExp();
            float c = 0.0f;
            while (i < terms && termArray[i].getExp() == e) {
                c += termArray[i].getCoef();
                ++i;
            }
            if (std::fabs(c) > 0.0f) {
                termArray[w] = Term(c, e);
                ++w;
            }
        }
        terms = w; // 收斂有效長度
    }
};

// ---- I/O operators ----

// 輸入格式：先輸入 n，再輸入 n 組 "(exp coef)"
// 例：3  4 3  2 2  0 1  代表 p(x)=3x^4 + 2x^2 + 1
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;        // 讀不到 n 直接回傳（保留原 p）
    Polynomial tmp(n + 4);            // 預留一點餘量（減少擴容次數）
    for (int i = 0; i < n; ++i) {
        int e; float c;
        is >> e >> c;
        tmp.addInPlace(e, c);         // 同 exp 會先行累加
    }
    tmp.normalize();
    p = tmp;                          // 深拷貝回 p
    return is;
}

// 友善輸出：省略 1*、處理正負號、x^1 不印 ^1
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) { os << "0"; return os; }

    for (int i = 0; i < p.terms; ++i) {
        float c = p.termArray[i].getCoef();
        int   e = p.termArray[i].getExp();

        // 項與項之間的正負號
        if (i > 0) os << (c >= 0 ? " + " : " - ");
        else if (c < 0) os << "-";

        float ac = std::fabs(c);
        bool printCoef = !(ac == 1.0f && e != 0); // 若係數為 ±1 且 e>0 就省略係數
        if (printCoef) os << ac;

        if (e > 0) {
            if (printCoef) os << "*";
            os << "x";
            if (e > 1) os << "^" << e;
        }
    }
    return os;
}

// ---- 算術運算子 ----

// 加法：類似歸併（兩個已排序串列合併）
Polynomial operator+(const Polynomial& a, const Polynomial& b) {
    Polynomial r(std::max(a.capacity, b.capacity));
    int i = 0, j = 0;
    while (i < a.terms || j < b.terms) {
        if (j == b.terms || (i < a.terms && a.termArray[i].getExp() > b.termArray[j].getExp())) {
            r.append(a.termArray[i].getExp(), a.termArray[i].getCoef());
            ++i;
        } else if (i == a.terms || b.termArray[j].getExp() > a.termArray[i].getExp()) {
            r.append(b.termArray[j].getExp(), b.termArray[j].getCoef());
            ++j;
        } else {
            float c = a.termArray[i].getCoef() + b.termArray[j].getCoef();
            if (std::fabs(c) > 0.0f) r.append(a.termArray[i].getExp(), c);
            ++i; ++j;
        }
    }
    r.normalize(); // 以防萬一（理論上已合併完）
    return r;
}

// 乘法：雙層迴圈逐項相乘＋同 exp 累加
Polynomial operator*(const Polynomial& a, const Polynomial& b) {
    Polynomial r(a.terms + b.terms + 4); // 粗估上限，預留空間
    for (int i = 0; i < a.terms; ++i)
        for (int j = 0; j < b.terms; ++j)
            r.addInPlace(a.termArray[i].getExp() + b.termArray[j].getExp(),
                         a.termArray[i].getCoef() * b.termArray[j].getCoef());
    r.normalize();
    return r;
}

// ---- Demo 主程式 ----
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

1. 時間複雜度：程式的時間複雜度為 *Eval:O(t) Add:O(n+m) Mult:O((nm)^2)*。
2. 空間複雜度：空間複雜度為 *Eval:O(1) Add:O(n+m) Mult:O((nm))*。

## 測試與驗證

### 測試案例

<img width="392" height="156" alt="image" src="https://github.com/user-attachments/assets/a444156d-fe6d-4ae9-b4b7-18cc2f015b3b" />

<img width="595" height="156" alt="image" src="https://github.com/user-attachments/assets/c40ff859-b5e0-459e-9803-2ebb83c43db9" />

<img width="432" height="152" alt="image" src="https://github.com/user-attachments/assets/d5562730-7d0b-4ba9-88d1-d3571196fa34" />



### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 -Wall Homework2.cpp -o hw2.exe
$ .\hw2.exe=
```

### 結論

1. 程式能正確計算當*exp*跟*coef*個別是多少時的答案。  
2. 在*exp*是*0*或*負數*的情況下，程式是否正確，符合設計預期。  

## 心得討論

##為什麼使用 *operator>>*與*operator<<*
*operator>>*：讓「輸入」多項式像基本型別一樣直覺（cin >> p;）。同時能把「讀 n、讀 (exp coef) 組」的規格集中管理，避免主程式散落解析邏輯。
*operator<<*：把「數學友善輸出」集中處理（省略 1*、處理正負號、x^1 不印 ^1），主程式只要 cout << p 就能漂亮輸出，維護性高。

### 選擇*friend*的原因

在本程式中，使用friend來寫這串*code*的主要原因如下：

1. **允許內部直接訪問私有資料**
   在 Polynomial 的成員函式裡（例如 Eval()、normalize()）可以直接寫：  
   s += termArray[i].coef * pow(x, termArray[i].exp); 
   這是可行的，因為 Term 對 Polynomial 是 friend;沒有這行 friend 的話則會出現error。

2. **好處**  
   Polynomial 是唯一能操作 Term 內部的類別。
   外部（例如 main()）不能直接動 coef 或 exp，防止亂改資料。
   
3. **沒有使用friend的情況**  
   因Term的私有資料無法被Polynomial使用，所以coef和exp會是pritave。
   程式碼會變得更長一串、更消耗效能、記憶體之類的。
   因為少了friend就代表不能直接存取，需要跑好幾個流程才可以存取。
   所以程式執行的時間一旦長起來，消耗的東西就會增長。


### 心得
在上學期就有學習過C++中的*friend*，但在課堂實際使用卻很少，這次的功課難得出現可以使用*friend*的程式，因為這次的程式碼很長一串，我當時就在講要怎麼把效能提升上去，不要吃這麼多記憶體，突然就想到了之前學過的*friend*而且剛好，這段程式碼又很常去抓函式裡面的數值或變數。
這次使用*friend*讓我對這個程式有更深入的理解，像是*friend*常使用的方式: *friend class X*
 *friend 函式宣告*，而且我覺得*friend*這個程式很有趣，跟他的名字一樣，是朋友，朋友之間可以互相存取，但朋友的朋友不能存取，很像是雙方的小祕密一樣。
 這次使用*friend*讓我知道他的優缺點，優點是可以緊密合作、保持封裝、參數不容易被修改、被保護得很好。缺點是當濫用了*friend*會讓類別之間相似度過高，很容易改了一邊參數另外一邊也一起更動了，可維護性和測試性會下降，因為太多朋友了會導致邊界模糊。
