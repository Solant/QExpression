#include "qexpression.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStack>
#include <QStringBuilder>
#include <cmath>

bool isNumber(const QString &arg);

OperatorList QExpression::m_operators;

QExpression::QExpression() {}

QExpression::QExpression(const QString &expression)
{
    m_expression = expression;
}

bool QExpression::eval()
{
    if (m_expression.isEmpty()) {
        m_error = NoExpression;
        return false;
    }
    if (!toReversePolishNotation()) {
        return false;
    }
    QStringList operands = m_reversePolishNotation.split(" ");
    QStack<double> stack;
    Q_FOREACH (const QString &operand, operands) {
        if (m_operators.hasOperator(operand)) {
            stack.push(m_operators.function(operand)(stack.pop(), stack.pop()));
        } else {
            stack.push(operand.toDouble());
        }
    }
    m_result = stack.pop();
    m_error = NoError;
    return true;
}

double QExpression::result() { return m_result; }

QExpression::EvaluationError QExpression::error() { return m_error; }

void QExpression::setExpression(const QString &expression)
{
    m_expression = expression;
}

bool QExpression::toReversePolishNotation()
{
    QString result;
    result.reserve(m_expression.size());
    QStringList symbols = m_expression.split(" ");
    QStack<QString> stack;
    Q_FOREACH (const QString &symbol, symbols) {
        if (symbol == "(") {
            stack.push(symbol);
            continue;
        }
        if (symbol == ")") {
            bool openBracketFound = false;
            while (!stack.isEmpty()) {
                QString el = stack.pop();
                if (el != "(") {
                    result += el + " ";
                } else {
                    openBracketFound = true;
                    break;
                }
            }
            if (!openBracketFound) {
                m_error = UnexpectedEnd;
                return false;
            }
            continue;
        }
        // is operator
        if (m_operators.hasOperator(symbol)) {
            if (stack.isEmpty()) {
                stack.push(symbol);
                continue;
            } else {
                int currentOperatorPriority = m_operators.priority(symbol);
                while (currentOperatorPriority
                       <= m_operators.priority(stack.top())) {
                    result += stack.pop() + " ";
                }
                stack.push(symbol);
                continue;
            }
        }
        // Is number
        bool ok;
        symbol.toDouble(&ok);
        if (ok) {
            result += symbol + " ";
            continue;
        }
        m_error = UnknownIdentifier;
        return false;
    }
    while (!stack.isEmpty()) {
        QString symbol = stack.pop();
        if (symbol == "(" || symbol == ")") {
            m_error = UnexpectedEnd;
            return false;
        }
        result += symbol + " ";
    }
    result.remove(result.length() - 1, 1);
    m_reversePolishNotation = result;
    return true;
}

bool isNumber(const QString &arg)
{
    const QRegularExpression re("[0-9]+");
    return re.match(arg).hasMatch();
}

OperatorList::OperatorList()
{
    // To add custom operator just create it here, and then put it to the list.
    Operator sum = {"+", 1, [](double a, double b) { return a + b; }};
    Operator diff = {"-", 1, [](double a, double b) { return a - b; }};
    Operator mul = {"*", 2, [](double a, double b) { return a * b; }};
    Operator div = {"/", 2, [](double a, double b) { return a / b; }};
    Operator exp = {"^", 3, [](double a, double b) { return pow(a, b); }};

    // Here is the trick: we treat brackets as operators for simplification of
    // toReverseNotation() method, they will have lowest priority and no
    // function to run.
    Operator openBracket = {"(", 0, 0};
    Operator closeBracket = {")", 0, 0};

    m_operatorList.append(sum);
    m_operatorList.append(diff);
    m_operatorList.append(mul);
    m_operatorList.append(div);
    m_operatorList.append(openBracket);
    m_operatorList.append(closeBracket);
    m_operatorList.append(exp);
}

int OperatorList::priority(const QString &signature)
{
    Q_FOREACH (const Operator &op, m_operatorList) {
        if (op.signature == signature)
            return op.priority;
    }
    return -1;
}

bool OperatorList::hasOperator(const QString &signature)
{
    Q_FOREACH (const Operator &op, m_operatorList) {
        if (op.signature == signature)
            return true;
    }
    return false;
}

FunctionCall OperatorList::function(const QString &signature)
{
    Q_FOREACH (const Operator &op, m_operatorList) {
        if (op.signature == signature)
            return op.function;
    }
    return Q_NULLPTR;
}
