#include<iostream>
#include<vector>
#include<stack>
#include<queue>
#include<string>
#include<map>
#include<algorithm>
#include<cstdio>
#include<cstring>
#include<cmath>
using namespace std;
#define FOR(i,n) for(int i=0;i<n;i++)

class Solution
{
public:
    void recursive_permute(int A[],int i,int n){
        if(i==n-1){
            for(int j=0;j<n;j++)
                cout<<A[j]<<" ";
            cout<<endl;
            return;
        }
        for(int j=i;j<n;j++){
            swap(A[i],A[j]);  // choose A[j] as the ith element
            recursive_permute(A,i+1,n);
            swap(A[i],A[j]);  // reset to enter next for
        }
    }

    void recursive_permute2(int A[],int i,int n){
        if(i==n-1){
            for(int j=0;j<n;j++)
                cout<<A[j]<<" ";
            cout<<endl;
            return;
        }
        for(int j=i;j<n;j++){
            int k;
            for(k=i;k<j;k++)if(A[k]==A[j])break; // A[j] already used
            if(k<j)continue;
            swap(A[i],A[j]);  // choose A[j] as the ith element
            recursive_permute2(A,i+1,n);
            swap(A[i],A[j]);  // reset to enter next for
        }
    }

    bool next_permute(int A[],int n){
        int i,j;
        // step .1
        if(n<2)return false;
        for(i=n-2;i>=0;i--){
            if(A[i]<A[i+1])break;
        }
        if(i<0)return false; // A[0] is maximum, no next permute for it
        // step .2
        for(j=n-1;j>i;j--){
            if(A[j]>A[i])break;
        }
        // step .3
        swap(A[i],A[j]);
        // step .4
        while(++i < --n){
            swap(A[i],A[n]);
        }
        return true;
    }

    void non_recursive_permute(int A[],int n){
        sort(A,A+n);
        int i;
        do{
            for(i=0;i<n;i++)cout<<A[i]<<" ";
            cout<<endl;
        }while(next_permute(A,n));
    }

    vector<vector<int> > permute(vector<int> &num) {
		vector<vector<int> > ans;
		sort(num.begin(),num.end());
		do{
			ans.push_back(num);
		}while(next_permutation(num.begin(),num.end()));
		return ans;
    }
};

/**
 *  @brief  Permute range into the next @a dictionary ordering.
 *  @ingroup sorting_algorithms
 *  @param  first  Start of range.
 *  @param  last   End of range.
 *  @return  False if wrapped to first permutation, true otherwise.
 *
 *  Treats all permutations of the range as a set of @a dictionary sorted
 *  sequences.  Permutes the current sequence into the next one of this set.
 *  Returns true if there are more sequences to generate.  If the sequence
 *  is the largest of the set, the smallest is generated and false returned.
*/
/*
template<typename _BidirectionalIterator>
bool
next_permutation(_BidirectionalIterator __first,
                 _BidirectionalIterator __last)
{
    // concept requirements
    __glibcxx_function_requires(_BidirectionalIteratorConcept<
                                _BidirectionalIterator>)
    __glibcxx_function_requires(_LessThanComparableConcept<
                                typename iterator_traits<_BidirectionalIterator>::value_type>)
    __glibcxx_requires_valid_range(__first, __last);

    if (__first == __last)
        return false;
    _BidirectionalIterator __i = __first;
    ++__i;
    if (__i == __last)
        return false;
    __i = __last;
    --__i;

    for(;;) {
        _BidirectionalIterator __ii = __i;
        --__i;
        if (*__i < *__ii) {
            _BidirectionalIterator __j = __last;
            while (!(*__i < *--__j)) {
            }
            std::iter_swap(__i, __j);
            std::reverse(__ii, __last);
            return true;
        }
        if (__i == __first) {
            std::reverse(__first, __last);
            return false;
        }
    }
}
*/

int main()
{
    Solution sol;
    int a[]={1,1,3,4,5};
    sol.non_recursive_permute(a,3);
    return 0;
}
