#pragma once

#include "standard.hpp"

class Environment
{
private:
    std::map<std::string, std::string> variables;

public:
    Environment *set_variable(const std::string &name, const std::string &value)
    {
        variables[name] = value;
        return this;
    }

    std::string get_value(const std::string &name) const
    {
        auto iter = variables.find(name);
        if (iter == variables.end())
        {
            return "";
        }
        return iter->second;
    }

    long long eval_ll(const std::string &expression)
    {
        // https://cp-algorithms.com/string/expression_parsing.html

        std::stack<long long> st;
        std::stack<char> op;

        auto delim = [](char c)
        {
            return c == ' ';
        };

        auto is_op = [](char c)
        {
            return c == '+' || c == '-' || c == '*' || c == '/';
        };

        auto is_unary = [](char c)
        {
            return c == '+' || c == '-';
        };

        auto priority = [](char op)
        {
            if (op < 0) // unary operator
                return 3;
            if (op == '+' || op == '-')
                return 1;
            if (op == '*' || op == '/')
                return 2;
            return -1;
        };

        auto process_op = [&st](char op)
        {
            if (op < 0)
            {
                long long l = st.top();
                st.pop();
                switch (-op)
                {
                case '+':
                    st.push(l);
                    break;
                case '-':
                    st.push(-l);
                    break;
                }
            }
            else
            {
                long long r = st.top();
                st.pop();
                long long l = st.top();
                st.pop();
                switch (op)
                {
                case '+':
                    st.push(l + r);
                    break;
                case '-':
                    st.push(l - r);
                    break;
                case '*':
                    st.push(l * r);
                    break;
                case '/':
                    st.push(l / r);
                    break;
                }
            }
        };

        bool may_be_unary = true;
        for (unsigned i = 0; i < expression.size(); i++)
        {
            if (delim(expression[i]))
                continue;

            if (expression[i] == '(')
            {
                op.push('(');
                may_be_unary = true;
            }
            else if (expression[i] == ')')
            {
                while (op.top() != '(')
                {
                    process_op(op.top());
                    op.pop();
                }
                op.pop();
                may_be_unary = false;
            }
            else if (is_op(expression[i]))
            {
                char cur_op = expression[i];
                if (may_be_unary && is_unary(cur_op))
                    cur_op = -cur_op;
                while (!op.empty() && ((cur_op >= 0 && priority(op.top()) >= priority(cur_op)) ||
                                       (cur_op < 0 && priority(op.top()) > priority(cur_op))))
                {
                    process_op(op.top());
                    op.pop();
                }
                op.push(cur_op);
                may_be_unary = true;
            }
            else
            {
                long long number = 0;
                while (i < expression.size() && isalnum(expression[i]))
                    number = number * 10 + expression[i++] - '0';
                --i;

                st.push(number);
                may_be_unary = false;
            }
        };

        while (!op.empty())
        {
            process_op(op.top());
            op.pop();
        }
        return st.top();
    }
};