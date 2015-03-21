template<class T>
class list {
    public:
        // types
        class iterator;
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
        list<T>& operator-=(const list<T>&);    ////// write this function //////
        iterator insert(iterator,const T& =T());
        iterator erase(iterator);
        void push_front(const T& d) { insert(begin(),d); }
        void push_back(const T& d) { insert(end(),d); }
        void pop_front() { erase(begin()); }
        void pop_back() { erase(--end()); }
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
        const_iterator(const node* cur=0) : current(cur) {}
        const_iterator(iterator it) : current(it.current) {}   // conversion (1)
        const_reference operator*() const { return current->datum; }
        const_pointer operator->() const { return &operator*(); }
        const_iterator& operator++() { current=current->succ; return *this; }
        const const_iterator operator++(int) { iterator old=*this; ++*this; return old;  }
        const_iterator& operator--() { current=current->pred; return *this; }
        const const_iterator operator--(int) { iterator old=*this; --*this; return old;  }
        bool operator==(const_iterator it) const { return current==it.current; }
        bool operator!=(const_iterator it) const { return current!=it.current; }
    private:
        const node* current;
};

template<class T>
class list<T>::iterator {
    friend class list<T>;								// modifier (1)
    friend class const_iterator;							// conversion (1)
    public:
    iterator(node* cur=0) : current(cur) {}
    reference operator*() const { return current->datum; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++() { current=current->succ; return *this; }
    const iterator operator++(int) { iterator old=*this; ++*this; return old;  }
    iterator& operator--() { current=current->pred; return *this; }
    const iterator operator--(int) { iterator old=*this; --*this; return old;  }
    bool operator==(iterator it) const { return current==it.current; }
    bool operator!=(iterator it) const { return current!=it.current; }
    private:
    node* current;
};

// ctor
    template<class T>
    list<T>::list()
: head((node*)operator new(sizeof(node)))
{
    head->pred=head->succ=head;
}

template<class T>
    template<class InputIterator>
    list<T>::list(InputIterator first,InputIterator last)
: head((node*)operator new(sizeof(node)))
{
    head->pred=head->succ=head;
    while (first!=last) { push_back(*first); ++first; }
};

// copy ctor
    template<class T>
    list<T>::list(const list<T>& rhs)
: head((node*)operator new(sizeof(node)))
{
    head->pred=head->succ=head;
    node* q=rhs.head->succ;
    while (q!=rhs.head) { push_back(q->datum); q=q->succ; }
}

// dtor
    template<class T>
list<T>::~list()
{
    while (!empty()) pop_back();
    operator delete(head);
}

// modifier (1)
    template<class T>
typename list<T>::iterator list<T>::insert(iterator pos,const T& d)
{
    node* p=new node(d,pos.current->pred,pos.current);
    pos.current->pred->succ=p;
    pos.current->pred=p;
    return p;
}

    template<class T>
typename list<T>::iterator list<T>::erase(iterator pos)
{
    node* p=pos.current;
    ++pos;
    p->pred->succ=p->succ;
    p->succ->pred=p->pred;
    delete p;
    return pos;
}

    template<class T>
list<T>& list<T>::operator-=(const list<T>&rhs)
{
    if (this==&rhs)
        while (!empty())
            pop_back();
    else {
        list<T> to_insert, deleted;
        for (const_iterator rit=rhs.begin(); rit!=rhs.end(); ++rit) {

            for (iterator it=begin(); it!=end(); ++it) {
                if (*it == *rit) {
                    deleted.push_back(*it);

                    it = erase(it);
                    --it;

                }
            }
            bool found = false;
            for (const_iterator check=deleted.begin(); check!=deleted.end(); ++check)
                if (*check == *rit)
                    found = true;

            if (!found) to_insert.push_back(*rit);
        }

        while (!to_insert.empty()) {
            push_back(to_insert.front());
            to_insert.pop_front();
        }

    }
}
