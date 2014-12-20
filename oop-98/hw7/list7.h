template<class T>
class list {
    public:
        // types
        class iterator;
        class const_iterator;
        class const_iterator;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference; 
        // ctor/dtor
        list();
        list(const list<T>&);
        template<class InputIterator> list(InputIterator,InputIterator);
        ~list();
        // modifiers
        iterator insert(iterator,const T& =T());
        iterator erase(iterator);
        void push_front(const T& d) { insert(begin(),d); }
        void push_back(const T& d) { insert(end(),d); }
        void pop_front() { erase(begin()); }
        void pop_back() { erase(--end()); }
        // list operations
        void unique();
        // capacity
        bool empty() const { return begin()==end(); }
        // element access
        reference front() { return *begin(); }
        const_reference front() const { return *begin(); }
        reference back() { return *--end(); }
        const_reference back() const { return *--end(); }
        // iterator                         
        iterator begin() { return head->succ; }
        const_iterator begin() const { return head->succ; }
        iterator end() { return head; }
        const_iterator end() const { return head; }
    private:
        struct node;
        node* head;
};


template<class T>
struct list<T>::node
{
    T datum;
    node *pred,*succ;
    node(const T& d,node* p,node* s)
        : datum(d), pred(p), succ(s)
    {}
};


template<class T>
class list<T>::const_iterator {
    public:
        const_iterator(const node* =0);
        // const_iterator(iterator);               // conversion (1)
        const_reference operator*() const;
        const_pointer operator->() const;
        const_iterator& operator++();
        const const_iterator operator++(int);
        const_iterator& operator--();
        const const_iterator operator--(int);
        bool operator==(const_iterator) const;
        bool operator!=(const_iterator) const;
    private:
        const node* current;           
};



template<class T>
class list<T>::iterator {
    // friend class list<T>;                   // modifier (1)
    // friend clas const_iterator;            // conversion (1)
    public:
        operator node*() const;                 // modifier (2)
        operator const_iterator() const;       // conversion (2)
        iterator(node* =0);
        reference operator*() const;
        pointer operator->() const;
        iterator& operator++();
        const iterator operator++(int);
        iterator& operator--();
        const iterator operator--(int); 
        bool operator==(iterator) const;
        bool operator!=(iterator) const;
    private:
        node* current;
};

// ctor/dtor
    template<typename T>
    list<T>::list() 
: head(static_cast<node*>(operator new(sizeof(node))))
{
    head->succ = head->pred = head;
}
    template<typename T>
    list<T>::list(const list<T>&rhs)
: head(static_cast<node*>(operator new(sizeof(node))))
{
    head->succ = head->pred = head;

    for (node *t = rhs.head->succ; t != rhs.head; t = t->succ)
        push_back(t->datum);
}

template<typename T>
    template<class InputIterator>
    list<T>::list(InputIterator begin, InputIterator end)
: head(static_cast<node*>(operator new(sizeof(node))))
{
    head->succ = head->pred = head;
    while (begin != end)
        push_back(*(begin++));
}

    template<typename T>
list<T>::~list()
{
    while (!empty())
        pop_front();
    operator delete(head);
}

// modifiers
    template<typename T>
typename list<T>::iterator list<T>::insert(iterator it, const T&val)
{
    node *t = it;
    return t->pred = t->pred->succ = new node(val, t->pred, t);
}
    template<typename T>
typename list<T>::iterator list<T>::erase(iterator it)
{
    node *t = it, *r = 0;
    r = t->pred->succ = t->succ;
    t->succ->pred = t->pred;
    delete t;

    return r;
}

// list operations
    template<typename T>
void list<T>::unique()
{
    node *t = head->succ;
    for (node *t = head->succ; t != head; t = t->succ)
        for (node *s = t->succ; s != head && t->datum == s->datum; s = erase(iterator(s)))
            ;
}
    template<typename T>
    list<T>::const_iterator::const_iterator(const node *t)
: current(t)
{
}
template<typename T>
typename list<T>::const_reference list<T>::const_iterator::operator*() const
{
    return current->datum;
}
template<typename T>
typename list<T>::const_pointer list<T>::const_iterator::operator->() const
{
    return &current->datum;
}
    template<typename T>
typename list<T>::const_iterator& list<T>::const_iterator::operator++()
{
    current = current->succ;
    return *this;
}
    template<typename T>
const typename list<T>::const_iterator list<T>::const_iterator::operator++(int)
{
    const_iterator old = *this;
    ++*this;
    return old;
}

    template<typename T>
typename list<T>::const_iterator& list<T>::const_iterator::operator--()
{
    current = current->pred;
    return *this;
}
    template<typename T>
const typename list<T>::const_iterator list<T>::const_iterator::operator--(int)
{
    const_iterator old = *this;
    --*this;
    return old;
}
template<typename T>
bool list<T>::const_iterator::operator==(const_iterator rhs) const
{
    return current==rhs.current;
}
template<typename T>
bool list<T>::const_iterator::operator!=(const_iterator rhs) const
{
    return current!=rhs.current;
}
template<typename T>
list<T>::iterator::operator node*() const
{
    return current;
}
template<typename T>
list<T>::iterator::operator const_iterator() const
{
    return const_iterator(current);
}
    template<typename T>
    list<T>::iterator::iterator(node *t)
: current(t)
{
}
template<typename T>
typename list<T>::reference list<T>::iterator::operator*() const
{
    return current->datum;
}
template<typename T>
typename list<T>::pointer list<T>::iterator::operator->() const
{
    return &current->datum;
}
    template<typename T>
typename list<T>::iterator& list<T>::iterator::operator++()
{
    current = current->succ;
    return *this;
}
    template<typename T>
const typename list<T>::iterator list<T>::iterator::operator++(int)
{
    iterator old = *this;
    ++*this;
    return old;
}
    template<typename T>
typename list<T>::iterator& list<T>::iterator::operator--()
{
    current = current->pred;
    return *this;
}
    template<typename T>
const typename list<T>::iterator list<T>::iterator::operator--(int) 
{
    iterator old = *this;
    --*this;
    return old;
}
template<typename T>
bool list<T>::iterator::operator==(iterator rhs) const
{
    return current==rhs.current;
}
template<typename T>
bool list<T>::iterator::operator!=(iterator rhs) const
{
    return current!=rhs.current;
}
