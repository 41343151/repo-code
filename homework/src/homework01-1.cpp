#include <iostream>
#include <iomanip>
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
    cout<<"請輸入兩個整數 m 和 n ";
    cin>>m>>n;
    cout<<"A("<<m<<","<<n<<")="<<Ackermann(m,n)<<endl;
    return 0;
}