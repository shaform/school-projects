/**
 * -- hw4 version 6 --
 * Revised from version 5,
 * it merges parser and lexer, and includes some
 * low level optimization techniques.
 * -------------------
 * I_refs	=84120
 * m_total	=15336
 * priority	=3.429
 * -------------------
 */
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdio>


namespace HOMEWORK {
	// -1: ( \0
	// 1 : ()
	// 2 : + -
	// 3 : * /
	// 4 : ^
	const int pred[] = {
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,-1,1,3,2,0,2,0,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	};

	//	lexer

	typedef char CT;

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

	double *n_head;
	CT *op_head;

	static void n_push(double e) { *++n_head = e; }
	static void op_push(CT e) { *++op_head = e; }

	static void n_pop() { --n_head; }
	static void op_pop() { --op_head; }

	static double& n_top() { return *n_head; }
	static CT& op_top() { return *op_head; }


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


	double Eval(const char *str, bool &error)
	{
		double tkd;
		double n_sk[5];
		CT op_sk[9];
		bool isopr = false;
		op_head = op_sk-1;
		n_head = n_sk-1;

		int par_c = 0;
		while (true) {
			if (isopr) {
				isopr = false;
				if (pred[*str] == -1) {
					if (*str == '\0')
						break;
					else goto err;
				}
				if (pred[*str]) {
					if (*str == ')') {
						if (--par_c < 0) goto err;

						while (op_top() != '(') {
							eval(op_top());
							op_pop();
						}
						// pop '('
						op_pop();
						isopr = true;
					} else {

						while (op_head>=op_sk && pred[op_top()] >= pred[*str]/* && *str != '^'*/) {
							eval(op_top());
							op_pop();
						}
						op_push(*str);
					}
					++str;
					continue;
				} else
					goto err;
			}
			if (*str=='(') {
				op_push('(');
				++par_c;
				++str;
				continue;
			}
			if ((pred[*str] && *str != '-') || *str == '.')
				goto err;

			const char *endptr;
			tkd = tkdtod(str, endptr);

			if (str == endptr)
				goto err;

			str = endptr;
			isopr = true;
			n_push(tkd);
		}
		if (par_c) goto err;
		while (op_head>=op_sk) {
			eval(op_top());
			op_pop();
		}

		error = false;
		return n_top();
err:
		error = true;
		return -1.0;
	}

}
