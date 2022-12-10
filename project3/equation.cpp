#include "equation.h"
#include <stack>
#include<set>
#include "controler.h"
namespace{

    std::set<std::string> newNames;// store valuable names in the equation(two side)

    bool xyappear=false;//x or y is in equation

    int priority(char o)
    {
        switch(o)
        {
        case '+':case '-':return 2;
        case '*':case '/':return 3;
        case '^':return 4;
        case 'S':case 'C':case 'T':return 5;//sine,cos,tan
        case '(':return 1;
        case ')':return -1;
        default:
            return 0;
        }
    }

    bool checkpair(std::string& test)// () or not paired is false
    {
        int leftcount=0,rightcount=0;
        for(int i=0;i<test.size();i++)
        {

            if(test[i]=='(')
                leftcount++;
            if(test[i]==')')
            {
                if(i==0||test[i-1]=='(')
                    return false;
                rightcount++;
            }

        }
        if(leftcount!=rightcount)
            return false;
        return true;
    }

    bool invalidVal(std::string& test)//true->invalid num or name,false->is num or valid name
    {
        bool oneDot=false,oneMinus=false,hasLetter=false;
        for(int i=0;i<test.size();i++)
        {
            if(test[i]=='-')
            {
                if(oneMinus)
                    return true;
                else
                    oneMinus=true;
            }
            if(test[i]=='.')
            {
                if(oneDot)
                    return true;
                else
                    oneDot=true;
            }
            if(std::isalpha(test[i]))
            {
                hasLetter=true;
            }
        }

        if(oneDot&&hasLetter)
            return true;

        if(oneDot&&test.size()==1)
            return true;

        if(oneMinus&&test.size()==1)
            return true;

        if(hasLetter&&test[oneMinus]>='0'&&test[oneMinus]<='9')//name with digit at first EX:-5ab,0d
            return true;

        return false;
    }

    bool checkInvalid(std::string& test)//maybe check valuables here too//FOR POSTFIX
    {
        if(test.find("()")<test.size())return false;

        int leftcount=0,rightcount=0,operatorcount=0;
        bool outofsis=true;
        std::vector<std::string> vals;//store 'operands' maybe num or valuable names

        for(int i=0;i<test.size();i++)
        {
            if(test[i]=='[')
            {
                bool check;
                int end=i,count=1;
                while(count)
                {
                    if(test[++end]=='[')count++;
                    if(test[end]==']')count--;
                }
                std::string ntest=test.substr(i+1,end-i);
                check=checkInvalid(ntest);
                if(!check)
                    return false;
                vals.back()+='#';//'#'mark if have -[] or S[],-S[]..., and the 'check' prove things in [] is valid.
                i=end+1;//continue after ']'
            }

            if(!outofsis&&test[i]!=')')
            {
                vals.back()+=test[i];
            }

            if(outofsis&&priority(test[i])>=2&&priority(test[i])<=4)
                operatorcount++;

            if(test[i]=='(')
            {
                outofsis=false;
                leftcount++;
                vals.push_back("");
            }
            if(test[i]==')')
            {
                outofsis=true;
                rightcount++;
            }
            if(rightcount-leftcount>1||rightcount-leftcount<-1)//like ((123))
            {
                return false;
            }
        }

        if(leftcount!=rightcount)
            return false;

        if(leftcount!=operatorcount+1)return false;


        for(std::string t:vals)
        {

            if(invalidVal(t))
            {
                return false;//invalid name or num
            }

            if(std::isdigit(t[t.find_first_not_of('-')]))
                continue;//valid num

            if(t.compare("x")==0||t.compare("-x")==0) //x or -x is not valable name
            {
                xyappear=true;
                continue;
            }

            if(t.compare("y")==0||t.compare("-y")==0)
            {
                xyappear=true;
                continue;
            }

            if(t.find('#')<t.size())continue;

            bool inList;
            Controler::getValuable(t.substr(t.find_first_not_of('-')),inList);//check valable name is known or not
            if(!inList)
            {
                newNames.insert(t.substr(t.find_first_not_of('-')));//valid name but not in record
            }
        }

        return true;
    }

    std::string getpostfix(std::string infix)//every sin->S,cos->C,tan->T
    {
        std::stack <char> opstack;//for operators

        std::string postfix="(";//(...)for every operand(valable)

        for(int i=0;i<infix.size();i++)
        {
            int pri=priority(infix[i]);

            if(pri==5)//sin cos tan
            {
                postfix+=infix[i];
                postfix+='[';//like recursive
                int end=i+1,count=1;
                for(;count>0;)
                {
                    if(infix[++end]=='(')count++;
                    if(infix[end]==')')count--;
                }
                postfix+=getpostfix(infix.substr(i+1,end-i));
                postfix+=']';
                opstack.push('(');
                i=end;
            }
            else if(pri>0&&pri<5)//other operators or '('
            {
                bool dont=false;
                if(pri!=1)//not '('
                {
                        if(pri==2&&postfix.back()=='(')//'-'or'+' and is first char
                        {
                            postfix+=infix[i];
                            if(infix[i+1]=='(')
                            {
                                postfix+='[';//like recursive
                                int end=i+1,count=1;
                                for(;count>0;)
                                {
                                    if(infix[++end]=='(')count++;
                                    if(infix[end]==')')count--;
                                }
                                postfix+=getpostfix(infix.substr(i+1,end-i));
                                postfix+=']';
                                opstack.push('(');
                                i=end;
                            }

                            dont=true;//it's part of val
                        }
                        else
                        {
                            if(priority(postfix.back())==0)
                                postfix+=')';

                        }
                    if(!dont)
                    while(!opstack.empty()&&priority(opstack.top())>=pri)
                    {
                        postfix+=opstack.top();
                        opstack.pop();
                    }
                    if(!dont)
                    postfix+='(';
                }
                if(!dont)
                opstack.push(infix[i]);
             }
            else if(pri==-1)
            {

                if(priority(postfix.back())==0)
                    postfix+=')';

                while(!opstack.empty()&&opstack.top()!='(')
                {


                    postfix+=opstack.top();
                    opstack.pop();
                }

                if(opstack.empty())
                    throw(std::exception("Not paired ')'."));

                opstack.pop();

            }else{
                if(isdigit(infix[i])||isalpha(infix[i])||infix[i]=='.')//S C or T won't get to here
                {
                    postfix+=infix[i];
                }
                else
                    throw(std::exception("Invalid symbal"));
            }
        }


        if(priority(postfix.back())==0)
            postfix+=')';

        while(!opstack.empty())
        {
            if(opstack.top()!='(')
                postfix+=opstack.top();
            opstack.pop();
        }

       if(!checkInvalid(postfix))
        {
            throw(std::exception("Invalid input"));
        }

        return postfix;
    }

    float calculate(float x, float y,std::string curOperating)//specials:'-','x'',-x','y','-y','S','-S'...
    {
        std::stack<float>ansStack;

        std::stack<char>opStack;

        std::string tempNum;//temp may be val or name

        bool enterOperand=true;//to recogn '-' is in () or not
        for(int i=0;i<curOperating.size();i++)
        {
            char cur=curOperating[i];

            if(cur=='(')
            {
                tempNum.clear();
                enterOperand=true;
            }
            else if(cur=='[')//into another calculate
            {
                int end=i+1,count=1;
                while(count>0)
                {
                    if(curOperating[++end]=='[')count++;
                    if(curOperating[end]==']')count--;
                }
                float val=calculate(x,y,curOperating.substr(i+1,end-i));// calculate in []

                switch(tempNum.back())//C,S,T
                {
                    case 'C':
                    val=cos(val);
                    break;
                    case 'S':
                    val=sin(val);
                    break;
                    case 'T':
                    val=tan(val);
                    break;
                }

                if(tempNum.front()=='-')
                    val*=-1;

                i=end+1;
                enterOperand=false;

                ansStack.push(val);
            }
            else if(cur==')')
            {
                if(tempNum=="x"||tempNum=="-x")
                {
                    if(tempNum.front()=='-')
                        ansStack.push(-x);
                    else
                        ansStack.push(x);
                }
                else if(tempNum=="y"||tempNum=="-y")
                {
                    if(tempNum.front()=='-')
                        ansStack.push(-y);
                    else
                        ansStack.push(y);
                }
                else//is number
                {
                    float val=std::stod(tempNum);//can handle minus
                    ansStack.push(val);
                }

                enterOperand=false;
            }
            else if(enterOperand)
            {
                tempNum+=cur;
            }
            else if(priority(cur)>1&&priority(cur)<5)//is one of /*-+^
            {
                float num1=ansStack.top();
                ansStack.pop();
                float num2=ansStack.top();
                ansStack.pop();
                float res;
                switch(cur)
                {
                case '+':
                    res=num1+num2;
                    ansStack.push(res);
                    break;
                case '-':
                    res=num2-num1;
                    ansStack.push(res);
                    break;
                case '*':
                    res=num2*num1;
                    ansStack.push(res);
                    break;
                case '/':
                    res=num2/num1;
                    ansStack.push(res);
                    break;
                case '^':
                    res=pow(num2,num1);
                    ansStack.push(res);
                    break;
                }
            }
        }

        if(ansStack.size()!=1)
            return 0;

        return ansStack.top();
    }
}

Equation::Equation()
{
}

Equation::Equation(std::string equation)
{
    setequation(equation);
}


void Equation::setequation(std::string equation)
{
    if(isSetVal&&isready)
    {
        Controler::deleteValuable(declaredValuable);//delete name declared before
    }
    isready=false;
    isSetVal=false;
    newNames.clear();
    xyappear=false;

    if(equation.empty())return;

    equation.erase(std::remove(equation.begin(), equation.end(), ' '), equation.end());//ignore ' '

    for(int i=0;i<equation.size();i++)
        equation[i]=std::tolower(equation[i]);//only lower case allowed

    while(equation.find("sin(")<equation.size())
    {
        equation=equation.replace(equation.find("sin("),4,"S(");
    }
    while(equation.find("cos(")<equation.size())
    {
        equation=equation.replace(equation.find("cos("),4,"C(");
    }
    while(equation.find("tan(")<equation.size())
    {
        equation=equation.replace(equation.find("tan("),4,"T(");
    }

    if(!checkpair(equation))throw(std::exception("Invalid input"));//check paired and no "()"

    //if(equation.find("--")<equation.size())throw(std::exception("Invalid input"));//maybe 1--1 is allowed ->1-(-1)
    if(equation.find("++")<equation.size())throw(std::exception("Invalid input"));
    if(equation.find("**")<equation.size())throw(std::exception("Invalid input"));
    if(equation.find("//")<equation.size())throw(std::exception("Invalid input"));
    if(equation.find("^^")<equation.size())throw(std::exception("Invalid input"));


    for(int i=0;i<Controler::valuables.size();i++)
    {
        for(auto iter=Controler::valuables.rbegin();iter!=Controler::valuables.rend();iter++)
        {
            while(equation.find(iter->first)<equation.size())
            {
                equation.replace(equation.find(iter->first),iter->first.size(),iter->second);
            }
        }
    }


    if(equation.find('=')<equation.size())//two side
    {
        if(equation.find('=')!=equation.rfind('='))//two or more '='
        {
            throw(std::exception("Too many '='"));
        }

        std::string left=equation.substr(0,equation.find('=')),right=equation.substr(equation.find('=')+1);//split into two part

        if(left.empty()||right.empty())
            throw(std::exception("Invalid equation"));

        leftpostfix=getpostfix(left);

        if(priority(leftpostfix.back())<2)//may be set val
        {
           if(leftpostfix.find('-')>=leftpostfix.size())
               if(leftpostfix.find('[')>=leftpostfix.size())
               {
                   if(std::isalpha(leftpostfix[1])&&leftpostfix!="(y)"&&leftpostfix!="(x)")//is set value
                   {
                       isSetVal=true;
                       this->declaredValuable=leftpostfix.substr(1,leftpostfix.size()-2);
                       bool found;
                       Controler::getValuable(declaredValuable,found);
                       if(found)
                       {
                           throw(std::exception("Duplicate definition"));
                       }
                       newNames.clear();//one or no name in it now,but either is ok,so clear to store right side's unknow
                   }
               }
        }

        rightpostfix=getpostfix(right);

        if(newNames.size()>0)//unknown names except delared name of this equation
        {
            throw newNames;//to show what names are unknown
        }

        if(isSetVal)
        {
            Controler::addValuable(declaredValuable,"("+right+")");
        }else
        {
            if(!xyappear)//only values = values ex:1.3=56 or invalid set val like 5=a
            {
                throw(std::exception("Invalid equation"));
            }
        }

    }
    else//one side, left side default "y"
    {
        leftpostfix="(y)";//postfix form
        rightpostfix=getpostfix(equation);
        if(newNames.size()>=1)//unknow names of two side,def invalid, and already 'y' at left side
        {
            throw newNames;//for show what names are not known
        }
    }

    isready=true;//equation is valid otherwise will be 'throw'
}

float Equation::calculateDiff(float x, float y)
{
    if(!isready||isSetVal)
        return 0;

    float left=calculate(x,y,leftpostfix);
    float right=calculate(x,y,rightpostfix);
    return left-right;
}
