#include "qexpression.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStack>
#include <QStringBuilder>
#include <cmath>

bool isNumber(const QString &arg);

FunctionList QExpression::m_functions;

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
        if (m_functions.hasOperator(operand)) {
            const BaseFunction *op = m_functions.op(operand);
            if (stack.size() < op->numberOfArguments()) {
                m_error = WrongNumberOfArguments;
                return false;
            }
            switch (op->type()) {
            case BaseFunction::BinaryOperator: {
                const BinaryOperator *bo
                    = static_cast<const BinaryOperator *>(op);
                double value = bo->function()(stack.pop(), stack.pop());
                stack.push(value);
                break;
            }
            case BaseFunction::Function: {
                const Function *function = static_cast<const Function *>(op);
                QList<double> params;
                for (int i = 0; i < function->numberOfArguments(); i++) {
                    params.append(stack.pop());
                }
                double value = function->function()(params);
                stack.push(value);
            }
            default:
                break;
            }
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
    QStringList symbols = m_expression.split(QRegularExpression("[ ,]+"));
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
        if (m_functions.hasOperator(symbol)) {
            if (stack.isEmpty()) {
                stack.push(symbol);
                continue;
            } else {
                int currentOperatorPriority = m_functions.priority(symbol);
                while (currentOperatorPriority
                       <= m_functions.priority(stack.top())) {
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

FunctionList::FunctionList()
{
    // To add custom operator just create it here, and then put it to the list.
    BinaryOperator *sum
        = new BinaryOperator("+", 1, [](double a, double b) { return a + b; });
    BinaryOperator *diff
        = new BinaryOperator("-", 1, [](double a, double b) { return a - b; });
    BinaryOperator *mul
        = new BinaryOperator("*", 2, [](double a, double b) { return a * b; });
    BinaryOperator *div
        = new BinaryOperator("/", 2, [](double a, double b) { return a / b; });
    BinaryOperator *exp = new BinaryOperator(
        "^", 3, [](double a, double b) { return pow(a, b); });

    // Here is the trick: we treat brackets as operators for simplification of
    // toReverseNotation() method, they will have lowest priority and no
    // function to run.
    BinaryOperator *openBracket = new BinaryOperator("(", 0, Q_NULLPTR);
    BinaryOperator *closeBracket = new BinaryOperator(")", 0, Q_NULLPTR);

    Function *sqrtFunc = new Function("sqrt", 1, [](const QList<double> &list) {
        return sqrt(list.first());
    });
    // Can be deleted, used in test scenario.
    Function *maxOfThree
        = new Function("max_of_three", 3, [](const QList<double> &list) {
              return qMax(qMax(list[0], list[1]), list[2]);
          });

    m_functionList.append(sum);
    m_functionList.append(diff);
    m_functionList.append(mul);
    m_functionList.append(div);
    m_functionList.append(openBracket);
    m_functionList.append(closeBracket);
    m_functionList.append(exp);
    m_functionList.append(sqrtFunc);
    m_functionList.append(maxOfThree);
}

FunctionList::~FunctionList() { qDeleteAll(m_functionList); }

const BaseFunction *FunctionList::op(const QString &signature)
{
    Q_FOREACH (const BaseFunction *op, m_functionList) {
        if (op->name() == signature)
            return op;
    }
    return Q_NULLPTR;
}

int FunctionList::priority(const QString &signature)
{
    Q_FOREACH (const BaseFunction *op, m_functionList) {
        if (op->name() == signature)
            return op->priority();
    }
    return -1;
}

bool FunctionList::hasOperator(const QString &signature)
{
    Q_FOREACH (const BaseFunction *op, m_functionList) {
        if (op->name() == signature)
            return true;
    }
    return false;
}

int BaseFunction::numberOfArguments() const { return m_numberOfArguments; }

BaseFunction::FunctionType BaseFunction::type() const { return m_type; }

QString BaseFunction::name() const { return m_name; }

BaseFunction::BaseFunction(const int &priority, const int &numberOfArguments,
                           const BaseFunction::FunctionType &type,
                           const QString &name)
    : m_priority(priority), m_numberOfArguments(numberOfArguments),
      m_type(type), m_name(name)
{
}

int BaseFunction::priority() const { return m_priority; }

BinaryOperator::BinaryOperator(const QString &name, const int priority,
                               const BinaryOperatorFunction function)
    : BaseFunction(priority, 2, FunctionType::BinaryOperator, name),
      m_function(function)
{
}

BinaryOperatorFunction BinaryOperator::function() const { return m_function; }

Function::Function(const QString &name, const int numberOfArguments,
                   const CustomFunction function)
    : BaseFunction(1, numberOfArguments, BaseFunction::Function, name),
      m_function(function)
{
}

CustomFunction Function::function() const { return m_function; }
