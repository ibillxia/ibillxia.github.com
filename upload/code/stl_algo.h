/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_ALGO_H
#define __SGI_STL_INTERNAL_ALGO_H

#include <stl_heap.h>

// See concept_checks.h for the concept-checking macros 
// __STL_REQUIRES, __STL_CONVERTIBLE, etc.


__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

// __median (an extension, not present in the C++ standard).
// 求三个元素的中值
template <class _Tp>
inline const _Tp& __median(const _Tp& __a, const _Tp& __b, const _Tp& __c) {
  __STL_REQUIRES(_Tp, _LessThanComparable);
  if (__a < __b) // a<b
    if (__b < __c) // a<b<c
      return __b;
    else if (__a < __c) // a<c<=b
      return __c;
    else // c<=a<b
      return __a;
  else if (__a < __c) // b<=a<c, 这里else if是跟最外层的if(a<b)配对
    return __a;
  else if (__b < __c) // b<c<=a, 这里else if是跟最外层的if(a<b)配对
    return __c;
  else // c<=b<=a
    return __b;
}
// 求三个元素的中值，指定比较大小的函数
template <class _Tp, class _Compare>
inline const _Tp&
__median(const _Tp& __a, const _Tp& __b, const _Tp& __c, _Compare __comp) {
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  if (__comp(__a, __b))
    if (__comp(__b, __c))
      return __b;
    else if (__comp(__a, __c))
      return __c;
    else
      return __a;
  else if (__comp(__a, __c))
    return __a;
  else if (__comp(__b, __c))
    return __c;
  else
    return __b;
}
// 对区间 [first, last) 内的所有元素执行函数 f
// for_each.  Apply a function to every element of a range.
template <class _InputIter, class _Function>
_Function for_each(_InputIter __first, _InputIter __last, _Function __f) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  for ( ; __first != __last; ++__first)
    __f(*__first);
  return __f;
}

// find and find_if.
// 查找区间 [first, last) 内是否存在值为 val 的元素，这里有四个参数，一般用户不是直接使用这个函数，而是调用line 197的那个
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val,
                       input_iterator_tag)
{
  while (__first != __last && !(*__first == __val))
    ++__first;
  return __first;
}
// 查找区间 [first, last) 内是否存在经过 pred 函数运算后结果为 true 的元素
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred,
                          input_iterator_tag)
{
  while (__first != __last && !__pred(*__first))
    ++__first;
  return __first;
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION // 具有类偏特化特性
// 偏特化版本1，元素可以随机存取，使用[loop unrolling](http://en.wikipedia.org/wiki/Loop_unrolling)
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter find(_RandomAccessIter __first, _RandomAccessIter __last,
                       const _Tp& __val,
                       random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;
  // 关于这个优化的trick请参见 http://stackoverflow.com/questions/24295972/
  for ( ; __trip_count > 0 ; --__trip_count) {
    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;
  }

  switch(__last - __first) { // 最后不被4整除的几个元素
  case 3:
    if (*__first == __val) return __first;
    ++__first;
  case 2:
    if (*__first == __val) return __first;
    ++__first;
  case 1:
    if (*__first == __val) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}
// 偏特化版本1，元素可以随机存取，可以指定查找条件（经过函数pred运算后为true）
template <class _RandomAccessIter, class _Predicate>
_RandomAccessIter find_if(_RandomAccessIter __first, _RandomAccessIter __last,
                          _Predicate __pred,
                          random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (__pred(*__first)) return __first;
    ++__first;
  case 2:
    if (__pred(*__first)) return __first;
    ++__first;
  case 1:
    if (__pred(*__first)) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
// 用户实际用得最多的查找算法的接口
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
            typename iterator_traits<_InputIter>::value_type, _Tp);
  return find(__first, __last, __val, __ITERATOR_CATEGORY(__first));
}
// 用户实际用得最多的 find_if 算法的接口
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
          typename iterator_traits<_InputIter>::value_type);
  return find_if(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}

// adjacent_find.
// 查找相邻两元素相等的位置（返回相邻元素中第一个元素的迭代器）
template <class _ForwardIter>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
    if (*__first == *__next)
      return __first;
    __first = __next;
  }
  return __last;
}
// 查找相邻两元素满足运算__binary_pred的位置（返回相邻元素中第一个元素的迭代器）
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last,
                           _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
          typename iterator_traits<_ForwardIter>::value_type,
          typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
    if (__binary_pred(*__first, *__next))
      return __first;
    __first = __next;
  }
  return __last;
}

// count and count_if.  There are two version of each, one whose return type
// type is void and one (present only if we have partial specialization)
// whose return type is iterator_traits<_InputIter>::difference_type.  The
// C++ standard only has the latter version, but the former, which was present
// in the HP STL, is retained for backward compatibility.
// SGI STL 特有的计数算法
template <class _InputIter, class _Tp, class _Size>
void count(_InputIter __first, _InputIter __last, const _Tp& __value,
           _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  for ( ; __first != __last; ++__first)
    if (*__first == __value)
      ++__n;
}
// SGI STL 特有的，可以用仿函数指定判断条件的版本
template <class _InputIter, class _Predicate, class _Size>
void count_if(_InputIter __first, _InputIter __last, _Predicate __pred,
              _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      ++__n;
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
// 具有偏特化特性时的计数，返回值类型为迭代器的difference_type
template <class _InputIter, class _Tp>
typename iterator_traits<_InputIter>::difference_type
count(_InputIter __first, _InputIter __last, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  for ( ; __first != __last; ++__first)
    if (*__first == __value)
      ++__n;
  return __n;
}
// 同上，只是可以通过仿函数pred指定判断条件
template <class _InputIter, class _Predicate>
typename iterator_traits<_InputIter>::difference_type
count_if(_InputIter __first, _InputIter __last, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      ++__n;
  return __n;
}


#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// search. 搜索
// 查找序列1中是否存在子序列2，查找到则返回序列中子序列的起始迭代器
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2) // 区间2仅包含一个元素
    return find(__first1, __last1, *__first2);

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {
    __first1 = find(__first1, __last1, *__first2);
    if (__first1 == __last1) // 在区间1中没有找到*first2
      return __last1;

    __p = __p1; // p1=first2+1, ref to line 338
    __current = __first1; 
    if (++__current == __last1) // 区间1结束了，说明搜索失败（区间2长度>1）
      return __last1;

    while (*__current == *__p) { // 查找后续是否一一相等
      if (++__p == __last2) // 查到了序列2的结束位置，说明搜索成功
        return __first1; // 返回序列1的起始位置
      if (++__current == __last1) // 序列1没结束而序列2结束，搜索失败
        return __last1;
    }

    ++__first1;
  }
  return __first1;
}
// 同上，只是可以指定为其他判定条件（而不一定是相等）
template <class _ForwardIter1, class _ForwardIter2, class _BinaryPred>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2,
                     _BinaryPred  __predicate) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2) {
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    return __first1;    
  }

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {
    while (__first1 != __last1) {
      if (__predicate(*__first1, *__first2))
        break;
      ++__first1;
    }
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    if (__first1 == __last1)
      return __last1;

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1) return __last1;

    while (__predicate(*__current, *__p)) {
      if (++__p == __last2)
        return __first1;
      if (++__current == __last1)
        return __last1;
    }

    ++__first1;
  }
  return __first1;
}

// search_n.  Search for __count consecutive copies of __val.
// 搜索序列中连续出现count个val的序列的起始位置
template <class _ForwardIter, class _Integer, class _Tp>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);

  if (__count <= 0)
    return __first;
  else {
    __first = find(__first, __last, __val); // 找到第一个val的位置
    while (__first != __last) {
      _Integer __n = __count - 1;
      _ForwardIter __i = __first;
      ++__i;
      while (__i != __last && __n != 0 && *__i == __val) {
        ++__i;
        --__n;
      }
      if (__n == 0)
        return __first;
      else
        __first = find(__i, __last, __val); // 找到第一个val的位置
    }
    return __last;
  }
}

template <class _ForwardIter, class _Integer, class _Tp, class _BinaryPred>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val,
                      _BinaryPred __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool, 
             typename iterator_traits<_ForwardIter>::value_type, _Tp);
  if (__count <= 0)
    return __first;
  else {
    while (__first != __last) {
      if (__binary_pred(*__first, __val))
        break;
      ++__first;
    }
    while (__first != __last) {
      _Integer __n = __count - 1;
      _ForwardIter __i = __first;
      ++__i;
      while (__i != __last && __n != 0 && __binary_pred(*__i, __val)) {
        ++__i;
        --__n;
      }
      if (__n == 0)
        return __first;
      else {
        while (__i != __last) {
          if (__binary_pred(*__i, __val))
            break;
          ++__i;
        }
        __first = __i;
      }
    }
    return __last;
  }
} 

// swap_ranges
// 交换两个区间的元素
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter2 swap_ranges(_ForwardIter1 __first1, _ForwardIter1 __last1,
                          _ForwardIter2 __first2) {
  __STL_REQUIRES(_ForwardIter1, _Mutable_ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter1>::value_type,
                    typename iterator_traits<_ForwardIter2>::value_type);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter2>::value_type,
                    typename iterator_traits<_ForwardIter1>::value_type);
  for ( ; __first1 != __last1; ++__first1, ++__first2)
    iter_swap(__first1, __first2); // 迭代器的交换，使用iter_swap
  return __first2;
}

// transform
// 对区间的每个元素进行opr运算，结果放在result中，这一点与for_each不同
template <class _InputIter, class _OutputIter, class _UnaryOperation>
_OutputIter transform(_InputIter __first, _InputIter __last,
                      _OutputIter __result, _UnaryOperation __opr) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);

  for ( ; __first != __last; ++__first, ++__result)
    *__result = __opr(*__first);
  return __result;
}
// 两个等长序列的运算，结果放在result中（注意输入参数中没有last2）
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _BinaryOperation>
_OutputIter transform(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _OutputIter __result,
                      _BinaryOperation __binary_op) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
    *__result = __binary_op(*__first1, *__first2);
  return __result;
}

// replace, replace_if, replace_copy, replace_copy_if
// 将序列中所有值为oldval的元素值都改为newval
template <class _ForwardIter, class _Tp>
void replace(_ForwardIter __first, _ForwardIter __last,
             const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (*__first == __old_value)
      *__first = __new_value;
}
// 同上，只是判断条件可以自己指定
template <class _ForwardIter, class _Predicate, class _Tp>
void replace_if(_ForwardIter __first, _ForwardIter __last,
                _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      *__first = __new_value;
}
// 将修改后的结果存到新的序列中
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter replace_copy(_InputIter __first, _InputIter __last,
                         _OutputIter __result,
                         const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = *__first == __old_value ? __new_value : *__first;
  return __result;
}
// 以上两个的合体，既可以指定判断条件，还将修改后的结果存到新的序列
template <class _InputIter, class _OutputIter, class _Predicate, class _Tp>
_OutputIter replace_copy_if(_InputIter __first, _InputIter __last,
                            _OutputIter __result,
                            _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
                typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = __pred(*__first) ? __new_value : *__first;
  return __result;
}

// generate and generate_n
// 将序列中的元素的值按给定函数赋值
template <class _ForwardIter, class _Generator>
void generate(_ForwardIter __first, _ForwardIter __last, _Generator __gen) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_GENERATOR_CHECK(_Generator, 
          typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    *__first = __gen();
}
// 将序列中的前n个元素的值按给定函数赋值
template <class _OutputIter, class _Size, class _Generator>
_OutputIter generate_n(_OutputIter __first, _Size __n, _Generator __gen) {
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __n > 0; --__n, ++__first)
    *__first = __gen();
  return __first;
}

// remove, remove_if, remove_copy, remove_copy_if
// 移除序列中值为val的元素，结果存到新的序列
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter remove_copy(_InputIter __first, _InputIter __last,
                        _OutputIter __result, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first)
    if (!(*__first == __value)) {
      *__result = *__first;
      ++__result;
    }
  return __result;
}
// 同上，只是移除条件可以自己指定
template <class _InputIter, class _OutputIter, class _Predicate>
_OutputIter remove_copy_if(_InputIter __first, _InputIter __last,
                           _OutputIter __result, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (!__pred(*__first)) {
      *__result = *__first;
      ++__result;
    }
  return __result;
}
// 移除序列中值为val的元素，质变版本
template <class _ForwardIter, class _Tp>
_ForwardIter remove(_ForwardIter __first, _ForwardIter __last,
                    const _Tp& __value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __first = find(__first, __last, __value);
  _ForwardIter __i = __first;
  return __first == __last ? __first 
                           : remove_copy(++__i, __last, __first, __value);
}
// 移除满足条件的元素，质变版本
template <class _ForwardIter, class _Predicate>
_ForwardIter remove_if(_ForwardIter __first, _ForwardIter __last,
                       _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
               typename iterator_traits<_ForwardIter>::value_type);
  __first = find_if(__first, __last, __pred);
  _ForwardIter __i = __first;
  return __first == __last ? __first 
                           : remove_copy_if(++__i, __last, __first, __pred);
}

// unique and unique_copy
// func1：将区间的元素的值唯一化，即去掉相邻的重复的项，带copy是非质变算法
template <class _InputIter, class _OutputIter, class _Tp> // 这里需要借助模板参数Tp来进行型别推导，关于这一点参考介绍iterators的那篇文章
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result, _Tp*) {
  _Tp __value = *__first;
  *__result = __value; 
  while (++__first != __last)
    if (!(__value == *__first)) { // 不相等时result才向后移
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}
// func2：调用上面的函数（func1），该函数中主要是迭代器型别萃取
template <class _InputIter, class _OutputIter>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result, 
                                 output_iterator_tag) { // tag为output_iterator
  return __unique_copy(__first, __last, __result, __VALUE_TYPE(__first)); // 型别萃取
}
// func3：输出迭代器result的类型为ForwardIter时的重载版本，直接实现（而不是像func2要调用func1）
template <class _InputIter, class _ForwardIter>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, forward_iterator_tag) { // tag为forward_iterator
  *__result = *__first;
  while (++__first != __last)
    if (!(*__result == *__first))
      *++__result = *__first;
  return ++__result;
}
// func4（对外接口）：先判断区间是否为空，再调用func2或func3（？什么时候会调用func3）
template <class _InputIter, class _OutputIter>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  if (__first == __last) return __result;
  return __unique_copy(__first, __last, __result,
                       __ITERATOR_CATEGORY(__result));
}
// func5：可以指定判断条件（而不一定是相等就移除）的版本
template <class _InputIter, class _OutputIter, class _BinaryPredicate,
          class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result,
                          _BinaryPredicate __binary_pred, _Tp*) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, _Tp, _Tp);
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!__binary_pred(__value, *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}
// func6：同上，只是该函数只对型别进行萃取，具体的实现还是调用func5
template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result,
                                 _BinaryPredicate __binary_pred,
                                 output_iterator_tag) { // tag为output_iterator
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __VALUE_TYPE(__first));
}
// func7：同上，只是输出迭代器型别为ForwardIter
template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, 
                           _BinaryPredicate __binary_pred,
                           forward_iterator_tag) { // tag为forward_iterator
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_ForwardIter>::value_type,
     typename iterator_traits<_InputIter>::value_type);
  *__result = *__first;
  while (++__first != __last)
    if (!__binary_pred(*__result, *__first)) *++__result = *__first;
  return ++__result;
}
// func8（对外接口）：先判断区间是否为空，在调用func6或func7（？什么时候会调用func7）
template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result,
                               _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  if (__first == __last) return __result;
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __ITERATOR_CATEGORY(__result));
}
// func9（对外接口）：先找到连续相等的两个元素的起始位置，再使用非质变算法进行unique化
template <class _ForwardIter>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __first = adjacent_find(__first, __last); // 找到需要unique化的起始位置
  return unique_copy(__first, __last, __first); // 调用func4进行unique化
}
// func10（对外接口）：同上，只是可以指定判定条件
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last,
                    _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, 
      typename iterator_traits<_ForwardIter>::value_type,
      typename iterator_traits<_ForwardIter>::value_type);
  __first = adjacent_find(__first, __last, __binary_pred); // 找到需要unique化的起始位置
  return unique_copy(__first, __last, __first, __binary_pred); // 调用func8进行unique化
}

// reverse and reverse_copy, and their auxiliary functions
// 反转，迭代器为双向迭代器
template <class _BidirectionalIter>
void __reverse(_BidirectionalIter __first, _BidirectionalIter __last, 
               bidirectional_iterator_tag) {
  while (true)
    if (__first == __last || __first == --__last)
      return;
    else
      iter_swap(__first++, __last);
}
// 反转，迭代器为随机存取迭代器
template <class _RandomAccessIter>
void __reverse(_RandomAccessIter __first, _RandomAccessIter __last,
               random_access_iterator_tag) {
  while (__first < __last)
    iter_swap(__first++, --__last);
}
// 反转（对外接口），质变算法版本
template <class _BidirectionalIter>
inline void reverse(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __reverse(__first, __last, __ITERATOR_CATEGORY(__first));
}
// 反转，非质变版本
template <class _BidirectionalIter, class _OutputIter>
_OutputIter reverse_copy(_BidirectionalIter __first,
                         _BidirectionalIter __last,
                         _OutputIter __result) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  while (__first != __last) {
    --__last;
    *__result = *__last;
    ++__result;
  }
  return __result;
}

// rotate and rotate_copy, and their auxiliary functions
// 求最大公约数（_EuclideanRingElement可以为那些类型？[const/signed/unsigned]short/int/long?）
template <class _EuclideanRingElement>
_EuclideanRingElement __gcd(_EuclideanRingElement __m,
                            _EuclideanRingElement __n)
{
  while (__n != 0) {
    _EuclideanRingElement __t = __m % __n;
    __m = __n;
    __n = __t;
  }
  return __m;
}
// 旋转，迭代器为ForwardIter，以middle为起点，将[first,middle)+[middle,last)->[middle,last)+[first,middle)
template <class _ForwardIter, class _Distance>
_ForwardIter __rotate(_ForwardIter __first,
                      _ForwardIter __middle,
                      _ForwardIter __last,
                      _Distance*,
                      forward_iterator_tag) {
  if (__first == __middle) // middle为first时旋转后middle=last
    return __last;
  if (__last  == __middle) // middle为last时旋转后middle=first
    return __first;
  // 先将middle之后的元素全部对调到middle之前
  _ForwardIter __first2 = __middle;
  do {
    swap(*__first++, *__first2++); // 将middle之前的数与middle之后的数对调
    if (__first == __middle) // middle之前的元素个数较少，从middle（新的first）开始旋转（first2为新的middle）
      __middle = __first2;   // 如果还是理解不了，可以画个图，以1~9为例，选取其中一个数作为旋转起点（middle）
  } while (__first2 != __last); // first2=last说明middle之前的元素个数大于middle之后的元素的个数

  _ForwardIter __new_middle = __first;
  // 再将middle之后的元素进行调整，使之按在middle之前时的顺序排列
  __first2 = __middle; 

  while (__first2 != __last) {
    swap (*__first++, *__first2++);
    if (__first == __middle) // middle之前调整完了，重新以first2为middle进行调整
      __middle = __first2;
    else if (__first2 == __last) // middle之后的调整完了，重新以middle为first2进行调整
      __first2 = __middle;
  }

  return __new_middle; // 返回旋转之后新的middle的值
}

// 迭代器为双向迭代器的版本
template <class _BidirectionalIter, class _Distance>
_BidirectionalIter __rotate(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance*,
                            bidirectional_iterator_tag) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  if (__first == __middle)
    return __last;
  if (__last  == __middle)
    return __first;

  __reverse(__first,  __middle, bidirectional_iterator_tag()); // 将[first,middle)反序
  __reverse(__middle, __last,   bidirectional_iterator_tag()); // 将[middle,last)反序
  // 对调，长度为min(middle-first,last-middle)
  while (__first != __middle && __middle != __last)
    swap (*__first++, *--__last);
  // middle之前的元素较少，对middle之后last之前的数据反序
  if (__first == __middle) {
    __reverse(__middle, __last,   bidirectional_iterator_tag());
    return __last;
  }
  else { // middle之后的元素较少，对middle之前first之后的数据进行反序
    __reverse(__first,  __middle, bidirectional_iterator_tag());
    return __first;
  }
}
// 迭代器为随机访问迭代器
template <class _RandomAccessIter, class _Distance, class _Tp>
_RandomAccessIter __rotate(_RandomAccessIter __first,
                           _RandomAccessIter __middle,
                           _RandomAccessIter __last,
                           _Distance *, _Tp *) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  _Distance __n = __last   - __first; // 序列总长度
  _Distance __k = __middle - __first; // middle之前的子序列的长度
  _Distance __l = __n - __k; // middle之后的子序列的长度
  _RandomAccessIter __result = __first + (__last - __middle);

  if (__k == 0) // middle=first，旋转后middle=last
    return __last;

  else if (__k == __l) { // middle刚好在序列的正中间，将前后区间进行对调即可
    swap_ranges(__first, __middle, __middle);
    return __result;
  }
  // 假设输入为1~9，n=9，middle为4或7
  _Distance __d = __gcd(__n, __k); // 最大公约数，d=3
  // 为了更好的理解，可以参见http://www.cs.bell-labs.com/cm/cs/pearls/s02b.pdf \
  for (_Distance __i = 0; __i < __d; __i++) { // 其中 A Juggling Algorithm 的图很形象的描述了该算法的原理
    _Tp __tmp = *__first; // 先把*first暂存起来
    _RandomAccessIter __p = __first;

    if (__k < __l) { // middle之前元素较少，设middle=4，则k=3，l=6
      for (_Distance __j = 0; __j < __l/__d; __j++) { // l/d=6/3=2
        if (__p > __first + __l) {
          *__p = *(__p - __l);
          __p -= __l;
        }

        *__p = *(__p + __k); // 调整的跨度为k
        __p += __k;
      }
    }

    else { // middle之后元素较少，设middle=7，类似的推导
      for (_Distance __j = 0; __j < __k/__d - 1; __j ++) {
        if (__p < __last - __k) {
          *__p = *(__p + __k);
          __p += __k;
        }

        *__p = * (__p - __l);
        __p -= __l;
      }
    }

    *__p = __tmp; // 将tmp=*first直接赋给最终它应属的位置
    ++__first;
  }

  return __result;
}
// 最终对外接口，质变版本
template <class _ForwardIter>
inline _ForwardIter rotate(_ForwardIter __first, _ForwardIter __middle,
                           _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  return __rotate(__first, __middle, __last,
                  __DISTANCE_TYPE(__first),
                  __ITERATOR_CATEGORY(__first));
}
// 最终对外接口，非质变版本
template <class _ForwardIter, class _OutputIter>
_OutputIter rotate_copy(_ForwardIter __first, _ForwardIter __middle,
                        _ForwardIter __last, _OutputIter __result) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  return copy(__first, __middle, copy(__middle, __last, __result));
}

// Return a random number in the range [0, __n).  This function encapsulates
// whether we're using rand (part of the standard C library) or lrand48
// (not standard, but a much better choice whenever it's available).

template <class _Distance>
inline _Distance __random_number(_Distance __n) {
#ifdef __STL_NO_DRAND48
  return rand() % __n;
#else
  return lrand48() % __n;
#endif
}

// random_shuffle
// 将序列随机重排
template <class _RandomAccessIter>
inline void random_shuffle(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i) // 循环遍历一次
    iter_swap(__i, __first + __random_number((__i - __first) + 1)); // 每次进行一次随机对调
}
// 同上，只是可以指定随机数生成函数
template <class _RandomAccessIter, class _RandomNumberGenerator>
void random_shuffle(_RandomAccessIter __first, _RandomAccessIter __last,
                    _RandomNumberGenerator& __rand) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + __rand((__i - __first) + 1));
}

// random_sample and random_sample_n (extensions, not part of the standard).
// 从序列中随机选取n个元素（n大于序列长度时，全选）
template <class _ForwardIter, class _OutputIter, class _Distance>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining); // remaining = last-first
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__random_number(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m; // m在减小，最后生成m个数
    }

    --__remaining; // remaining 一直在减小，remaining>m时随机选取，<m时就全部选取(line 1010)
    ++__first;
  }
  return __out;
}
// 同上，只是可以指定随机数生成函数
template <class _ForwardIter, class _OutputIter, class _Distance,
          class _RandomNumberGenerator>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n,
                            _RandomNumberGenerator& __rand)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining);
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__rand(__remaining) < __m) { // 使用自定义随机数生成函数
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}
// 迭代器为随机访问迭代器，同样是生成n个数，且是乱序的（不一定按原序列顺序）
template <class _InputIter, class _RandomAccessIter, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  const _Distance __n)
{
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)  // 前n个数全选
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __random_number(__t);
    if (__M < __n)
      __out[__M] = *__first; // 将前n个数中的第M个数重置为当前元素的值
    ++__first;
  }

  return __out + __m;
}
// 同上，只是可以指定随机数生成函数
template <class _InputIter, class _RandomAccessIter,
          class _RandomNumberGenerator, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  _RandomNumberGenerator& __rand,
                                  const _Distance __n)
{
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __rand(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}
// 从序列中随机选择n个数
template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __out_last - __out_first);
}

// 同上，只是可以指定随机数生成函数
template <class _InputIter, class _RandomAccessIter, 
          class _RandomNumberGenerator>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last,
              _RandomNumberGenerator& __rand) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __rand,
                         __out_last - __out_first);
}

// partition, stable_partition, and their auxiliary functions
// 将序列分割成
template <class _ForwardIter, class _Predicate>
_ForwardIter __partition(_ForwardIter __first,
		         _ForwardIter __last,
			 _Predicate   __pred,
			 forward_iterator_tag) {
  if (__first == __last) return __first;
  // 先找到分割点，满足pred(*first)=false
  while (__pred(*__first))
    if (++__first == __last) return __first;
  // 进行分割，分割点之前的子序列不变，分割点之后的元素可能会前移
  _ForwardIter __next = __first;
  
  while (++__next != __last)
    if (__pred(*__next)) { // 满足该条件的元素前移到紧跟分割点后，它变成新的分割点
      swap(*__first, *__next);
      ++__first;
    }
  // 最后，所有满足条件pred的元素都在分割点之前，而不满足条件的都在分割点之后
  return __first; // 返回分割点的位置
}
// 同上，只是迭代器的类型为双向迭代器
template <class _BidirectionalIter, class _Predicate>
_BidirectionalIter __partition(_BidirectionalIter __first,
                               _BidirectionalIter __last,
			       _Predicate __pred,
			       bidirectional_iterator_tag) {
  while (true) { // 从前面找到第一个不满足条件的，从最后找到第一个满足条件的，然后将二者对调
    while (true) // 从前面找到第一个不满足条件的
      if (__first == __last) // 结束
        return __first;
      else if (__pred(*__first)) // 满足条件，后移
        ++__first;
      else // 不满足条件pred，退出循环
        break;
    --__last;
    while (true) // 从最后找到第一个满足条件的
      if (__first == __last)
        return __first;
      else if (!__pred(*__last))
        --__last;
      else
        break;
    iter_swap(__first, __last); // 将二者对调
    ++__first;
  }
}
// 对外接口
template <class _ForwardIter, class _Predicate>
inline _ForwardIter partition(_ForwardIter __first,
   			      _ForwardIter __last,
			      _Predicate   __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
        typename iterator_traits<_ForwardIter>::value_type);
  return __partition(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}
// 原地、稳定的分割算法
// 先将序列分成两等分，分别进行分割，然后将第一个序列的后半部分与第二个序列的前半部分进行对调（使用旋转）
template <class _ForwardIter, class _Predicate, class _Distance>
_ForwardIter __inplace_stable_partition(_ForwardIter __first,
                                        _ForwardIter __last,
                                        _Predicate __pred, _Distance __len) {
  if (__len == 1)
    return __pred(*__first) ? __last : __first;
  _ForwardIter __middle = __first;
  advance(__middle, __len / 2); // 将middle指向序列的中间位置
  return rotate(__inplace_stable_partition(__first, __middle, __pred, 
                                           __len / 2),
                __middle,
                __inplace_stable_partition(__middle, __last, __pred,
                                           __len - __len / 2));
}
// 在保证稳定性的前提下自适应选择就地分割或借助buffer进行分割
template <class _ForwardIter, class _Pointer, class _Predicate, 
          class _Distance>
_ForwardIter __stable_partition_adaptive(_ForwardIter __first,
                                         _ForwardIter __last,
                                         _Predicate __pred, _Distance __len,
                                         _Pointer __buffer,
                                         _Distance __buffer_size) 
{
  if (__len <= __buffer_size) { // 如果buffer空间足够，借用buffer来进行分割
    _ForwardIter __result1 = __first;
    _Pointer __result2 = __buffer;
    for ( ; __first != __last ; ++__first)
      if (__pred(*__first)) {
        *__result1 = *__first;
        ++__result1;
      }
      else { // 将不满足pre条件的放在buffer中
        *__result2 = *__first;
        ++__result2;
      }
    copy(__buffer, __result2, __result1); // 将buffer序列合并到result1中
    return __result1;
  }
  else { // buffer不足，递归分割子序列在就地旋转
    _ForwardIter __middle = __first;
    advance(__middle, __len / 2);
    return rotate(__stable_partition_adaptive(
                          __first, __middle, __pred,
                          __len / 2, __buffer, __buffer_size),
                    __middle,
                    __stable_partition_adaptive(
                          __middle, __last, __pred,
                          __len - __len / 2, __buffer, __buffer_size));
  }
}
// 稳定分割，辅助函数
template <class _ForwardIter, class _Predicate, class _Tp, class _Distance>
inline _ForwardIter
__stable_partition_aux(_ForwardIter __first, _ForwardIter __last, 
                       _Predicate __pred, _Tp*, _Distance*)
{
  _Temporary_buffer<_ForwardIter, _Tp> __buf(__first, __last);
  if (__buf.size() > 0) // buffer非空
    return __stable_partition_adaptive(__first, __last, __pred,
                                       _Distance(__buf.requested_size()),
                                       __buf.begin(), __buf.size());
  else
    return __inplace_stable_partition(__first, __last, __pred, 
                                      _Distance(__buf.requested_size()));
}
// 对外接口
template <class _ForwardIter, class _Predicate>
inline _ForwardIter stable_partition(_ForwardIter __first,
                                     _ForwardIter __last, 
                                     _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
      typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __first;
  else
    return __stable_partition_aux(__first, __last, __pred,
                                  __VALUE_TYPE(__first),
                                  __DISTANCE_TYPE(__first));
}
// 随机访问迭代器，分割条件为元素小于指定值pivot
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot) 
{
  while (true) { 
    while (*__first < __pivot) // 从前面开始找到第一个不满足条件的
      ++__first;
    --__last;
    while (__pivot < *__last) // 从尾部开始找到第一个满足条件的
      --__last;
    if (!(__first < __last))
      return __first;
    iter_swap(__first, __last); // 互换
    ++__first;
  }
}    
// 随机访问迭代器，分割条件由用户指定
template <class _RandomAccessIter, class _Tp, class _Compare>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot, _Compare __comp) 
{
  while (true) {
    while (__comp(*__first, __pivot))
      ++__first;
    --__last;
    while (__comp(__pivot, *__last))
      --__last;
    if (!(__first < __last))
      return __first;
    iter_swap(__first, __last);
    ++__first;
  }
}

const int __stl_threshold = 16;

// sort() and its auxiliary functions.  排序算法
// 线性插入
template <class _RandomAccessIter, class _Tp>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val) {
  _RandomAccessIter __next = __last;
  --__next;
  while (__val < *__next) { // val比当前值小就一直往前找
    *__last = *__next;
    __last = __next;
    --__next;
  }
  *__last = __val;
}
// 线性插入，比较大小方法可以用户指定
template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val, 
                               _Compare __comp) {
  _RandomAccessIter __next = __last;
  --__next;  
  while (__comp(__val, *__next)) { // 比较大小
    *__last = *__next;
    __last = __next;
    --__next;
  }
  *__last = __val;
}
// 线性插入，考虑了插入值比第一个元素小
template <class _RandomAccessIter, class _Tp>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*) {
  _Tp __val = *__last;
  if (__val < *__first) {
    copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else
    __unguarded_linear_insert(__last, __val);
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Tp, class _Compare>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*, _Compare __comp) {
  _Tp __val = *__last;
  if (__comp(__val, *__first)) {
    copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else
    __unguarded_linear_insert(__last, __val, __comp);
}
// 插入排序
template <class _RandomAccessIter>
void __insertion_sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  if (__first == __last) return; 
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i) // 从第二个元素开始
    __linear_insert(__first, __i, __VALUE_TYPE(__first));
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Compare>
void __insertion_sort(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Compare __comp) {
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    __linear_insert(__first, __i, __VALUE_TYPE(__first), __comp);
}
// 不考虑插入元素是否小于第一个元素
template <class _RandomAccessIter, class _Tp>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last, _Tp*) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i));
}
// 同上
template <class _RandomAccessIter>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                _RandomAccessIter __last) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first));
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last,
                                    _Tp*, _Compare __comp) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i), __comp);
}
// 同上
template <class _RandomAccessIter, class _Compare>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                       _RandomAccessIter __last,
                                       _Compare __comp) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first),
                                 __comp);
}
// 最终的插入排序
template <class _RandomAccessIter>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last) {
  if (__last - __first > __stl_threshold) { // 序列长度大于 16时
    __insertion_sort(__first, __first + __stl_threshold); // 对前16个插入排序
    __unguarded_insertion_sort(__first + __stl_threshold, __last); // 对余下部分进行插入排序
  } // ？问题是前16个数中有比剩余数中最小数大的数肿么办？（其实不会出现这种情况，因为经过分割后前面的数必比后面的数小）
  else // 序列长度小于16
    __insertion_sort(__first, __last); // 直接使用插入排序
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Compare>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first > __stl_threshold) {
    __insertion_sort(__first, __first + __stl_threshold, __comp);
    __unguarded_insertion_sort(__first + __stl_threshold, __last, __comp);
  }
  else
    __insertion_sort(__first, __last, __comp);
}
// 计算n的二进制最高位的位数
template <class _Size>
inline _Size __lg(_Size __n) {
  _Size __k;
  for (__k = 0; __n != 1; __n >>= 1) ++__k;
  return __k;
}
// 
template <class _RandomAccessIter, class _Tp, class _Size>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit)
{
  while (__last - __first > __stl_threshold) {
    if (__depth_limit == 0) { // 达到递归深度的限制了
      partial_sort(__first, __last, __last); // partial_sort实际使用heap sort实现
      return;
    }
    --__depth_limit;
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1)))); // 分割，使用前、中、后三个数的中值作为分割值
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit); // 后半部分递归
    __last = __cut; // 前半部分
  }
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Tp, class _Size, class _Compare>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit, _Compare __comp)
{
  while (__last - __first > __stl_threshold) {
    if (__depth_limit == 0) { // 超过递归次数限制
      partial_sort(__first, __last, __last, __comp); // 使用heap sort
      return;
    }
    --__depth_limit;
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1), __comp)),
       __comp);
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit, __comp);
    __last = __cut;
  }
}
// 最终的sort对外接口
template <class _RandomAccessIter>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2);
    __final_insertion_sort(__first, __last);
  }
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Compare>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last,
                 _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2,
                     __comp);
    __final_insertion_sort(__first, __last, __comp);
  }
}

// stable_sort() and its auxiliary functions.
// 稳定排序算法，基本思想是归并排序，或者说是merge sort
template <class _RandomAccessIter>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  if (__last - __first < 15) { // 这里为啥直接用15？不用一个变量来表示？？为啥不是前面定义过的__stl_threshold=16？？？
    __insertion_sort(__first, __last);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle);
  __inplace_stable_sort(__middle, __last);
  __merge_without_buffer(__first, __middle, __last,
                         __middle - __first,
                         __last - __middle);
}
// 同上，只是可以指定比较大小的方法
template <class _RandomAccessIter, class _Compare>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first < 15) {
    __insertion_sort(__first, __last, __comp);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle, __comp);
  __inplace_stable_sort(__middle, __last, __comp);
  __merge_without_buffer(__first, __middle, __last,
                         __middle - __first,
                         __last - __middle,
                         __comp);
}
// 将若干对长度为step_size的有序的子序列合并
template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size) {
  _Distance __two_step = 2 * __step_size;

  while (__last - __first >= __two_step) {
    __result = merge(__first, __first + __step_size,
                     __first + __step_size, __first + __two_step,
                     __result);
    __first += __two_step;
  }
  // 最后不整除的部分（主要是针对[first+step_size,last)这个区间）
  __step_size = min(_Distance(__last - __first), __step_size);
  merge(__first, __first + __step_size, __first + __step_size, __last,
        __result);
}
// 同上
template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance, class _Compare>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size,
                       _Compare __comp) {
  _Distance __two_step = 2 * __step_size;

  while (__last - __first >= __two_step) {
    __result = merge(__first, __first + __step_size,
                     __first + __step_size, __first + __two_step,
                     __result,
                     __comp);
    __first += __two_step;
  }
  __step_size = min(_Distance(__last - __first), __step_size);

  merge(__first, __first + __step_size,
        __first + __step_size, __last,
        __result,
        __comp);
}

const int __stl_chunk_size = 7;
// 分段插入排序
template <class _RandomAccessIter, class _Distance>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Distance __chunk_size)
{
  while (__last - __first >= __chunk_size) {
    __insertion_sort(__first, __first + __chunk_size); // 分段插入排序
    __first += __chunk_size;
  }
  __insertion_sort(__first, __last); // 对整个区间整体使用插入排序
}
// 同上
template <class _RandomAccessIter, class _Distance, class _Compare>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last,
                            _Distance __chunk_size, _Compare __comp)
{
  while (__last - __first >= __chunk_size) {
    __insertion_sort(__first, __first + __chunk_size, __comp); // 对每个子区间进行插入排序
    __first += __chunk_size; // 区间的起始位置在变
  }
  __insertion_sort(__first, __last, __comp); // 最后一个子区间进行插入排序
}
// 借助buffer的有序子序列合并
template <class _RandomAccessIter, class _Pointer, class _Distance>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last,
                              _Pointer __buffer, _Distance*) {
  _Distance __len = __last - __first;
  _Pointer __buffer_last = __buffer + __len;

  _Distance __step_size = __stl_chunk_size;
  __chunk_insertion_sort(__first, __last, __step_size); // 分段插入排序
  // 有序子序列合并，每循环一次子序列长度变2倍
  while (__step_size < __len) {
    __merge_sort_loop(__first, __last, __buffer, __step_size);
    __step_size *= 2;
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size);
    __step_size *= 2;
  }
}
// 同上+compare
template <class _RandomAccessIter, class _Pointer, class _Distance,
          class _Compare>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last, _Pointer __buffer,
                              _Distance*, _Compare __comp) {
  _Distance __len = __last - __first;
  _Pointer __buffer_last = __buffer + __len;

  _Distance __step_size = __stl_chunk_size;
  __chunk_insertion_sort(__first, __last, __step_size, __comp);

  while (__step_size < __len) {
    __merge_sort_loop(__first, __last, __buffer, __step_size, __comp);
    __step_size *= 2;
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size, __comp);
    __step_size *= 2;
  }
}
// stable and adaptive
template <class _RandomAccessIter, class _Pointer, class _Distance>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) { // 递归调用，将区间平均切分为两份
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size);
  }
  else {
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 
                   _Distance(__last - __middle), __buffer, __buffer_size); // 区间合并
}
// 同上+compare
template <class _RandomAccessIter, class _Pointer, class _Distance, 
          class _Compare>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size, _Compare __comp) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) {
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size, 
                           __comp);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size, 
                           __comp);
  }
  else {
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0,
                               __comp);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0,
                               __comp);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 
                   _Distance(__last - __middle), __buffer, __buffer_size,
                   __comp);
}
// stable_sort辅助函数
template <class _RandomAccessIter, class _Tp, class _Distance>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last); // 申请辅助空间
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()));
}
// 同上+compare
template <class _RandomAccessIter, class _Tp, class _Distance, class _Compare>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*,
                              _Compare __comp) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last);
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last, __comp);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()),
                           __comp);
}
// 稳定排序
template <class _RandomAccessIter>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first));
}
// 同上
template <class _RandomAccessIter, class _Compare>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first), 
                    __comp);
}

// partial_sort, partial_sort_copy, and auxiliary functions.
// 将[first,middle)部分进行对排序（这个函数还是没看懂%>_<%）
template <class _RandomAccessIter, class _Tp>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*) {
  make_heap(__first, __middle); // 前半部分建堆，大根堆，父节点大于子女节点
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i) // 后半部分
    if (*__i < *__first) // 将堆顶元素pop出来并存入i所在位置，而将*i的值push到堆中并adjust
      __pop_heap(__first, __middle, __i, _Tp(*__i), // 最后的效果是，比较大的元素都调整到middle之后了
                 __DISTANCE_TYPE(__first)); // 而比较小的元素仍然留在堆中
  sort_heap(__first, __middle); // 前半部分较小的元素进行heap sort
}
// 对外接口
template <class _RandomAccessIter>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first));
}
// 同上上+compare
template <class _RandomAccessIter, class _Tp, class _Compare>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*, _Compare __comp) {
  make_heap(__first, __middle, __comp);
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i)
    if (__comp(*__i, *__first))
      __pop_heap(__first, __middle, __i, _Tp(*__i), __comp,
                 __DISTANCE_TYPE(__first));
  sort_heap(__first, __middle, __comp);
}
// 同上上
template <class _RandomAccessIter, class _Compare>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, 
      typename iterator_traits<_RandomAccessIter>::value_type,
      typename iterator_traits<_RandomAccessIter>::value_type);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first), __comp);
}
// 非质变版本，先进行copy，剩下的和质变版本一样
template <class _InputIter, class _RandomAccessIter, class _Distance,
          class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                      _InputIter __last,
                                      _RandomAccessIter __result_first,
                                      _RandomAccessIter __result_last, 
                                      _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;
  _RandomAccessIter __result_real_last = __result_first;
  while(__first != __last && __result_real_last != __result_last) { // 进行拷贝
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last);
  while (__first != __last) {
    if (*__first < *__result_first) 
      __adjust_heap(__result_first, _Distance(0),
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first));
    ++__first;
  }
  sort_heap(__result_first, __result_real_last);
  return __result_real_last;
}
// 对外接口
template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _LessThanComparable);
  return __partial_sort_copy(__first, __last, __result_first, __result_last, 
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}
// 同上上+compare
template <class _InputIter, class _RandomAccessIter, class _Compare,
          class _Distance, class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                         _InputIter __last,
                                         _RandomAccessIter __result_first,
                                         _RandomAccessIter __result_last,
                                         _Compare __comp, _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;
  _RandomAccessIter __result_real_last = __result_first;
  while(__first != __last && __result_real_last != __result_last) {
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last, __comp);
  while (__first != __last) {
    if (__comp(*__first, *__result_first))
      __adjust_heap(__result_first, _Distance(0),
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first),
                    __comp);
    ++__first;
  }
  sort_heap(__result_first, __result_real_last, __comp);
  return __result_real_last;
}
// 同上，对外接口
template <class _InputIter, class _RandomAccessIter, class _Compare>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last, _Compare __comp) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);  
  return __partial_sort_copy(__first, __last, __result_first, __result_last,
                             __comp,
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}

// nth_element() and its auxiliary functions.  
// 第n大数，使用partition算法缩小搜索范围直至找到
template <class _RandomAccessIter, class _Tp>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*) {
  while (__last - __first > 3) { // 不断的进行partition
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1))));
    if (__cut <= __nth)
      __first = __cut;
    else 
      __last = __cut;
  }
  __insertion_sort(__first, __last); // 在不到3个元素时，进行排序，结果第n大元素位于第n个位置
}
// 同上，对外接口
template <class _RandomAccessIter>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first));
}
// 同上上+compare
template <class _RandomAccessIter, class _Tp, class _Compare>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*, _Compare __comp) {
  while (__last - __first > 3) {
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2), 
                                         *(__last - 1),
                                         __comp)),
                            __comp);
    if (__cut <= __nth)
      __first = __cut;
    else 
      __last = __cut;
  }
  __insertion_sort(__first, __last, __comp);
}
// 同上上+compare
template <class _RandomAccessIter, class _Compare>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first), __comp);
}


// Binary search (lower_bound, upper_bound, equal_range, binary_search).
// 二分查找val，存在则返回对应的迭代器，否则返回最小的不小于val的元素的迭代器
template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*) 
{
  _Distance __len = 0;
  distance(__first, __last, __len); // 获取区间长度
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1; // 区间长度减半
    __middle = __first;
    advance(__middle, __half);
    if (*__middle < __val) { // middle后半区间找
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else // middle前半区间找，*middle=val的在这部分
      __len = __half;
  }
  return __first;
}
// 同上，对外接口
template <class _ForwardIter, class _Tp>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
				const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __lower_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// 同上上+compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                              const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else
      __len = __half;
  }
  return __first;
}
// 同上上+compare
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __lower_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
// 二分查找val，存在则返回该元素的下一个元素的迭代器，否则返回最小的不小于val的元素的迭代器
template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__val < *__middle) // 在middle的前半部分找
      __len = __half;
    else { // 在middle的后半部分找，val=*middle的在部分
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}
// 对外接口
template <class _ForwardIter, class _Tp>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __upper_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// 同上上+compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(__val, *__middle))
      __len = __half;
    else {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}
// 对外接口+compare
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __upper_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
// 仍然是二分查找，返回值为val可插入的位置的区间
template <class _ForwardIter, class _Tp, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {
    __half = __len >> 1; // 二分
    __middle = __first;
    advance(__middle, __half);
    if (*__middle < __val) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__val < *__middle)
      __len = __half;
    else { // 剩下区间内的值均为val
      __left = lower_bound(__first, __middle, __val); // 下界
      advance(__first, __len);
      __right = upper_bound(++__middle, __first, __val); // 上界
      return pair<_ForwardIter, _ForwardIter>(__left, __right);
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}
// 对外接口
template <class _ForwardIter, class _Tp>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __equal_range(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// 同上上+compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__comp(__val, *__middle))
      __len = __half;
    else {
      __left = lower_bound(__first, __middle, __val, __comp);
      advance(__first, __len);
      __right = upper_bound(++__middle, __first, __val, __comp);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}           
// 同上上+compare
template <class _ForwardIter, class _Tp, class _Compare>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
            _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __equal_range(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
} 
// 二分查找，找到返回true，否则返回false
template <class _ForwardIter, class _Tp>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  _ForwardIter __i = lower_bound(__first, __last, __val);
  return __i != __last && !(__val < *__i);
}
// 同上+compare
template <class _ForwardIter, class _Tp, class _Compare>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val,
                   _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  _ForwardIter __i = lower_bound(__first, __last, __val, __comp);
  return __i != __last && !__comp(__val, *__i);
}

// merge, with and without an explicitly supplied comparison function.
// 两个有序序列合并为一个有序序列
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) { // 两个子序列都还没结束
    if (*__first2 < *__first1) { // 序列2的首元素较小
      *__result = *__first2;
      ++__first2;
    }
    else { // 序列1的首元素较小
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result)); // 两个子序列中有一个结束，将为结束序列的剩余部分拷贝到result尾部
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter1>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}

// inplace_merge and its auxiliary functions. 
// 将一个序列的两半有序子序列进行合并
template <class _BidirectionalIter, class _Distance>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) { // 仅包含两个数
    if (*__middle < *__first)
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) { // 第一个序列较长，等分成两部分
    __len11 = __len1 / 2;
    advance(__first_cut, __len11); // 获取第len11个元素的迭代器first_cut
    __second_cut = lower_bound(__middle, __last, *__first_cut); // 在后半序列中找*first_cut应插入的下界迭代器，作为后半部分的切割点
    distance(__middle, __second_cut, __len22); // 计算切割点second_cut到起点middle的距离
  }
  else { // 第二个序列较长，将其等分为两部分
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut); // 原地进行旋转
  __merge_without_buffer(__first, __first_cut, __new_middle,
                         __len11, __len22);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22);
}
// 同上+compare
template <class _BidirectionalIter, class _Distance, class _Compare>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2,
                            _Compare __comp) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (__comp(*__middle, *__first))
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle, __len11, __len22,
                         __comp);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22, __comp);
}
// 旋转，借助buffer，三次copy就ok了
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _Distance>
_BidirectionalIter1 __rotate_adaptive(_BidirectionalIter1 __first,
                                      _BidirectionalIter1 __middle,
                                      _BidirectionalIter1 __last,
                                      _Distance __len1, _Distance __len2,
                                      _BidirectionalIter2 __buffer,
                                      _Distance __buffer_size) {
  _BidirectionalIter2 __buffer_end;
  if (__len1 > __len2 && __len2 <= __buffer_size) {
    __buffer_end = copy(__middle, __last, __buffer);
    copy_backward(__first, __middle, __last);
    return copy(__buffer, __buffer_end, __first);
  }
  else if (__len1 <= __buffer_size) {
    __buffer_end = copy(__first, __middle, __buffer);
    copy(__middle, __last, __first);
    return copy_backward(__buffer, __buffer_end, __last);
  }
  else
    return rotate(__first, __middle, __last);
}
// 从后往前合并
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (*__last2 < *__last1) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}
// 同上+compare
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3, class _Compare>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result,
                                     _Compare __comp) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (__comp(*__last2, *__last1)) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}
// 使用buffer进行merge的算法，在buffer不足时与without_buffer的算法是类似的
template <class _BidirectionalIter, class _Distance, class _Pointer>
void __merge_adaptive(_BidirectionalIter __first,
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
    _Pointer __buffer_end = copy(__first, __middle, __buffer); // 将前半部分拷贝到buffer中
    merge(__buffer, __buffer_end, __middle, __last, __first); // 直接使用merge算法
  }
  else if (__len2 <= __buffer_size) {
    _Pointer __buffer_end = copy(__middle, __last, __buffer); // 将后半部分拷贝到buffer中
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last); // 从后往前拷贝
  }
  else { // buffer_size < len1 && buffer_size < len2，buffer不够用
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) { // 第一个序列较长，将其等分成两个部分
      __len11 = __len1 / 2;
      advance(__first_cut, __len11); // 取得len11处的迭代器first_cut
      __second_cut = lower_bound(__middle, __last, *__first_cut); // 在后半序列中找*first_cut应插入的下界迭代器，作为后半部分的切割点
      distance(__middle, __second_cut, __len22); // 计算切割点second_cut到起点middle的距离
    }
    else { // 第二个序列较长，将其等分为两部分
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle = // 借助buffer进行旋转
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11, // 将first_cut到middle的数旋转到middle之后
                        __len22, __buffer, __buffer_size); // 同时middle到second_cut的数旋转到middle之前
    __merge_adaptive(__first, __first_cut, __new_middle, __len11, // 将值小于first_cut的两个有序子序列合并
                     __len22, __buffer, __buffer_size);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11, // 将值大于first_cut的两个有序子序列合并
                     __len2 - __len22, __buffer, __buffer_size);
  }
}
// 同上+compare
template <class _BidirectionalIter, class _Distance, class _Pointer,
          class _Compare>
void __merge_adaptive(_BidirectionalIter __first, 
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size,
                      _Compare __comp) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
    merge(__buffer, __buffer_end, __middle, __last, __first, __comp);
  }
  else if (__len2 <= __buffer_size) {
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last,
                     __comp);
  }
  else {
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {
      __len11 = __len1 / 2;
      advance(__first_cut, __len11);
      __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
      distance(__middle, __second_cut, __len22);   
    }
    else {
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size, __comp);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size, __comp);
  }
}
// 原地合并辅助函数
template <class _BidirectionalIter, class _Tp, class _Distance>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1); // len1=middle-first
  _Distance __len2 = 0;
  distance(__middle, __last, __len2); // len2=last-middle

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last); // 申请辅助内存
  if (__buf.begin() == 0) // 内存申请失败，进行原地合并
    __merge_without_buffer(__first, __middle, __last, __len1, __len2);
  else // 内存分配成功，借助buffer进行合并
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()));
}
// 同上+compare
template <class _BidirectionalIter, class _Tp, 
          class _Distance, class _Compare>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*,
                                _Compare __comp) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)
    __merge_without_buffer(__first, __middle, __last, __len1, __len2, __comp);
  else
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()),
                     __comp);
}
// 原地进行合并
template <class _BidirectionalIter>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __middle || __middle == __last) // 两个子序列中有一个为空
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}
// 同上+compare
template <class _BidirectionalIter, class _Compare>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last, _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
           typename iterator_traits<_BidirectionalIter>::value_type,
           typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __middle || __middle == __last)
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first),
                      __comp);
}

// Set algorithms: includes, set_union, set_intersection, set_difference,
// set_symmetric_difference.  All of these algorithms have the precondition
// that their input ranges are sorted and the postcondition that their output
// ranges are sorted.
// 包含
template <class _InputIter1, class _InputIter2>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first2 < *__first1)
      return false; // 出现了2中存在而1中不存在的元素
    else if(*__first1 < *__first2) 
      ++__first1;
    else // 同时在两个集合中出现
      ++__first1, ++__first2;

  return __first2 == __last2;
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _Compare>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1))
      return false;
    else if(__comp(*__first1, *__first2)) 
      ++__first1;
    else
      ++__first1, ++__first2;

  return __first2 == __last2;
}
// 集合的并运算
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) {
    if (*__first1 < *__first2) {
      *__result = *__first1;
      ++__first1;
    }
    else if (*__first2 < *__first1) {
      *__result = *__first2;
      ++__first2;
    }
    else { // 两个集合中的元素相同
      *__result = *__first1;
      ++__first1;
      ++__first2;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result)); // 剩余部分直接copy
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
// 集合的交运算
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) 
    if (*__first1 < *__first2) 
      ++__first1;
    else if (*__first2 < *__first1) 
      ++__first2;
    else { // 在两个集合中都存在的元素才保留
      *__result = *__first1;
      ++__first1;
      ++__first2;
      ++__result;
    }
  return __result;
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2))
      ++__first1;
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
      ++__result;
    }
  return __result;
}
// 集合的差运算
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2,
                           _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first1 < *__first2) { // 集合1中存在而集合2中不存在的元素保留
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (*__first2 < *__first1)
      ++__first2;
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first1, __last1, __result);
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _OutputIter, 
          class _Compare>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2, 
                           _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first1, __last1, __result);
}
// 集合的对称差运算
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first1 < *__first2) { // 集合1中存在而2中不存在的元素
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (*__first2 < *__first1) { // 集合2中存在而集合1中不存在的元素
      *__result = *__first2;
      ++__first2;
      ++__result;
    }
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
// 同上+compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result,
                         _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
      ++__result;
    }
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}

// min_element and max_element, with and without an explicitly supplied
// comparison function.
// 求最大值
template <class _ForwardIter>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (*__result < *__first)
      __result = __first; // 保存最小值的迭代器
  return __result;
}
// 同上+compare
template <class _ForwardIter, class _Compare>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__result, *__first)) __result = __first;
  return __result;
}
// 求最小值
template <class _ForwardIter>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (*__first < *__result)
      __result = __first; // 保存最大值的迭代器
  return __result;
}
// 同上+compare
template <class _ForwardIter, class _Compare>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__first, *__result))
      __result = __first;
  return __result;
}

// next_permutation and prev_permutation, with and without an explicitly 
// supplied comparison function.
// 排列中当前序列的下一个序列，具体见 http://ibillxia.github.io/blog/2014/04/24/next-permutation-and-analysis-of-its-stl-implementation/
template <class _BidirectionalIter>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (*__i < *__ii) {
      _BidirectionalIter __j = __last;
      while (!(*__i < *--__j))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
// 同上+compare
template <class _BidirectionalIter, class _Compare>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__i, *__ii)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*__i, *--__j))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
// 全排列当前序列的上一个排列
template <class _BidirectionalIter>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (*__ii < *__i) {
      _BidirectionalIter __j = __last;
      while (!(*--__j < *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
// 同上+compare
template <class _BidirectionalIter, class _Compare>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__ii, *__i)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*--__j, *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}

// find_first_of, with and without an explicitly supplied comparison function.
// 在第一个序列中依次查找第二个序列中某个元素第一次出现的位置
template <class _InputIter, class _ForwardIter>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);
  // 双重循环
  for ( ; __first1 != __last1; ++__first1) // 外循环遍历第一个序列
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter) // 内循环遍历第二个序列
      if (*__first1 == *__iter)
        return __first1;
  return __last1;
}
// 同上+compare
template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2,
                         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) 
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (__comp(*__first1, *__iter))
        return __first1;
  return __last1;
}


// find_end, with and without an explicitly supplied comparison function.
// Search [first2, last2) as a subsequence in [first1, last1), and return
// the *last* possible match.  Note that find_end for bidirectional iterators
// is much faster than for forward iterators.
// 在序列1中查找是否存在序列2这样的子序列，返回最后一次查找结果
// find_end for forward iterators. 
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag)
{
  if (__first2 == __last2) // 待查找区间为空
    return __last1;
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
      _ForwardIter1 __new_result
        = search(__first1, __last1, __first2, __last2); // 在序列1中查找是否存在序列2这样的子序列，line 314
      if (__new_result == __last1) // 查找结束
        return __result;
      else { // 保存当前查找结果
        __result = __new_result;
        __first1 = __new_result;
        ++__first1;
      }
    }
  }
}
// 同上+compare
template <class _ForwardIter1, class _ForwardIter2,
          class _BinaryPredicate>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag,
                         _BinaryPredicate __comp)
{
  if (__first2 == __last2)
    return __last1;
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
      _ForwardIter1 __new_result
        = search(__first1, __last1, __first2, __last2, __comp);
      if (__new_result == __last1)
        return __result;
      else {
        __result = __new_result;
        __first1 = __new_result;
        ++__first1;
      }
    }
  }
}

// find_end for bidirectional iterators.  Requires partial specialization.
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION // 具有类偏特化的版本
// 输入区间的迭代器为双向迭代器
template <class _BidirectionalIter1, class _BidirectionalIter2>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));
    return __result;
  }
}
// 同上+compare
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BinaryPredicate>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag, 
           _BinaryPredicate __comp)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2,
                               __comp);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));
    return __result;
  }
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Dispatching functions for find_end.
// 对外接口，在序列1中查找是否存在序列2这样的子序列，返回最后一次查找结果
template <class _ForwardIter1, class _ForwardIter2>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  return __find_end(__first1, __last1, __first2, __last2, // 可能会调用到特化的版本
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2));
}
// 同上+compare
template <class _ForwardIter1, class _ForwardIter2, 
          class _BinaryPredicate>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2,
         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2),
                    __comp);
}

// is_heap, a predicate testing whether or not a range is
// a heap.  This function is an extension, not part of the C++
// standard.
// 判断一个序列是否为堆
template <class _RandomAccessIter, class _Distance>
bool __is_heap(_RandomAccessIter __first, _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__first[__parent] < __first[__child]) // 判断父节点是否大于其孩子节点
      return false;
    if ((__child & 1) == 0)
      ++__parent;
  }
  return true;
}
// 同上+compare
template <class _RandomAccessIter, class _Distance, class _StrictWeakOrdering>
bool __is_heap(_RandomAccessIter __first, _StrictWeakOrdering __comp,
               _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__comp(__first[__parent], __first[__child]))
      return false;
    if ((__child & 1) == 0)
      ++__parent;
  }
  return true;
}
// 对外接口
template <class _RandomAccessIter>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  return __is_heap(__first, __last - __first);
}

// 同上+compare
template <class _RandomAccessIter, class _StrictWeakOrdering>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last,
                    _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
         typename iterator_traits<_RandomAccessIter>::value_type, 
         typename iterator_traits<_RandomAccessIter>::value_type);
  return __is_heap(__first, __comp, __last - __first);
}

// is_sorted, a predicated testing whether a range is sorted in
// nondescending order.  This is an extension, not part of the C++
// standard.
// 判断序列是否是有序的
template <class _ForwardIter>
bool is_sorted(_ForwardIter __first, _ForwardIter __last)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (*__next < *__first) // 判断前后相邻的两个元素是否有序
      return false;
  }

  return true;
}
// 同上+compare
template <class _ForwardIter, class _StrictWeakOrdering>
bool is_sorted(_ForwardIter __first, _ForwardIter __last,
               _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
        typename iterator_traits<_ForwardIter>::value_type,
        typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (__comp(*__next, *__first))
      return false;
  }

  return true;
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1209
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_ALGO_H */

// Local Variables:
// mode:C++
// End:
