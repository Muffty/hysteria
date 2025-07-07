#include "CoreAI/TestClass.h"

TestClass::TestClass() : Value(0) {}

int TestClass::GetValue() const
{
    return Value;
}

void TestClass::SetValue(int val)
{
    Value = val;
}
