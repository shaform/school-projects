/**
 * -- hw4 version 3 --
 * This version eliminates the stack class completely
 * to avoid indirect reference when calling member functions.
 * But it only gains a slight improvement over version 1.
 * -------------------
 * I_refs	=399684
 * m_total	=15395
 * priority	=1.570
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

	bool isopr;
	const char *curr;

	CT tk;
	double tkd;

	inline int lex()
	{
		if (isopr) {
			isopr = false;
			if (pred[*curr])
				return *(curr++);
			else
				return ERROR;
		} else if (*curr=='(')
			return *(curr++);
		else if (pred[*curr] && *curr != '-')
			return ERROR;
		else {
			char *endptr;
			tkd = strtod(curr, &endptr);

			if (curr == endptr)
				return ERROR;
			else {
				curr = endptr;
				isopr = true;
				return NUM;
			}
		}
	}
	// Stacks
	double n_sk[5];
	CT op_sk[9];

	double *n_head = n_sk-1;
	CT *op_head = op_sk-1;

	inline void n_push(double e) { *++n_head = e; }
	inline void op_push(CT e) { *++op_head = e; }

	inline void n_pop() { --n_head; }
	inline void op_pop() { --op_head; }

	inline double& n_top() { return *n_head; }
	inline CT& op_top() { return *op_head; }

	inline void n_clear() { n_head = n_sk-1; }
	inline void op_clear() { op_head = op_sk-1; }

	inline bool op_empty() { return op_head<op_sk; }


	// Evalulate the postfix stack.
	inline void eval(CT op)
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

	int par_c;
	double res;
	inline bool parse()
	{
		isopr = false;
		op_clear();
		n_clear();
		par_c = 0;
		while (tk = lex()) {
			switch (tk) {
				case NUM:
					n_push(tkd);
					break;
				case ERROR:
					return true;
				case '(':
					op_push('(');
					++par_c;
					break;
				case ')':
					if (--par_c < 0) return true;

					while (op_top() != '(') {
						eval(op_top());
						op_pop();
					}
					// pop '('
					op_pop();
					isopr = true;
					break;
				default:
					while (!op_empty() && pred[op_top()] >= pred[tk]) {
						eval(op_top());
						op_pop();
					}
					op_push(tk);
			}

		}

		if (par_c) return true;
		else while (!op_empty()) {
			eval(op_top());
			op_pop();
		}

		res = n_top();
		return false;
	}

	double Eval(const char *str, bool &error)
	{
		curr = str;
		error = parse();
		if (error)
			return -1.0;
		else
			return res;
	}
}
