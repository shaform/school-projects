template<typename T>
class stack {
    public:
        typedef size_t size_type;
        typedef T value_type;
        stack();
        ~stack() { while (!empty()) pop(); operator delete[](stk); }
        void push(const value_type&);
        void pop();
        value_type& top() { return stk[_top]; }
        const value_type& top() const { return stk[_top]; }
        size_type size() const { return _top+1; }
        bool empty() const { return size()==0; }
    private:
        T* stk;
        size_type _top,_capacity;
};

    template<typename T>
    stack<T>::stack()
: stk(0), _top(-1), _capacity(0)
{
}

    template<typename T>
void stack<T>::push(const value_type &val)
{
    if (_capacity==0) {
        stk = static_cast<T*>(operator new[](sizeof(T)));
        _capacity = 1;

    } else if (_top+1==_capacity) {
        _capacity*=2;
        T* new_stk = static_cast<T*>(operator new[](sizeof(T)*_capacity));

        for (int i=0; i<=_top; ++i)
            new (new_stk+i) T(stk[i]);


        int new_top = _top;
        while (!empty())
            pop();
        operator delete[](stk);
        stk = new_stk;
        _top = new_top;
    }

    new (stk+(++_top)) T(val);
}

    template<typename T>
void stack<T>::pop()
{
    if (!empty())
        stk[_top--].~T();
}
