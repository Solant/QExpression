#include "testqexpression.h"
#include "../src/qexpression.h"

TestQExpression::TestQExpression(QObject *parent) : QObject(parent) {}

void TestQExpression::emptyString()
{
    QExpression e;
    QCOMPARE(e.eval(), false);
    QCOMPARE(e.error(), QExpression::NoExpression);
}

void TestQExpression::evalAfterEmpty()
{
    QExpression e;
    QCOMPARE(e.eval(), false);
    QCOMPARE(e.error(), QExpression::NoExpression);

    e.setExpression("1+2");
    QVERIFY(e.eval());
    QCOMPARE(e.error(), QExpression::NoError);
    QCOMPARE(e.result(), 3.0);
}

void TestQExpression::unknownIdentifier()
{
    QExpression e("25d + 13");
    QCOMPARE(e.eval(), false);
    QCOMPARE(e.error(), QExpression::UnknownIdentifier);
}

void TestQExpression::unexpectedEnd()
{
    QExpression e("23 + 3 )");
    QCOMPARE(e.eval(), false);
    QCOMPARE(e.error(), QExpression::UnexpectedEnd);

    e.setExpression("23 + ( 3");
    QCOMPARE(e.eval(), false);
    QCOMPARE(e.error(), QExpression::UnexpectedEnd);
}

void TestQExpression::simpleSum()
{
    QExpression e("1 + 2");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 3.0);
}

void TestQExpression::simpleDiv()
{
    QExpression e("10 / 5");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 2.0);
}

void TestQExpression::complexExpression()
{
    QExpression e("3 + 4 * 2 / (1 - 5)^2");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 3.5);
}

void TestQExpression::sampleFunction()
{
    QExpression e("sqrt(4)");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 2.0);
}

void TestQExpression::functionWithThreeParameters()
{
    QExpression e("max_of_three(1, 4, 2)");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 4.0);
}

void TestQExpression::functionWithWrongNumberOfArguments()
{
    QExpression e("max_of_three(1, 2)");
    QVERIFY(!e.eval());
    QCOMPARE(e.error(), QExpression::WrongNumberOfArguments);
}

void TestQExpression::unaryOperator()
{
    QExpression e("1 + (3! / 6)");
    QVERIFY(e.eval());
    QCOMPARE(e.result(), 2.0);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    TestQExpression testObject;
    return QTest::qExec(&testObject, argc, argv);
}
