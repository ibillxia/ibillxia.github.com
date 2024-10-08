#include<iostream>
#include<cstdio>
using namespace std;

template <class T>
inline void xswap(T &a,T &b){
    a=a+b;
    printf("a=%u\n",a);
    b=a-b;
    a=a-b;
}

template <class T>
inline void xswap2(T &a,T &b){
    a=a-b;
    printf("a=%u\n",a);
    b=a+b;
    a=b-a;
}

template <class T>
inline void xswap3(T &a,T &b){
    a=a^b;
    b=a^b;
    a=b^a;
}

template <class T>
inline void swap3(T &a,T &b,T &c){
    a=a^b;
    b=a^b;
    a=b^a;
    b=b^c;
    c=b^c;
    b=b^c;
}

int main()
{
    unsigned char a=255,b=1,c=127;
    printf("a=%u,b=%u,c=%u\n",a,b,c);
    swap3(a,b,c);
    //swap(a,b);
    printf("a=%u,b=%u,c=%u\n",a,b,c);
    return 0;
}
