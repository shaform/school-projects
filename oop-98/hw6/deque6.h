template<typename T>
class deque { 
    public:
        // types
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef T value_type;

        // ctor/copy ctor/dtor
        deque();
        deque(const deque&);
        ~deque();

        // capacity
        size_type size() const;
        bool empty() const;

        // modifiers 
        void push_front(const T&); 
        void push_back(const T&);
        void pop_front(); 
        void pop_back();

        // element access
        reference front(); 
        const_reference front() const; 
        reference back(); 
        const_reference back() const;

    private:
        struct node;
        node *head;
        size_type _size;
};

template<typename T>
deque<T>::deque() :
    head(static_cast<node*>(operator new(sizeof(node)))) , _size(0)
{
    head->succ = head->pred = head;
}
template<typename T>
deque<T>::deque(const deque<T> &rhs) :
    head(static_cast<node*>(operator new(sizeof(node)))) , _size(0)
{
    head->succ = head->pred = head;

    node *it = rhs.head->succ;
    while (it != rhs.head) {
        push_back(it->datum);
        it = it->succ;
    }
}
    template<typename T>
deque<T>::~deque()
{
    while (!empty())
        pop_front();
    operator delete(head);
}

template<typename T>
typename deque<T>::size_type deque<T>::size() const
{
    return _size;
}
template<typename T>
bool deque<T>::empty() const
{
    return _size == 0;
}

    template<typename T>
void deque<T>::push_front(const T&val) 
{
    head->succ = head->succ->pred = new node(val, head, head->succ);
    ++_size;
}
    template<typename T>
void deque<T>::push_back(const T&val)
{
    head->pred = head->pred->succ = new node(val, head->pred, head);
    ++_size;
}
    template<typename T>
void deque<T>::pop_front() 
{
    node *t = head->succ;
    if (t != head) {
        head->succ = t->succ;
        head->succ->pred = head;
        delete t;
        --_size;
    }
}
    template<typename T>
void deque<T>::pop_back()
{
    node *t = head->pred;
    if (t != head) {
        head->pred = t->pred;
        head->pred->succ = head;
        delete t;
        --_size;
    }
}

    template<typename T>
typename deque<T>::reference deque<T>::front()
{
    return head->succ->datum;
}

template<typename T>
typename deque<T>::const_reference deque<T>::front() const
{
    return head->succ->datum;
}

    template<typename T>
typename deque<T>::reference deque<T>::back()
{
    return head->pred->datum;
}

template<typename T>
typename deque<T>::const_reference deque<T>::back() const
{
    return head->pred->datum;
}

template<typename T>
struct deque<T>::node {
    node(const T&,node*,node*);
    T datum;
    node *pred,*succ;
};

template<typename T>
deque<T>::node::node(const T&d, node *p, node *s) :
    datum(d), pred(p), succ(s)
{
}
