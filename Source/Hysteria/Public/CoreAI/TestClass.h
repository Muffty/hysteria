#pragma once

class TestClass
{
public:
    TestClass();
    int GetValue() const;
    void SetValue(int val);

private:
    int Value;
};
