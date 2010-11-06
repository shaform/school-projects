#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdio>


using namespace std;
namespace WPH {

	const size_t SSZ = 15;
	const size_t MUL = 2;

	template<typename T>
		class stack {
			public:
				// types
				typedef size_t size_type;
				typedef T& reference;
				typedef const T& const_reference;

				// ctor/copy ctor/dtor
				stack()
					: start(static_cast<T*>(operator new[](sizeof(T)*SSZ))),
					finish(start),
					end_storage(start+SSZ),
					sz(SSZ) {}
				~stack() { operator delete[] (start); }

				// capacity
				size_type size() const { return finish - start; }
				size_type capacity() const { return sz; }
				bool empty() const { return start == finish; }

				// stack operation
				void pop() { --finish; }
				reference top() { return *(finish-1); }

				void push(const T &e)
				{
					if (finish == end_storage)
						expand();

					*(finish++) = e;
				}

				void expand();

				// modifier
				void clear() { finish = start; }
			private:
				T *start, *finish, *end_storage;
				size_type sz;

				stack(const stack<T>&); 
				stack<T> &operator=(const stack<T>&);
		};

	template<typename T>
		void stack<T>::expand()
		{
			size_type nsz = sz*MUL;
			size_t psz = nsz*sizeof(T);

			T *nstart = static_cast<T*>(operator new[](psz));
			memcpy(nstart, start, sz*sizeof(T));

			operator delete[](start);

			start = nstart;
			finish = start+sz;

			sz = nsz;
			end_storage = start+sz;
		}
}


namespace HOMEWORK {

	using WPH::stack;

	const char *curr;

	stack<double> s;
	stack<char> op_s;

	// next token
	char tkc;
	double tkd;

	// 1 : ()
	// 2 : + -
	// 3 : * /
	// 4 : ^
	const int pred[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,1,3,2,0,2,0,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	};

	bool ttype, prev_ttype;
	// false is operator, true is operand


	inline double strtotkd(const char *str, char *&ptr)
	{
		char *cstr = ptr = const_cast<char *>(str);

		if (*ptr=='-')
			++ptr;

		if (isdigit(*ptr)) {
			while (isdigit(*++ptr));
			if (*ptr == '.')
				while (isdigit(*++ptr));
			return atof(str);
		} else {
			ptr = cstr;
			return 0.0;
		}
	}
	inline bool parse(bool &error)
	{
		if (prev_ttype) {
			if (*curr=='.' || *curr=='(') {
				error = true;
				return false;
			} else if (*curr == '\0')
				return false;
			else
				tkc = *(curr++);
			ttype = false;
		} else {
			if (*curr=='(') {
				tkc = '(';
				ttype = false;
				++curr;
				return true;
			}

			char *endptr;
			tkd = strtotkd(curr, endptr);

			if (endptr==curr) {
				error = true;
				return false;
			}
			curr = endptr;

			ttype = true;
		}

		return true;
	}

	// Evalulate the postfix stack.
	void eval(char op)
	{
		// Get the two operands, assuming that they exist.
		double t, u;

		u = s.top();
		s.pop();

		t = s.top();
		s.pop();

		// Do the calculation.
		switch (op) {
			case '*':
				t *= u;
				break;
			case '+':
				t += u;
				break;
			case '-':
				t -= u;
				break;
			case '/':
				t /= u;
				break;
			default:
				t = pow(t, u);
		}


		s.push(t);
	}

	int par_c;
	double Eval(const char *str, bool &error)
	{
		curr = str;
		s.clear();
		op_s.clear();
		par_c = 0;
		prev_ttype = false;
		error = false;

		while (parse(error)) {
			// if is a operand
			if (ttype) {
				s.push(tkd);
				// if is a operator
			} else {
				if (tkc=='(') {
					op_s.push('(');
					++par_c;
				} else if (tkc==')') {
					// checking parentheses matching
					if (--par_c < 0) {
						error = true;
						break;
					}

					while (op_s.top() != '(') {
						eval(op_s.top());
						op_s.pop();
					}
					// pop '('
					op_s.pop();

					// the pair of parentheses is a operand
					ttype = true;
				} else {

#if 0
					if (pred[tkc]) {
#endif
						while (!op_s.empty() && pred[op_s.top()] >= pred[tkc]) {
							eval(op_s.top());
							op_s.pop();
						}
						op_s.push(tkc);
#if 0
					} else {
						// if not a valid op
						error = true;
						break;
					}
#endif
				}

			}
			prev_ttype = ttype;
		}
		if (par_c)
			error = true;

		if (error)
			return -1.0;
		else while (!op_s.empty()) {
			eval(op_s.top());
			op_s.pop();
		}

		if (s.size() != 1) {
			error = true;
			return -1.0;
		} else
			return s.top();
	}

}
