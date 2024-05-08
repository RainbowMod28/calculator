#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Token stuff
struct token
{
    char kind;       // what kind of token
    double value;    // for numbers: a value

    // constructors
    token(char ch)
      : kind(ch)
      , value(0)
    {
    }
    token(char ch, double val)
      : kind(ch)
      , value(val)
    {
    }
};

class token_stream
{
    bool full;       // is there a token in the buffer?
    token buffer;    // here is where we keep a Token put back using putback()

public:
    token get();            // get a token
    void putback(token);    // put a token back into the token_stream

    token_stream() : full(false), buffer(0) {}
};

// single global instance of the token_stream
token_stream ts;

void token_stream::putback(token t)
{
    if (full)
        throw std::runtime_error("putback() into a full buffer");
    buffer = t;
    full = true;
}

token token_stream::get()    // read a token from the token_stream
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch;
    std::cin >> ch;

    switch (ch)
    {
    case '(':
    case ')':
    case ';':
    case 'q':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
        return token(ch);
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        std::cin.putback(ch);
        double val;
        std::cin >> val;
        return token('8', val);
    }
    default:
        throw std::runtime_error("Bad token");
    }
}

// declaration so that primary() can call expression()
double expression();

double primary()    // Number or ‘(‘ Expression ‘)’
{
    token t = ts.get();
    switch (t.kind)
    {
    case '(':    // handle ‘(’expression ‘)’
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')')
            throw std::runtime_error("')' expected");
        return d;
    }
    case '8':    // we use ‘8’ to represent the “kind” of a number
        return t.value;    // return the number’s value
    case '-':
        return -primary(); // handle negative numbers
    default:
        throw std::runtime_error("primary expected");
    }
}

// exactly like expression(), but for * and / and %
double term()
{
    double left = primary();    // get the Primary
    while (true)
    {
        token t = ts.get();    // get the next Token ...
        switch (t.kind)
        {
        case '*':
            left *= primary();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0)
                throw std::runtime_error("divide by zero");
            left /= d;
            break;
        }
        case '%':
        {
            double d = primary();
            if (d == 0)
                throw std::runtime_error("divide by zero");
            left = fmod(left, d); // calculate remainder using fmod function
            break;
        }
        default:
            ts.putback(t); // put back the unused token
            return left;   // return the value
        }
    }
}

// read and evaluate: 1   1+2.5   1+2+3.14  etc.
// 	 return the sum (or difference)
double expression()
{
    double left = term();    // get the Term
    while (true)
    {
        token t = ts.get();    // get the next token…
        switch (t.kind)        // ... and do the right thing with it
        {
        case '+':
            left += term();
            break;
        case '-':
            left -= term();
            break;
        default:
            ts.putback(t); // put back the unused token
            return left;   // return the value of the expression
        }
    }
}

std::unordered_map<std::string, double> variables; // map to store variables and their values

double get_value(std::string s)
{
    if (variables.find(s) != variables.end())
        return variables[s];
    else
        throw std::runtime_error("Undefined variable: " + s);
}

void set_value(std::string s, double d)
{
    variables[s] = d;
}

double statement()
{
    token t = ts.get();
    if (t.kind == '8') // number
    {
        token var = ts.get(); // variable name
        if (var.kind != '=')
            throw std::runtime_error("= expected after number");
        double d = expression();
        set_value(std::string(1, t.kind), d); // store the value in the variable
        return d;
    }
    else if (t.kind == 'q') // quit
    {
        exit(0);
    }
    else if (t.kind == ';') // ignore new lines
    {
        return 0;
    }
    else
    {
        ts.putback(t);
        return expression();
    }
}

int main()
try
{
    while (std::cin)
    {
        std::cout << ">> ";
        statement();
        std::cout << '\n';
    }
    return 0;
}
catch (std::runtime_error &e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "exception \n";
    return 2;
}

