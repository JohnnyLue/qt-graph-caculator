#ifndef EQUATION_H
#define EQUATION_H
#include <string>

class Equation
{
public:
    Equation();
    Equation(std::string);
    void setequation(std::string equation);
    float calculateDiff(float x,float y);
    bool isSetVal=false;
    bool isready=false;//the equation is ready to draw
private:
    std::string leftpostfix;//"=" left
    std::string rightpostfix;
    std::string declaredValuable;

};

#endif // EQUATION_H
