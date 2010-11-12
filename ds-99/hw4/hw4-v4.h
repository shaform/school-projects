/**
 * -- hw4 version 4 --
 * Still failing to gain faster speed,
 * this version employs a LR parser to evaluate expressions directly.
 * -------------------
 * I_refs	=445762
 * m_total	=16165
 * priority	=1.450
 * -------------------
 */
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cctype>
namespace HOMEWORK {


	//	lexer

	enum Token { END = 0, MINUS, PLUS, MUL, DIV, POW, LP, RP, NUM, ERROR, EXP };

	int tk;
	double tkd;
	bool isopr;
	const char *curr;

	inline void lex()
	{
		if (isopr) {
			isopr = false;
			switch (*(curr++)) {
				case '+':
					tk = PLUS;
					break;
				case '-':
					tk = MINUS;
					break;
				case '*':
					tk = MUL;
					break;
				case '/':
					tk = DIV;
					break;
				case '^':
					tk = POW;
					break;
				case ')':
					tk = RP;
					isopr = true;
					break;
				case '\0':
					tk = END;
					break;
				default:
					tk = ERROR;
			}
		} else if (*curr=='(') {
			tk = LP;
			++curr;
		} else if (*curr=='+' || *curr=='.') {
			tk = ERROR;
		} else {
			register char *endptr;
			tkd = strtod(curr, &endptr);

			if (curr == endptr)
				tk = ERROR;
			else {
				curr = endptr;
				isopr = true;
				tk = NUM;
			}
		}
	}



	//	parser


	double res;

	/**
	 * LR Parsing Table
	 * positive for shift
	 * negative for reduce
	 * zero if error
	 */
	const size_t STSZ = 16;
	const size_t TKSZ = 10;
	const int ACCEPTED = -100;

	const int LR_TABLE[STSZ][TKSZ] = {
		{0,0,0,0,0,0,2,0,1},
		{-1,-1,-1,-1,-1,-1,0,-1,0},
		{0,0,0,0,0,0,2,0,1},
		{ACCEPTED,5,6,7,8,9,0,0,0},
		{0,5,6,7,8,9,0,10},
		{0,0,0,0,0,0,2,0,1},
		{0,0,0,0,0,0,2,0,1},
		{0,0,0,0,0,0,2,0,1},
		{0,0,0,0,0,0,2,0,1},
		{0,0,0,0,0,0,2,0,1},
		{-7,-7,-7,-7,-7,-7,0,-7,0},
		{-3,-3,-3,7,8,9,0,-3,0},
		{-2,-2,-2,7,8,9,0,-2,0},
		{-4,-4,-4,-4,-4,9,0,-4,0},
		{-5,-5,-5,-5,-5,9,0,-5,0},
		{-6,-6,-6,-6,-6,9,0,-6,0}
	};
	const int GOTO[STSZ] = {3,0,4,0,0,11,12,13,14,15};

	/**
	 *
	 */

	struct node {
		int tk;
		int st;
		double d;
	} tnode;


	/**
	 * parsing stack
	 */

	const size_t PSZ = 15;
	const size_t PMUL = 2;

	node *start = static_cast<node*>(calloc(PSZ, sizeof(node)));
	node *finish = start+1;
	node *end_storage = start+PSZ;
	size_t p_sz = PSZ;

	void p_expand()
	{
		size_t nsz = p_sz*PMUL;

		node *nstart = static_cast<node*>(malloc(nsz*sizeof(node)));
		memcpy(nstart, start, p_sz*sizeof(node));
		free(start);


		start = nstart;
		finish = start + p_sz;

		p_sz = nsz;
		end_storage = start+p_sz;
	}

	inline void p_push(const node &e)
	{
		if (finish == end_storage)
			p_expand();

		*(finish++) = e;
	}
	inline void p_pop() { --finish; }
	inline node& p_top() { return *(finish-1); }
	inline node& p_prev() { return *(finish-2); }
	inline void p_clear() { finish = start+1; }

	inline bool parse()
	{
		isopr = false;
		p_clear();
		while (true) {
			lex();
reduced:
			register int t = LR_TABLE[p_top().st][tk];
			if (t > 0) {
				// Shift #t
				tnode.tk = tk;
				tnode.st = t;
				if (tk == NUM) tnode.d = tkd;

				p_push(tnode);
			} else if (t < 0) {
				if (t == ACCEPTED) {
					res = p_top().d;
					return false;
				}
				// Reduce #t
				if (t==-1) {
					// exp -> NUM
					p_top().tk = EXP;
				} else if (t==-7) {
					// exp -> (exp)
					p_pop();
					tnode = p_top();
					p_pop();
					p_top() = tnode;
				} else {
					register double td = p_top().d;
					p_pop();
					p_pop();
					switch (t) {
						case -2:
							// exp -> exp + exp
							p_top().d += td;
							break;
						case -3:
							// exp -> exp - exp
							p_top().d -= td;
							break;
						case -4:
							// exp -> exp * exp
							p_top().d *= td;
							break;
						case -5:
							// exp -> exp / exp
							p_top().d /= td;
							break;
						case -6:
							// exp -> exp ^ exp
							p_top().d = pow(p_top().d, td);
							break;
					}
				}
				if (t = GOTO[p_prev().st]) {
					p_top().st = t;
					goto reduced;
				} else
					return true;
			} else
				return true;
		}
	}


	double Eval(const char *str, bool &error)
	{
		curr = str;
		if (error = parse())
			return -1.0;
		else
			return res;
	}
	struct destructor {
		~destructor() { free(start); }
	} des;

}
