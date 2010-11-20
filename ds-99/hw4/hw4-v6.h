/**
 * -- hw4 version 6 --
 * Revised from version 5,
 * it merges parser and lexer, and includes some
 * low level optimization techniques.
 * -------------------
 * I_refs	=87384
 * m_total	=15442
 * priority	=3.353
 * -------------------
 */
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdio>


namespace HOMEWORK {
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

	//	lexer

	typedef char CT;
	const CT NUM = -2;
	const CT ERROR = -1;

	static double tkdtod(const char *str, const char *&ptr)
	{
		double d = 0.0, d2 = 0.1;
		ptr = str;

		if (*ptr=='-')
			++ptr;
		while (pred[*ptr] == 0 && *ptr!= '.') {
			d *= 10.0;
			d += (*ptr-'0');
			++ptr;
		}
		if (*ptr == '.') {
			++ptr;
			while (pred[*ptr] == 0) {
				d += d2*(*ptr-'0');
				d2 /= 10.0;
				++ptr;
			}
		}
		if (*str == '-') {
			if (ptr == str+1) ptr = str;
			d = -d;
		}
		return d;
	}

	// Stacks
	double n_sk[5];
	CT op_sk[9];

	double *n_head = n_sk-1;
	CT *op_head = op_sk-1;

	static void n_push(double e) { *++n_head = e; }
	static void op_push(CT e) { *++op_head = e; }

	static void n_pop() { --n_head; }
	static void op_pop() { --op_head; }

	static double& n_top() { return *n_head; }
	static CT& op_top() { return *op_head; }

	static void n_clear() { n_head = n_sk-1; }
	static void op_clear() { op_head = op_sk-1; }

	static bool op_empty() { return op_head<op_sk; }


	// Evalulate the postfix stack.
	static void eval(CT op)
	{
		// Get the two operands, assuming that they exist.
		double u;

		u = n_top();
		n_pop();

		double &t = n_top();


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
	}

	double res;

	static bool parse(const char *curr)
	{
		double tkd;
		bool isopr = false;
		op_clear();
		n_clear();
		int par_c = 0;
		while (true) {
			if (isopr) {
				isopr = false;
				if (*curr == '\0')
					break;
				if (pred[*curr]) {
					if (*curr == ')') {
						if (--par_c < 0) return true;

						while (op_top() != '(') {
							eval(op_top());
							op_pop();
						}
						// pop '('
						op_pop();
						isopr = true;
					} else {

						while (!op_empty() && pred[op_top()] >= pred[*curr] && *curr != '^') {
							eval(op_top());
							op_pop();
						}
						op_push(*curr);
					}
					++curr;
					continue;
				} else
					return true;
			}
			if (*curr=='(') {
				op_push('(');
				++par_c;
				++curr;
				continue;
			}
			if ((pred[*curr] && *curr != '-') || *curr == '.')
				return true;

			const char *endptr;
			tkd = tkdtod(curr, endptr);

			if (curr == endptr)
				return true;

			curr = endptr;
			isopr = true;
			n_push(tkd);
		}
		if (par_c) return true;
		while (!op_empty()) {
			eval(op_top());
			op_pop();
		}

		res = n_top();
		return false;
	}

	double Eval(const char *str, bool &error)
	{
		error = parse(str);
		if (error)
			return -1.0;
		return res;
	}
}
