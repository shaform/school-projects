/**
 * -- hw4 version 6 --
 * Revised from version 5,
 * this version merges parser and lexer
 * and includes some low level optimization.
 * -------------------
 * I_refs	=80099
 * m_total	=15319
 * priority	=3.516
 * -------------------
 */
#include <cmath>
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
	typedef char CT;
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
		double u = n_top();
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
		double tkd, n_sk[5];
		CT op_sk[9];
		op_head = op_sk-1;
		n_head = n_sk-1;
		int par_c = 0;
		bool isopr = false;
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


			// tkdtod
			const char *ptr = str;
			int tkn=0;
			double d2 = 0.1;
			if (*ptr=='-')
				++ptr;
			while (pred[*ptr] == 0 && *ptr!= '.') {
				tkn *= 10;
				tkn += (*ptr-'0');
				++ptr;
			}
			tkd = tkn;
			if (*ptr == '.') {
				++ptr;
				while (pred[*ptr] == 0) {
					tkd += d2*(*ptr-'0');
					d2 /= 10.0;
					++ptr;
				}
			}
			if (*str == '-') {
				if (ptr == str+1) goto err;
				tkd = -tkd;
			}
			str = ptr;
			// tkdtod ends


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
