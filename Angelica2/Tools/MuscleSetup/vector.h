  /*
 * FILE: vector.h
 *
 * DESCRIPTION: vector
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/
#ifndef __ABASE_VECTOR_H__
#define __ABASE_VECTOR_H__
#pragma once

#include <assert.h>
#include <stdlib.h>
#include <new>
#pragma warning(disable: 4786) // identifier was truncated in the debug information

namespace abase
{

template<class T>
inline void swap(T & lhs,T & rhs)
{
	T tmp;
	tmp = lhs;
	lhs = rhs;
	rhs = tmp;
}

template <class T> 
class vector
{
protected:
	inline T * _M_allocate(size_t __n) { return (T*)malloc(__n * sizeof(T));}
	inline void _M_deallocate(T* __p, size_t __n){free(__p);}
	inline T *_M_allocate_and_copy(size_t __n, T *start, T * finish){
		T * tmp = _M_allocate(__n);
		T * tmp2;
		tmp2 = tmp;
		while(start != finish && (__n--)>0)
		{
			new ((void *)&*tmp++) T(*start++);
		}
		return tmp;
	}
protected:
	T * _data;
	T * _finish;
	size_t _max_size;
	size_t _cur_size;
public:
	explicit vector(int n=10);
	vector(int n, const T & val);
	vector(const vector<T> & rhs);
	~vector();
	const vector<T> &operator=(const vector<T> & rhs);

public:
	inline const T & operator [](size_t pos) const {assert(pos >=0 && pos < _cur_size);return _data[pos];}
	inline T & operator [](size_t pos) {assert(pos >=0 && pos < _cur_size);return _data[pos];}
	inline const T & at(size_t pos) const {assert(pos >=0 && pos < _cur_size); return _data[pos];}
	inline T & at(size_t pos) {assert(pos >=0 && pos < _cur_size); return _data[pos];}
	inline size_t capacity() const {return _max_size;}
	inline size_t max_size() const{ return size_t(-1) / sizeof(T); }
	inline size_t size() const { return _cur_size;}
	inline T & front() { return *begin(); }
	inline const T &front() const { return *begin(); }
	inline T & back(){return *(end()-1);}
	inline const T & back() const {return *(end()-1);}
	inline bool empty() const {return size() == 0;}
	inline const T * begin() const {return _data;}
	inline T * begin() {return _data;}
	inline const T * end() const {return _data + _cur_size;}
	inline T * end() { return _data + _cur_size;}
	inline void clear(){ erase(begin(),end());}

	void reserve(size_t __n) {
		if (capacity() < __n) {
			const size_t __old_size = size();
			T * __tmp = _M_allocate_and_copy(__n, _data, _finish);
			for(T * fp = _data;fp < _finish;fp ++)
				fp->~T();
			_M_deallocate(_data, _max_size);
			_data		= __tmp;
			_finish		= __tmp + __old_size;
			_max_size	= __n;
		}
	}
	void push_back(const T & x);
	void pop_back() {_finish--;_cur_size --;_finish->~T();}
	T *insert(T * data,const T& x);
	void insert(T *it, size_t n, const T& x);
	void swap(vector<T> & x);
	void erase(T *obj);		// delete the obj
	void erase_noorder(T *obj);
	void erase(T *first, T * last);	// delete [first, last)

};

template <class T>
vector<T>::vector(int n)
{
	_max_size = n;
	_cur_size = 0;
	_data = _M_allocate(n);
	_finish = _data;
	assert(_data);
}
template <class T>
vector<T>::vector(int n, const T & val)
{
	_max_size	= n;
	_data		= _M_allocate(n);
	assert(_data);
	_finish		= _data;
	_cur_size	= n;
	while(n--) new ((void*)&*_finish++) T(val);
}

template<class T>
vector<T>::vector(const vector<T> & rhs)
{
	_max_size	= rhs._max_size;
	_cur_size	= rhs._cur_size;
	_data		= _M_allocate(_max_size);
	_finish		= _data;
	int n = _cur_size;
	const T * it = rhs.begin();
	while(n--) new((void*)&*_finish++) T(*it++);
}

template<class T>
const vector<T> & vector<T>::operator=(const vector<T> & rhs)
{
	if(this == &rhs) return rhs;
	vector<T> tmp(rhs);
	tmp.swap(*this);
	return *this;
}

template <class T>
vector<T>::~vector()
{	
	clear();
	_M_deallocate(_data,_max_size);
}

template <class T>
void vector<T>::push_back(const T & x)
{
	if(_cur_size == _max_size)
	{
		size_t newsize;
		size_t i;
		T * newdata,*tmp1,*tmp2;
		newsize = _cur_size;
		if(newsize == 0) 
			newsize = 1;
		else
			newsize += _cur_size > 2? _cur_size>>1 : _cur_size;
		newdata = _M_allocate(newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;
		for(i = 0; i < _cur_size;i++)
			new ((void *)&*tmp1++) T(*tmp2++);

		tmp2 = _data;
		for(i = 0; i < _cur_size; i++)
			tmp2++->~T();

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	
	new ((void *)&*(_finish)) T(x);
	_cur_size ++;
	_finish ++;
}

template <class T>
T *vector<T>::insert(T * data,const T& x)
{
	int idx;
	idx = data - _data;
	if(_cur_size == _max_size)
	{
		int newsize;
		T * newdata,*tmp1,*tmp2;
		newsize = _cur_size > 0?_cur_size * 2 : 1;
		newdata = _M_allocate(newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;

		while(tmp2 < data)
			new ((void *)&*tmp1++) T(*tmp2++);

		new ((void *)&*tmp1++) T(x);

		while(tmp2 < _finish)
			new ((void *)&*tmp1++) T(*tmp2++);

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	else
	{
		if(_finish != data)
		{
			new ((void*)_finish) T(*(_finish -1));
			for(T * tp = _finish - 1;tp > data;tp --)
				*tp = *(tp-1);
			*data = x;
		}
		else
		{
			new((void*)_finish) T(x);
		}

		
	}
	_cur_size ++;
	_finish ++;
	return _data + idx;
}

template <class T>
void vector<T>::insert(T *it, size_t n, const T& x)
{
	int idx;
	idx = it - _data;
	if(_cur_size + n > _max_size)
	{
		int newsize;
		T * newdata,*tmp1,*tmp2;
		newsize = (_cur_size + n) * 2; //$$
		newdata = _M_allocate(newsize);
		assert(newdata);
		tmp1 = newdata;
		tmp2 = _data;

		while(tmp2 < it)
			new ((void *)&*tmp1++) T(*tmp2++);

		for(int i = 0; i< n; i++) new ((void *)&*tmp1++) T(x);

		while(tmp2 < _finish)
			new ((void *)&*tmp1++) T(*tmp2++);

		_M_deallocate(_data,_max_size);
		_max_size = newsize;
		_data = newdata;
		_finish = _data + _cur_size;
	}
	else
	{
		for(T * tp = _finish - 1;tp >= it;tp --)
			*(tp + n) = *tp;
		for(T *tp2 = it; tp2 < it + n; tp2++)
			*tp2 = x;
	}
	_cur_size += n;
	_finish += n;
	return;
}

template <class T>
void vector<T>::erase(T *obj)
{
	if(_cur_size)
	{
		int idx = obj - _data;

		for(T * fp = obj;fp < _finish - 1;fp++)
		{
			*fp = *(fp+1);
		}
		_cur_size --;
		_finish --;
		_finish->~T();	//destroy the last
	}

}

template <class T>
void vector<T>::swap(vector<T> & __x)
{
    abase::swap(_data, __x._data);
    abase::swap(_finish, __x._finish);
    abase::swap(_cur_size, __x._cur_size);
    abase::swap(_max_size, __x._max_size);
}

template <class T>
void vector<T>::erase(T *first, T * last)
{
	T * fp;
	T * fp2;
	for(fp = last,fp2 = first; fp < _finish;fp++,fp2++)
	{
		*fp2 = *fp;
	}

	for(;fp2 < _finish; fp2++)
	{
		fp2->~T();
	}
	_finish   -= (last - first);
	_cur_size -= (last - first);
}
template <class T>
void vector<T>::erase_noorder(T *obj)
{
	int idx = obj - _data;
	assert(idx >=0 && idx < (int)_cur_size);
	_cur_size --;
	_finish --;
	if(_cur_size)
	{
		_data[idx] = *_finish;
	}
	_finish->~T();	//destroy the last
}


template <class T>
class vector_ptr
{
	vector<void *> __ivector;
public:
	vector_ptr(int n=10):__ivector(n){assert("vector_ptr需要一个指针类型" && (T)0  == (void*)0 );}
	vector_ptr(int n, const T & val):__ivector(n,val){assert("vector_ptr需要一个指针类型" && (T)0 == (void*)0 );}
	vector_ptr(const vector_ptr<T> & rhs):__ivector(rhs.__ivector){assert("vector_ptr需要一个指针类型" && (T)0  == (void*)0);}
	const vector_ptr<T> & operator=(const vector_ptr<T> & rhs)
	{
		__ivector = rhs.__ivector;
		return *this;
	}

public:
	
	inline const T & operator [](size_t pos) const {return (const T&)__ivector[pos];}
	inline T & operator [](size_t pos) {return (T&)__ivector[pos];}
	inline const T & at(size_t pos) const {return (const T &)__ivector.at(pos);}
	inline T & at(size_t pos) {return (T&)__ivector.at(pos);}
	inline size_t capacity() const {return __ivector.capacity();}
	inline size_t max_size() const{ return __ivector.max_size();}
	inline size_t size() const { return __ivector.size();}
	inline T & front() { return (T&)__ivector.front();}
	inline const T &front() const { return (const T &)__ivector.front();}
	inline T & back(){return (T&)__ivector.back();}
	inline const T & back() const {return (const T &)__ivector.back();}
	inline bool empty() const {return __ivector.empty();}
	inline const T * begin() const {return (const T*)__ivector.begin();}
	inline T * begin() {return (T*)__ivector.begin();}
	inline const T * end() const {return (const T*)__ivector.end();}
	inline T * end() { return (T*)__ivector.end();}
	inline void clear(){ __ivector.clear();}
	inline void reserve(size_t __n) {__ivector.reserve();}
	inline void push_back(const T & x) {__ivector.push_back(x);}
	inline void pop_back() {__ivector.pop_back();}
	inline T *insert(T * data,const T& x){return (T*)__ivector.insert((void**)data,x);}
	inline void insert(T *it, size_t n, const T& x){__ivector.insert((void**)it,n,x);}
	inline void swap(vector_ptr<T> & x) {__ivector.swap(x.__ivector);}
	inline void erase(T *obj) {__ivector.erase((void**)obj);}
	inline void erase_noorder(T *obj) {__ivector.erase_noorder((void**)obj);}
	inline void erase(T *first, T * last) {__ivector.erase((void**)first,(void**)last);}
};

template<class T>
inline void clear_vector_ptr(vector_ptr<T> & vec)
{
	int i;
	for(i = 0; i < vec.size(); i++)
	{
		delete vec[i];
	}
	vec.clear();
}

}
#endif
