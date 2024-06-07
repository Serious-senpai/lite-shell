#include "environment.hpp"

namespace liteshell
{
    Environment::Environment() {}

    Environment *Environment::set_value(const std::string &name, const std::string &value)
    {
        _variables[name] = value;
        return this;
    }

    std::string Environment::get_value(const std::string &name) const
    {
        auto iter = _variables.find(name);
        if (iter == _variables.end())
        {
            return "";
        }
        return iter->second;
    }

    std::map<std::string, std::string> Environment::get_values() const
    {
        return _variables;
    }

    std::string Environment::resolve(const std::string &message) const
    {
        auto result = message;
        while (true)
        {
            std::set<std::string> _variables;
            for (auto begin = result.begin(); begin != result.end(); begin++)
            {
                boost::smatch match;
                bool resolved = boost::regex_search(result, match, _var_resolver);
                if (resolved)
                {
                    auto group_1 = utils::strip(match.str(1)), group_2 = utils::strip(match.str(2));
                    _variables.insert(group_1.empty() ? group_2 : group_1);
                }
            }

            if (_variables.empty())
            {
                break;
            }
            else
            {
                for (auto &variable : _variables)
                {
                    auto pattern = boost::regex(utils::format("(?<!\\$)\\$(?:\\{%s\\}|%s)", variable.c_str(), variable.c_str()));
                    result = boost::regex_replace(result, pattern, utils::regex_escape(get_value(variable)));
                }
            }
        }

        result = boost::regex_replace(result, _escape_resolver, "$");
        return result;
    }

    long long Environment::eval_ll(const std::string &expression) const
    {
        for (auto &c : expression)
        {
            if (!utils::is_math_symbol(c))
            {
                throw std::runtime_error(utils::format("Unrecognized symbol: %c", c));
            }
        }

        // https://cp-algorithms.com/string/expression_parsing.html

        std::stack<long long> st;
        std::stack<char> op;

        auto priority = [](char op)
        {
            if (op < 0) // unary operator
                return 3;
            if (op == '+' || op == '-')
                return 1;
            if (op == '*' || op == '/' || op == '%')
                return 2;

            return -1;
        };

        auto process_op = [&st](char op)
        {
            if (op < 0)
            {
                if (st.empty())
                {
                    throw std::runtime_error("Invalid expression at unary operator");
                }

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
                default:
                    throw std::runtime_error(utils::format("Invalid expression - unknown unary operator %c", op));
                }
            }
            else
            {
                if (st.empty())
                {
                    throw std::runtime_error("Invalid expression at binary operator");
                }
                long long r = st.top();
                st.pop();

                if (st.empty())
                {
                    throw std::runtime_error("Invalid expression at binary operator");
                }
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
                    if (r == 0)
                    {
                        throw std::runtime_error("Invalid expression - division by zero");
                    }

                    st.push(l / r);
                    break;
                case '%':
                    if (r == 0)
                    {
                        throw std::runtime_error("Invalid expression - division by zero");
                    }

                    st.push(l % r);
                    break;
                default:
                    throw std::runtime_error(utils::format("Invalid expression - unknown binary operator %c", op));
                }
            }
        };

        bool may_be_unary = true;
        for (std::size_t i = 0; i < expression.size(); i++)
        {
            if (expression[i] == ' ')
            {
                continue;
            }

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

                    if (op.empty())
                    {
                        throw std::runtime_error("Invalid expression - missing bracket");
                    }
                }

                op.pop();
                may_be_unary = false;
            }
            else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '%')
            {
                char cur_op = expression[i];
                if (may_be_unary && (cur_op == '+' || cur_op == '-'))
                {
                    cur_op = -cur_op;
                }
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
                while (i < expression.size() && expression[i] >= '0' && expression[i] <= '9')
                {
                    number = number * 10 + expression[i++] - '0';
                }
                i--;

                st.push(number);
                may_be_unary = false;
            }
        }

        while (!op.empty())
        {
            process_op(op.top());
            op.pop();
        }

        return st.empty() ? 0 : st.top();
    }
}
