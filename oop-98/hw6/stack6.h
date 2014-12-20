#include "deque6.h"
template<typename T,typename Container=deque<T> >
class stack {
    public:

        // types
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;

        // ctor
        explicit stack(const Container& =Container());

        // modifiers
        void push(const value_type&);
        void pop();

        // element access
        value_type& top();
        const value_type& top() const;

        // capacity
        bool empty() const;
        size_type size()  const;

    private:        
        Container d;
};

    template<typename T, typename Container>
stack<T, Container>::stack(const Container&c) : d(c)
{
}
    template<typename T, typename Container>
void stack<T, Container>::push(const value_type&val)
{
    d.push_back(val);
}
    template<typename T, typename Container>
void stack<T, Container>::pop()
{
    d.pop_back();
}
    template<typename T, typename Container>
typename stack<T, Container>::value_type& stack<T, Container>::top()
{
    return d.back();
}
template<typename T, typename Container>
const typename stack<T, Container>::value_type& stack<T, Container>::top() const
{
    return d.back();
}
template<typename T, typename Container>
bool stack<T, Container>::empty() const
{
    return d.empty();
}
template<typename T, typename Container>
typename stack<T, Container>::size_type stack<T, Container>::size()  const
{
    return d.size();
}
