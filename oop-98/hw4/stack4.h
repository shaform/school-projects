template<typename T>
class stack {
    public:
        typedef size_t size_type;
        typedef T value_type;
        stack(size_type=2);
        ~stack();
        void push(const value_type&);
        void pop();
        value_type& top();
        const value_type& top() const;
        size_type size() const;
        bool empty() const;

    private:
        struct node;
        node* head;
        int _top;
        const size_type sz;
        size_type _size;
};

template<typename T>
struct stack<T>::node {
    T* stk;
    node* succ;
    node(T* m_stk, node* m_succ) : stk(m_stk), succ(m_succ) {} 
};



    template<typename T>
    stack<T>::stack(size_type size)
: sz(size), _top(-1), head(0), _size(0)
{
}

    template<typename T>
stack<T>::~stack()
{
    while (!empty()) pop();
}
    template<typename T>
void stack<T>::push(const value_type& val)
{
    if (_top == sz-1 || _size == 0) {
        node* h = new node(static_cast<T*>(operator new[](sizeof(value_type)*sz)), head);
        head = h;

        _top = -1;
        ++_size;
    }

    new (head->stk + (++_top)) T(val);
}

    template<typename T>
void stack<T>::pop()
{
    if (_top >= 0) {
        head->stk[_top].~value_type();
        --_top;


        if (_top == -1) {
            node *t = head;
            head = head->succ;

            operator delete[](t->stk);
            delete t;
            --_size;

            if (head)
                _top = sz-1;
        }
    }
}

    template<typename T>
typename stack<T>::value_type& stack<T>::top()
{
    static value_type temp;
    if (_top>=0)
        return head->stk[_top];
    else
        return temp;
}

template<typename T>
const typename stack<T>::value_type& stack<T>::top() const
{
    static const value_type temp;
    if (_top>=0)
        return head->stk[_top];
    else
        return temp;
}

template<typename T>
typename stack<T>::size_type stack<T>::size() const
{
    return sz*_size + _top + 1;
}

template<typename T>
bool stack<T>::empty() const
{
    return _top==-1;
}
