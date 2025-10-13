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
