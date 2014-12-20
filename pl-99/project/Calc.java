import javax.swing.*;
import javax.swing.event.*;
import java.lang.Math;
import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList; 
public class Calc extends JFrame {
	// Interface
	private JPanel panel;
	private JLabel comLabel;
	private JScrollPane comPane;
	private JTextArea comArea;
	private JLabel viewLabel;
	private JScrollPane viewPane;
	private JTextArea viewArea;
	// Internal members.
	private ExprEvaluator evaluator = new ExprEvaluator();
	private ExprModifier modifier = new ExprModifier();
	private ArrayList<String> stack;
	private State state = new WaitForNum();
	private int cursor;
	private int pair = 0;
	private String M;
	private String M1;
	private String M2;
	public Calc()
	{
		super("Java Calculator");
		initInterface();
		initStorage();

		Action action = new AbstractAction() {
			public void actionPerformed(ActionEvent actionEvent) {
				char c = actionEvent.getActionCommand().charAt(0);
				switch (c) {
					case '=':
						state.save();
					case 'O':
						state.off();
						break;
					case 'C':
						state.clear();
						break;
					case 'S':
						state.stack();
						break;
					case 'Q':
						state.sqrt();
						break;
					case 'M':
						state.memory();
						break;
					case ' ':
						state.space();
						break;
					case '+':
					case '-':
					case '*':
					case '/':
					case 'E':
						state.oper(c);
						break;
					case '.':
						state.dot();
						break;
					case '(':
						state.open();
						break;
					case ')':
						state.close();
						break;
					default:
						//System.out.println('0' < '9');

						if (c >= '0' && c <= '9') {
							//System.out.println("yes\n");
							state.num(c);
						} else {
							//System.out.println("no\n");
						}
				}
				//System.out.println(actionEvent.getActionCommand());
			}
		};
		Action actenter = new AbstractAction() {
			public void actionPerformed(ActionEvent actionEvent) {
				state.enter();
				//System.out.println("Enter");
			}
		};
		Action actback = new AbstractAction() {
			public void actionPerformed(ActionEvent actionEvent) {
				state.backspace();
				//System.out.println("Backspace");
			}
		};


		// Register all valid key strokes
		comArea.getActionMap().put("keyPressed", action);
		comArea.getActionMap().put("keyEnter", actenter);
		comArea.getActionMap().put("keyBack", actback);
		InputMap imap = comArea.getInputMap(/*JComponent.WHEN_IN_FOCUSED_WINDOW*/);
		imap.put(KeyStroke.getKeyStroke('S'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('O'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('Q'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('C'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('E'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('M'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke(' '), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('+'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('-'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('*'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('/'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('='), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('0'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('1'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('2'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('3'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('4'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('5'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('6'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('7'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('8'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('9'), "keyPressed");
		imap.put(KeyStroke.getKeyStroke('.'), "keyPressed");

		imap.put(KeyStroke.getKeyStroke('('), "keyPressed");
		imap.put(KeyStroke.getKeyStroke(')'), "keyPressed");


		imap.put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "keyEnter");
		imap.put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, 0), "keyBack");

	}
	private void initInterface()
	{
		setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		panel = new JPanel();
		panel.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));

		// Create the command line input area.
		comLabel = new JLabel("Enter expressions...");
		comArea = new JTextArea(8, 50);
		comArea.setEditable(false);
		comPane = new JScrollPane(comArea);
		

		// Create the temporary display area.
		viewLabel = new JLabel("Additional Info:");
		viewArea = new JTextArea(5, 50);
		viewArea.setEditable(false);
		viewPane = new JScrollPane(viewArea);


		panel.add(comLabel);
		panel.add(comPane);
		panel.add(viewLabel);
		panel.add(viewPane);

		getContentPane().setLayout(new BorderLayout());
		getContentPane().add(panel, BorderLayout.CENTER);

		pack();
	}

	// The states of the calculator.
	private abstract class State {
		protected State prev = null;
		protected int hold = 0;
		State()
		{
		}
		State(State s)
		{
			prev = s;
			init();
		}
		public void init()
		{
			if (pair == 0) {
				String expr = modifier.strip(getLine());
				if (expr.length() > 0) {
					char op = expr.charAt(expr.length()-1);
					if (op == '+' || op == '-') {
						expr = modifier.replaceMemory(expr.substring(0, expr.length()-1));
						setView(Double.toString(evaluator.evaluate(expr)));
						return;
					}
				}

			}
		}
		public void num(char n) { }
		public void dot() {}
		public void space() {}
		public void enter() {}
		public void open() {}
		public void close() {}
		public void save() {}
		public void backspace()
		{
			if (hold > 0) {
				--hold;
				removeText();
			} else if (prev != null) {
				removeText();
				String line = getLine();
				if (line.length() == 0 || (line.length() == 1 && line.charAt(0) == '-')) {
					clearLine();
					state = new WaitForNum();
				} else {
					state = prev;
					state.init();
				}
			}
		}
		public void stack()
		{
			addText('S');
			state = new ReadyStack(this);
		}
		public void clear()
		{
			addText('C');
			state = new ReadyClear(this);
		}
		public void off()
		{
			addText('O');
			state = new ReadyOff(this);
		}

		public void sqrt() {}
		public void oper(char op) {}
		public void memory() {}

		public void setPreviousState(State s) { prev = s; }
		public State getPreviousState() { return prev; }
	}
	private abstract class ReadyState extends State {
		ReadyState(State s)
		{
			super(s);
		}
		public void stack() {}
		public void clear() {}
		public void off() {}
		public abstract void enter();
	}
	private class ReadyStack extends ReadyState {
		ReadyStack(State s)
		{
			super(s);
		}
		public void enter()
		{
			removeText();
			nextLine();
			addText('S');
			nextLine();
			setView(getStack());
			state = new WaitForNum();
		}
	}
	private class ReadyOff extends ReadyState {
		ReadyOff(State s)
		{
			super(s);
		}
		public void enter()
		{
			System.exit(0);
		}
	}
	private class ReadyClear extends ReadyState {
		ReadyClear(State s)
		{
			super(s);
		}
		public void enter()
		{
			initStorage();
			state = new WaitForNum();
		}
	}
	private class ReadySave extends ReadyState {
		ReadySave(State s)
		{
			super(s);
		}
		public void enter()
		{
			String s = modifier.strip(getLine());
			String t;
			char c = peekText();
			switch (c) {
				case '1':
				case '2':
					removeText();
					removeText();
					break;
				default:
					removeText();
					s = s.substring(0, s.length()-1);
					s = modifier.replaceMemory(s);
					t = Double.toString(evaluator.evaluate(s));
					M = t;
			}
			s = modifier.replaceMemory(s);
			t = Double.toString(evaluator.evaluate(s));
			M = t;

			if (c=='1') M1 = M;
			else if (c=='2') M2 = M;

			stack.add(0, s);

			nextLine();
			state = new WaitForNum();

			if (t.length() > 0 && t.charAt(0) == '-') {
				state.num('0');
				state.oper('-');
				removeText();
				removeText();
				addText('-');
			}


			for(int i=0; i<t.length(); i++) {
				c = t.charAt(i);
				if (c == '.')
					state.dot();
				else if (c >= '0' && c <= '9')
					state.num(c);
			}
		}
	}
	private class SaveMemory extends ReadyState {
		SaveMemory(State s)
		{
			super(s);
		}
		public void enter() {}
		public void num(char n)
		{
			if (n == '1' || n == '2') {
				addText(n);
				state = new ReadySave(this);
			}
		}
	}
	private class GetMemory extends ContinueNum  {
		GetMemory(State s)
		{
			super(s);
		}
		public void num(char n)
		{
			if (n == '1' || n == '2') {
				addText(n);
				state = new WaitForOp(this);
			}
		}
	}

	private class WaitForNum extends State {
		WaitForNum()
		{
		}
		WaitForNum(State s)
		{
			super(s);
		}

		public void open()
		{
			addText('(');
			++hold;
			++pair;
		}
		public void num(char n)
		{
			addText(n);
			state = new ContinueIntegral(this);
		}
		public void space()
		{
			addText(' ');
			++hold;
		}
		public void memory()
		{
			addText('M');
			state = new GetMemory(this);
		}
	}
	private abstract class ContinueNum extends State {
		ContinueNum(State s)
		{
			super(s);
		}
		public void num(char n)
		{
			addText(n);
			++hold;
		}
		public void close()
		{
			if (pair > 0) {
				addText(')');
				--pair;
				state = new WaitForOp(this);
			}
		}
		public void space()
		{
			addText(' ');
			state = new WaitForOp(this);
		}
		public void oper(char op)
		{
			addText(op);
			state = new WaitForNum(this);
		}
		public void sqrt()
		{
			addText('Q');
			state = new WaitForOp(this);
		}
		public void memory()
		{
			addText('M');
			state = new SaveMemory(this);
		}
		public void save()
		{
			if (pair == 0) {
				addText('=');
				state = new ReadySave(this);
			}
		}
		public void enter()
		{
			if (pair == 0) {
				String expr = modifier.strip(getLine());
				if (expr.length() > 0) {
					expr = modifier.replaceMemory(expr.substring(0, expr.length()));
					setView(Double.toString(evaluator.evaluate(expr)));
				}

			}
		}
	}
	private class ContinueIntegral extends ContinueNum {
		ContinueIntegral(State s)
		{
			super(s);
		}
		public void dot()
		{
			addText('.');
			state = new WaitForFractional(this);
		}
	}
	private class WaitForFractional extends State {
		WaitForFractional(State s)
		{
			super(s);
		}
		public void num(char n)
		{
			addText(n);
			state = new ContinueFractional(this);
		}
	}
	private class ContinueFractional extends ContinueNum {
		ContinueFractional(State s)
		{
			super(s);
		}
	}
	private class WaitForOp extends State {
		WaitForOp(State s)
		{
			super(s);
		}
		public void close()
		{
			if (pair > 0) {
				addText(')');
				--pair;
				++hold;
			}
		}
		public void space()
		{
			addText(' ');
			++hold;
		}
		public void oper(char op)
		{
			addText(op);
			state = new WaitForNum(this);
		}
		public void sqrt()
		{
			addText('Q');
			++hold;
		}
		public void memory()
		{
			addText('M');
			state = new SaveMemory(this);
		}
		public void save()
		{
			if (pair == 0) {
				addText('=');
				state = new ReadySave(this);
			}
		}
		public void enter()
		{
			if (pair == 0) {
				String expr = modifier.strip(getLine());
				if (expr.length() > 0) {
					expr = modifier.replaceMemory(expr.substring(0, expr.length()));
					setView(Double.toString(evaluator.evaluate(expr)));
				}

			}
		}
	}
	// An expressin evaluator, assuming the input is always right.
	private class ExprEvaluator {
		private int curr;
		private String expr;
		// skip the spaces in the input string.
		private void skip()
		{
			while (curr < expr.length() && expr.charAt(curr) == ' ' && curr + 1 < expr.length())
				++curr;
		}
		// parse numbers with sqrt
		private double parseN()
		{
			skip();
			boolean neg = false;
			if (curr < expr.length() && expr.charAt(curr) == '-') {
				neg = true;
				++curr;
			}
			double sum = 0.0;
			while (curr < expr.length() && (expr.charAt(curr) <= '9' && expr.charAt(curr) >= '0')) {
				sum *= 10;
				sum += (expr.charAt(curr)-'0');
				++curr;
			}
			if (curr < expr.length() && expr.charAt(curr) == '.') {
				++curr;
				double d2 = 0.1;
				while (curr < expr.length() && (expr.charAt(curr) <= '9' && expr.charAt(curr) >= '0')) {
					sum += d2*(expr.charAt(curr)-'0');
					d2 /= 10.0;
					++curr;
				}
			}
			if (neg) return -sum;
			else return sum;
		}
		private double parseA()
		{
			skip();
			double sum = 0.0;
			if (expr.charAt(curr) == '(') {
				++curr;
				sum = parseE();
				skip();
				// skip ')'
				++curr;
			} else
				sum = parseN();
			skip();
			while (curr < expr.length() && expr.charAt(curr) == 'Q') {
				++curr;
				skip();
				sum = Math.sqrt(sum);
			}
			return sum;
		}
		private double parseF()
		{
			double sum = parseA();
			skip();
			if (curr < expr.length() && expr.charAt(curr) == 'E') {
				++curr;
				double next = parseF();
				sum = Math.pow(sum, next);
				skip();
			}
			return sum;
		}
		private double parseT()
		{
			double sum = parseF();
			skip();
			while (curr < expr.length() && (expr.charAt(curr) == '*' || expr.charAt(curr) == '/')) {
				char op = expr.charAt(curr);
				++curr;
				double next = parseF();
				switch (op) {
					case '*':
						sum *= next;
						break;
					case '/':
						sum /= next;
						break;
				}
				skip();
			}
			return sum;
		}
		private double parseE()
		{
			double sum = parseT();
			skip();
			while (curr < expr.length() && (expr.charAt(curr) == '+' || expr.charAt(curr) == '-')) {
				char op = expr.charAt(curr);
				++curr;
				double next = parseT();
				switch (op) {
					case '+':
						sum += next;
						break;
					case '-':
						sum -= next;
						break;
				}
				skip();
			}
			return sum;

		}
		public synchronized double evaluate(String s)
		{
			curr = 0;
			expr = s;
			if (expr.length() == 0) return 0;
			return parseE();
		}
	}
	private class ExprModifier {
		public String strip(String s)
		{
			return s.replaceAll("\\s+", "");
		}
		public String replaceMemory(String s)
		{
			return s.replaceAll("M1", M1).replaceAll("M2", M2).replaceAll("M", M);
		}
	};
	private void initStorage()
	{
		stack = new ArrayList<String>();
		M1 = M2 = M = "0";
		pair = 0;
		comArea.setText("# ");
		cursor = 2;
	}
	private void clearLine()
	{
		int t = comArea.getText().length();
		if (t > cursor) {
			comArea.replaceRange("", cursor, t);
		}
	}
	private void nextLine()
	{
		comArea.append("\n# ");
		cursor = comArea.getText().length();
	}
	private String getLine()
	{
		return comArea.getText().substring(cursor);
	}
	private void addText(char c)
	{
		clearView();
		comArea.append(Character.toString(c));
	}
	private void setView(String s)
	{
		viewArea.setText(s);
	}
	private void clearView()
	{
		setView("");
	}
	private void removeText()
	{
		clearView();
		int t = comArea.getText().length();
		if (t > cursor) {
			if (peekText() == '(') --pair;
			if (peekText() == ')') ++pair;
			comArea.replaceRange("", t-1, t);
		}
	}
	private char peekText()
	{
		return comArea.getText().charAt(comArea.getText().length()-1);
	}
	private String getStack()
	{
		String s = "";
		for(Iterator it = stack.iterator(); it.hasNext();) {
			s += it.next() + "\n";
		}
		return s;
	}
	public static void main(String[] args) {
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				new Calc().setVisible(true);
			}
		});
	}
}
