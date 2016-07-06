# QExpression
Simple expression evaluator for Qt framework with customizable operators. <br />
You need something to evaluate expression and because of reasons you don't use QJSEngine or other similar thing? You came to the right neighborhood! 

### Example usage
```cpp
#include

QExpression e("3 + 4 * 2 / ( 1 - 5 ) ^ 2");
e.eval();
double result = e.result();//3.5
```

### Building
 * To run tests run `qmake` and then `make`.
 * To use QExpression just copy `qexpression.cpp` and `qexpression.h` to your project.

## Usage
### Operators
You can define your own operators or change existing ones in OperatorList constructor in `qexpression.cpp` file.
