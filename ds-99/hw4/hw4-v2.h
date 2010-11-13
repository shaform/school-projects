/**
 * -- hw4 version 2 --
 * Failing to get better performance than version 1,
 * this version attempts to detect error as soon as possible
 * to eliminate the time spent on calculating invalid expressions.
 * It also uses union to avoid multiple instantiations of stack template.
 * ** Updated **
 * Merged the tktod() from version 5.
 * -------------------
 * I_refs	=144637
 * m_total	=17402
 * priority	=2.455
 * -------------------
 */
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
	union nd {
		double d;
		char c;
	};

	const char *curr;

	stack<nd> s;
	stack<nd> op_s;

	// next token
	nd tk;

	// 1 : ()
	// 2 : + -
	// 3 : * /
	// 4 : ^
	const int pred[] = {
		8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,1,3,2,0,2,0,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	};

	bool isopr;
	// false is operator, true is operand


	inline double tktod(const char *str, char **ptr)
	{
		double d = 0.0, d2 = 0.1;
		*ptr = const_cast<char *>(str);

		if (**ptr=='-')
			++*ptr;
		while (pred[**ptr] == 0 && **ptr!= '.') {
			d *= 10.0;
			d += (**ptr-'0');
			++*ptr;
		}
		if (**ptr == '.') {
			++*ptr;
			while (pred[**ptr] == 0) {
				d += d2*(**ptr-'0');
				d2 /= 10.0;
				++*ptr;
			}
		}
		if (*str == '-') {
			if (*ptr == const_cast<char *>(str+1)) *ptr = const_cast<char *>(str);
			return -d;
		} else
			return d;
	}
	/*
	inline double strtotkd(const char *str, const char *&ptr)
	{
		ptr = str;

		if (*ptr=='-')
			++ptr;

		if (isdigit(*ptr)) {
			while (isdigit(*++ptr));
			if (*ptr == '.')
				while (isdigit(*++ptr));
			return atof(str);
		} else {
			ptr = str;
			return 0.0;
		}
	}
	*/
	inline bool parse()
	{
		if (*curr == '\0')
			return false;

		if (isopr) {
			tk.c = *(curr++);
			isopr = false;
		} else {
			if (*curr=='(') {
				tk.c = '(';
				isopr = false;
				++curr;
				return true;
			}

			char *endptr;
			tk.d = tktod(curr, &endptr);

			curr = endptr;

			isopr = true;
		}

		return true;
	}

	// Evalulate the postfix stack.
	void eval(char op)
	{
		// Get the two operands, assuming that they exist.
		double t, u;

		u = s.top().d;
		s.pop();

		t = s.top().d;
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


		nd tnd;
		tnd.d = t;
		s.push(tnd);
	}

	inline bool check(const char *str)
	{
		bool isopr = false;
		int pp = 0;

		while (*str != '\0') {
			if (isopr) {
				if (*str == ')') {
					if (--pp < 0) return false;
					isopr = true;
				} else if (pred[*str]==0) {
					return false;
				} else
					isopr = false;
			} else {
				if (*str=='(')
					++pp;
				else if (*str =='.')
					return false;
				else {
					char *endptr;
					tktod(str, &endptr);

					if (endptr==str)
						return false;

					isopr = true;
					str = endptr;
					continue;
				}
			}
			++str;
		}

		return isopr;
	}

	double Eval(const char *str, bool &error)
	{
		if (!check(str)) {
			error = true;
			return -1.0;
		} else
			error = false;

		curr = str;
		s.clear();
		op_s.clear();
		isopr = false;

		while (parse()) {
			if (isopr)
				s.push(tk);
			else {
				if (tk.c==')') {
					while (op_s.top().c != '(') {
						eval(op_s.top().c);
						op_s.pop();
					}
					// pop '('
					op_s.pop();

					// the pair of parentheses is a operand
					isopr = true;
				} else if (tk.c=='(')
					op_s.push(tk);
				else {
					while (!op_s.empty() && pred[op_s.top().c] >= pred[tk.c] && tk.c != '^') {
						eval(op_s.top().c);
						op_s.pop();
					}
					op_s.push(tk);
				}

			}
		}

		while (!op_s.empty()) {
			eval(op_s.top().c);
			op_s.pop();
		}

		return s.top().d;
	}

}
