#pragma once
#include "bureaucracy.h"
#include <initializer_list>

#define default_heap_size 100

/* comments:
	- bfs traversal
	- parent(index) == floor( (index - 1) / 2 )
	- height()      == floor( log2(n) )
	- property()    == it is never broken
*/

template <class T = int>
class maxheap
{
	// typedefs:
	typedef const T& type;
	typedef bool (*fct)(type, type);

	// iterator concept:
	class iterator
	{
		T* value;
	public:
		iterator(T* val);

		T		operator * () const;
		void	operator ++();
		bool	operator !=(const iterator& two) const;
	};

public:
	// constructors:
	~maxheap();
	maxheap(const size_t& n = default_heap_size);
	maxheap(const std::initializer_list<T>& val, const size_t& n = default_heap_size, bool (*f)(type, type) = nullptr);
	maxheap(T* val, const size_t& val_size, const size_t& n = default_heap_size, bool (*f)(type, type) = nullptr);
	maxheap(const maxheap<T>& h);
	maxheap(const maxheap<T>&& h);

	// modifier methods:
	maxheap<T>& operator = (const maxheap<T>& h);
	maxheap<T>& clear();
	maxheap<T>& set_f(fct f);
	
	// specific methods:
	void replce(const size_t& index, const T& value);
	void insert(const T& value);
	void extrct();
	
	// constant methods:
	bool operator == (const maxheap<T>& h) const;
	size_t getn() const;
	size_t getl() const;
	void   prnt() const;
	bool  empty() const;

	// query operations:
	bool search(const T& value) const;

	// iterator methods:
	iterator begin() const;
	iterator end() const;

	// friend functions:
	friend T* convert(const maxheap<T>& h);
	friend std::ostream& operator << (std::ostream& out, const maxheap<T>& h);

private:
	// data members:
	size_t n;
	size_t last;
	T* values;

	// auxiliar utility:
	fct  compare;
	void heapify(const size_t& nnn, const size_t& index); // we need f here
	void arrange();
};

//------------------------------------------------
// auxiliar utility:

template <class T>
void maxheap<T>::heapify(const size_t& nnn, const size_t& index)
{
	size_t index_largest = index, index_left = 2 * index + 1, index_rght = 2 * index + 2;
	if (index_left < nnn && f(values[index_left], values[index_largest]))
		index_largest = index_left;
	if (index_rght < nnn && f(values[index_rght], values[index_largest]))
		index_largest = index_rght;

	if (index != index_largest)
	{
		T temp = values[index];
		values[index] = values[index_largest];
		values[index_largest] = temp;
		heapify(nnn, index_largest);
	}
}

template <class T>
void maxheap<T>::arrange()
{
	for (int i = (last + 1) / 2 - 1; i >= 0; i--)
		heapify(last + 1, i);
}

//------------------------------------------------
// constructors:

template <class T>
maxheap<T>::~maxheap()
{
	delete[]values;
}

template <class T>
maxheap<T>::maxheap(const size_t& n)
{
	f = [](type x, type y) {return x > y; };
	this->n = n;
	last = -1;
	values = new T[n];
	FOR(n)
		values[i] = NULL;
}

template <class T>
maxheap<T>::maxheap(const std::initializer_list<T>& val, const size_t& n, bool (*f)(type, type))
{
	this->f = f;
	if (f == nullptr) this->f = [](type x, type y)->bool { return x > y; };

	this->n = n;
	last = val.size() - 1;
	if (last >= n)
		hard_error("wrong number of elements given");
	values = new T[n];

	size_t index = 0;
	for (auto i : val)
		values[index++] = i;
	for (; index < n; index++)
		values[index] = NULL;
	arrange();
}

template <class T>
maxheap<T>::maxheap(T* val, const size_t& val_size, const size_t& n, bool (*f)(type, type))
{
	this->f = f;
	if (f == nullptr) this->f = [](type x, type y)->bool {return x > y; };

	this->n = n;
	this->last = -1;

	size_t index = 0;
	while (index < val_size)
	{
		values[++last] = *val;
		val++;
		index++;
	}

	if (last >= n)
		hard_error("wrong number of elements given");
	for (size_t i = last + 1; i < n; i++)
		values[i] = NULL;
	arrange();
}

template <class T>
maxheap<T>::maxheap(const maxheap<T>& h)
{
	this->f = h.f;
	n = h.getn();
	last = h.getl();
	values = new T[n];

	size_t index = 0;
	for (auto i : h)
		values[index++] = i;
	for (; index < n; index++)
		values[index] = NULL;
}

template <class T>
maxheap<T>::maxheap(const maxheap<T>&& h)
{
	this->f = h.f;
	n = h.getn();
	last = h.getl();
	values = new T[n];

	size_t index = 0;
	for (auto i : h)
		values[index++] = i;
	for (; index < n; index++)
		values[index] = NULL;
	delete h;
}

//------------------------------------------------
// iterator methods:

template <class T>
maxheap<T>::iterator::iterator(T* val) : value(val) {};

template <class T>
T maxheap<T>::iterator::operator * () const
{
	return *value;
}

template <class T>
void maxheap<T>::iterator::operator ++()
{
	value++;
}

template <class T>
bool maxheap<T>::iterator::operator !=(const iterator& two) const
{
	return value != two.value;
}

template <class T>
typename maxheap<T>::iterator maxheap<T>::begin() const
{
	return iterator_maxheap(&values[0]);
}

template <class T>
typename maxheap<T>::iterator maxheap<T>::end() const
{
	return iterator_heap(&values[last + 1]);
}

//------------------------------------------------
// specific methods:

template <class T>
maxheap<T>& maxheap<T>::operator = (const maxheap<T>& h)
{
	this->f = h.f;
	this->n = h.n;
	this->last = -1;
	delete[]values;
	values = new T[n];
	for (auto i : h)
		this->values[++last] = i;
}

template <class T>
void maxheap<T>::replce(const size_t& index, const T& value)
{
	if (index >= n)
		hard_error("bad index");
	if (index > last)
	{
		insert(value);
		return;
	}

	values[index] = value;
}

template <class T>
void maxheap<T>::insert(const T& value)
{
	if (last + 1 >= n)
	{
		eazy_error("no more space");
		return;
	}

	values[++last] = value;
	T temp = values[last];
	values[last] = values[0];
	values[0] = temp;
	heapify(n, 0);
}

template <class T>
void maxheap<T>::extrct()
{
	T temp = values[last];
	values[last] = values[0];
	values[0] = temp;
	last--;
}

template <class T>
bool maxheap<T>::search(const T& value) const
{
	int nr_nodes = 1;
	size_t index = 0;
	while (value <= values[index] && index <= last)
	{
		index = index * 2 + 1;
		nr_node *= 2;
	}

	// 3 cases
	if (index > last)
		return false;
	if (value == values[index])
		return true;

	// current level
	for (size_t i = index; i < 2 * index + 1; i++)
		if (values[index] == value)
			return true;
	// one level above
	for (size_t i = (index - 1) / 2; i < index; i++)
		if (values[index] == value)
			return true;
	return false;
}

//------------------------------------------------
// constant methods:

template <class T>
bool maxheap<T>::operator == (const maxheap<T>& h) const
{
	if (this->n != h.n)
		return false;
	if (this->last != h.last)
		return false;
	size_t index = 0;
	for (auto i : h)
		if (values[index++] != i)
			return false;
	return true;
}

template <class T>
size_t maxheap<T>::getn() const
{
	return n;
}

template <class T>
size_t maxheap<T>::getl() const
{
	return l;
}

template <class T>
void maxheap<T>::prnt() const
{
	FOR(last + 1)
		std::cout << values[i] << ' ';
}

template <class T>
bool maxheap<T>::empty() const
{
	return l == -1;
}

//------------------------------------------------
// friend functions:

template <class T>
T* convert(const maxheap<T>& h)
{
	T* ptr = new T[h.last + 1];
	for (size_t i = 0; i < h.last + 1; i++)
		ptr[i] = h.values[i];
	return ptr;
}

template <class T>
std::ostream& operator << (std::ostream& out, const maxheap<T>& h)
{
	for (auto i : h)
		out << i << ' ';
	return out;
}

