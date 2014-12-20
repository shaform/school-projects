template<typename T>
class vector {
    public:
        // types
        typedef size_t size_type;
        typedef T& reference;
        typedef const T& const_reference;
        // ctor/copy ctor/dtor
        vector();
        vector(size_type,const T& =T()); 
        vector(const vector<T>&); 
        ~vector();
        // capacity
        size_type size() const;
        size_type capacity() const;
        bool empty() const;
        // element access
        reference operator[](size_type n); 
        const_reference operator[](size_type n) const;
    private:
        T *start, *finish, *end_storage;
};

template<typename T>
vector<T>::vector() : 
    start(0), finish(0), end_storage(0)
{
}
template<typename T>
vector<T>::vector(size_type n, const T&val) :
    start(static_cast<T*>(operator new[](sizeof(T)*n))), finish(start+n), end_storage(finish)
{
    for (T *i = start; i != finish; ++i)
        new (i) T(val);
}
template<typename T>
vector<T>::vector(const vector<T>&rhs) :
    start(static_cast<T*>(operator new[](sizeof(T)*rhs.capacity()))), finish(start+rhs.size()), end_storage(start+rhs.capacity())
{
    for (T *i = start; i != finish; ++i)
        new (i) T(rhs[i-start]);
}
    template<typename T>
vector<T>::~vector()
{
    for (T *i=start; i != finish; ++i)
        i->~T();

    if (capacity())
        operator delete[] (start);
    start = finish = end_storage = 0;

}
template<typename T>
typename vector<T>::size_type vector<T>::size() const
{
    return finish - start;
}

template<typename T>
typename vector<T>::size_type vector<T>::capacity() const
{
    return end_storage - start;
}

template<typename T>
bool vector<T>::empty() const
{
    return start == finish;
}

    template<typename T>
typename vector<T>::reference vector<T>::operator[](size_type n)
{
    return start[n];
}

template<typename T>
typename vector<T>::const_reference vector<T>::operator[](size_type n) const
{
    return start[n];
}
