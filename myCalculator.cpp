#include<iostream>
#include<stack>
#include<string>
#include<stdexcept>
#include<fstream>

using namespace std;

class post
{
private:
	string p;
public:
	bool isOperand(char a);//check is operand or operator
	bool isOperator(char b);
	void intoPost(string c);//transform into postfix
	void print();//print out postfix store in p
	int evaluate()throw(out_of_range);//calculate the postfix
	bool compare(char e, char f);//compare operator priority
	int calculate(char g, int h, int i);//operator perform
	void scan(string j)throw(logic_error);//scan the input for error
};

int main(int argc, char **argv)
{
	string equation;
	string scan;
	post obj;
	char a;
	ifstream fin(argv[1]);
	getline(fin, equation);

	for (int i = 0; i < equation.length(); i++)
	{
		if (equation[i] == ' ')continue;
		else scan += equation[i];
	}

	try{
		obj.scan(scan);
	}
	catch (logic_error e){
		cout << e.what();
		return 0;
	}
	obj.intoPost(equation);
	obj.print();
	cout << endl;
	try{
		obj.evaluate();
	}
	catch (out_of_range e){
		cout << e.what();
		return 0;
	}

	cout << obj.evaluate();

	fin.close();
	return 0;
}

void post::scan(string a)throw(logic_error)
{
	int error = 0, c1 = 0, c2 = 0, mark = 0;
	if (isOperator(a[0]) || isOperator(a[a.length() - 1])) error = 1;//if operator is in the first or last place in the equation error
	if (a[0] == '-' || a[0] == '(') error = 0;// except '-'

	for (int i = 0; i < a.length(); i++)//scan all for one time
	{
		if (isOperator(a[i]) || a[i] == ' ') c1++;//if all are operator error
		if (isOperator(a[i]) && isOperator(a[i + 1]))error = 1;//if operator show up for two time continuously error
		if (isOperator(a[i]) && a[i + 1] == '(') error = 0;
		if (a[i] == ')' && isOperator(a[i + 1])) error = 0;
		if (isOperator(a[i]) && a[i + 1] == '-') error = 0;
		if (isOperand(a[i]) || a[i] == ' ' || isOperator(a[i]) || a[i] == ')') c2++;
		if (a[i] == '/' && a[i + 1] == '0') error = 1;
		if (a[i] == '(') mark = 1;
		if (a[i] == ')' && mark == 0) error = 1;
		if (a[i] == ')' && mark == 1)mark = 0;
	}

	if (mark == 1) error = 1;
	if (c1 == a.length()) error = 1;
	if (c2 < a.length()) error = 1;
	if (error == 1)
		throw logic_error("illegal input");
}
void post::intoPost(string a)
{
	stack<char> s;
	int check = 0;
	for (int i = 0; i < a.length(); i++){
		if (a[i] == ')')
		{
			while (!s.empty() && s.top() != '(')//pop up the top and move into string p  until ( show up
			{
				p += s.top();
				p += ' ';
				s.pop();
			}
			s.pop();//pop up (
		}
		else if (a[i] == '-' && isOperand(a[i + 1]) && check == 1)//'-'follow with a operand '-'move into p as negative
		{
			s.push('~');//push in input
		}
		else if (isOperator(a[i]))
		{
			while (!s.empty() && compare(s.top(), a[i]))//top priority >input priority pop up
			{
				p += s.top();
				p += ' ';
				s.pop();
			}

			s.push(a[i]);//push in input
			check = 1;
		}
		else if (isOperand(a[i]))//operand or' ' move in p directly
		{
			while (a[i] != ' ' && isOperand(a[i]))
			{
				p += a[i];
				i++;
			}
			i--;
			p += ' ';
			check = 0;
		}
	}
	while (!s.empty())//clear the rest of them in the stack
	{
		p += s.top();
		p += ' ';
		s.pop();
	}
}

bool post::isOperand(char a)
{
	if (a >= '0' && a <= '9')
		return true;
	else
		return false;
}

bool post::isOperator(char b)
{
	if (b == '+' || b == '-' || b == '*' || b == '/' || b == '%' || b == '(' || b == '~')
		return true;
	else
		return false;
}

bool post::compare(char a, char b)
{
	int si = 0, ai = 0;
	if (a == '(') si = 1;
	if (a == '+' || a == '-') si = 2;
	if (a == '*' || a == '/' || a == '%') si = 3;
	if (a == '~')si = 4;

	if (b == ')') ai = 1;
	if (b == '+' || b == '-') ai = 2;
	if (b == '*' || b == '/' || b == '%') ai = 3;
	if (b == '(') ai = 4;
	if (b == '~')ai = 5;
	return si >= ai;
}

void post::print()
{
	cout << p;
}

int post::evaluate()throw(out_of_range)
{
	stack<int> s;

	for (int i = 0; i < p.length(); i++)
	{
		if (p[i] == ' ')//' 'igmore
			continue;
		else if (s.size() >= 2 && isOperator(p[i]))//stack need to have more than 1 value
		{
			int o2 = s.top(); s.pop();
			int o1 = s.top(); s.pop();
			s.push(calculate(p[i], o1, o2));//push the result into stack

			if (o1>0 && o2 > 0 && o1 + o2 < 0) throw out_of_range("overflow");
			if (o1 < 0 && o2<0 && o1 + o2 >0)throw out_of_range("underflow");
			if (o1 < 0 && o2>0 && o1 - o2 >0)throw out_of_range("underflow");
			if (o1 > 0 && o2<0 && o1 - o2 <0)throw out_of_range("underflow");

		}

		else if (isOperand(p[i]))//find positive value
		{
			int number = 0;
			while (i<p.length() && isOperand(p[i]))
			{
				number = number * 10 + (p[i] - '0');
				i++;
			}
			i--;
			if (p[i + 2] == '~')
			{
				number *= -1;
				i += 2;
			}
			s.push(number);

		}
	}
	if (p[p.length() - 2] == '-' && isOperator(p[p.length() - 4]))
		s.top() *= -1;

	return s.top();
}

int post::calculate(char a, int b, int c)
{
	if (a == '+')return b + c;
	else if (a == '-')return b - c;
	else if (a == '*')return b * c;
	else if (a == '/')return b / c;
	else if (a == '%')return b % c;
	else
		cout << "Error";
	return -1;
}