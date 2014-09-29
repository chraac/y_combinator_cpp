// test_y_combinator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <memory>
#include <iostream>
#include <string>



using namespace std;



template<typename TResult, typename ...TArgs>
class YBuilder

{

private:

    function<TResult(function<TResult(TArgs...)>, TArgs...)> partialLambda;



public:

    YBuilder(function<TResult(function<TResult(TArgs...)>, TArgs...)> _partialLambda)

        :partialLambda(_partialLambda)

    {

    }



    TResult operator()(TArgs ...args)const

    {

        return partialLambda(

            [this](TArgs ...args)

        {

            return this->operator()(args...);

        }, args...);

    }

};



template<typename TMethod>
struct PartialLambdaTypeRetriver

{

    typedef void FunctionType;

    typedef void LambdaType;

    typedef void YBuilderType;

};



template<typename TClass, typename TResult, typename ...TArgs>
struct PartialLambdaTypeRetriver<TResult(__thiscall TClass::*)(function<TResult(TArgs...)>, TArgs...)>

{

    typedef TResult FunctionType(TArgs...);

    typedef TResult LambdaType(function<TResult(TArgs...)>, TArgs...);

    typedef YBuilder<TResult, TArgs...> YBuilderType;

};



template<typename TClass, typename TResult, typename ...TArgs>
struct PartialLambdaTypeRetriver<TResult(__thiscall TClass::*)(function<TResult(TArgs...)>, TArgs...)const>

{

    typedef TResult FunctionType(TArgs...);

    typedef TResult LambdaType(function<TResult(TArgs...)>, TArgs...);

    typedef YBuilder<TResult, TArgs...> YBuilderType;

};



template<typename TLambda>
function<typename PartialLambdaTypeRetriver<decltype(&TLambda::operator())>::FunctionType> Y(TLambda partialLambda)

{

    return typename PartialLambdaTypeRetriver<decltype(&TLambda::operator())>::YBuilderType(partialLambda);

}






auto Fact = [](int n)->int
{
    return n? 
        1: 
        n * Fact(n - 1);
};


template<typename TFunc, typename TRet>
TRet FactMaker()
{
    return [](TFunc procedure)
    {
        return[](int n)
        {
            return n ?
                1 :
                n * procedure(procedure)(n - 1);
        };
    }([](TFunc procedure)
    {
        return[](int n)
        {
            return n ?
                1 :
                n * procedure(procedure)(n - 1);
        };
    });
}



template<typename TFunc, typename TRet>
TRet FactMaker2()
{
    return [](TFunc procedure)
    {
        return[](int n)
        {
            return n ?
                1 :
                n * [](int arg){ return procedure(procedure)(arg); }(n - 1);
        };
    }([](TFunc procedure)
    {
        return[](int n)
        {
            return n ?
                1 :
                n * [](int arg){ return procedure(procedure)(arg); }(n - 1);
        };
    });
}



template<typename TFunc, typename TRet, typename TFuncInner>
TRet FactMaker3()
{
    return [](TFunc procedure)
    {
        return[](TFuncInner func_arg)
        {
            return[](int n)
            {
                return n ?
                    1 :
                    n * func_arg(n - 1);
            };
        }([](int arg){ return procedure(procedure)(arg); });
    }([](TFunc procedure)
    {
        return[](TFuncInner func_arg)
        {
            return[](int n)
            {
                return n ?
                    1 :
                    n * func_arg(n - 1);
            };
        }([](int arg){ return procedure(procedure)(arg); });
    });
}



template<typename TFunc, typename TRet, typename TFuncInner, typename TFake>
TRet FactMaker4()
{
    return[](TFake fake)
    {
        return [](TFunc procedure)
        {
            return fake([](int arg){ return procedure(procedure)(arg); });
        }([](TFunc procedure)
        {
            return fake([](int arg){ return procedure(procedure)(arg); });
        }); 
    }([](TFuncInner func_arg)
    {
        return[](int n)
        {
            return n ?
                1 :
                n * func_arg(n - 1);
        };
    });
}


template <typename TRet>
struct RecursiveFunc 
{
    function<TRet(RecursiveFunc)> o;
};

// template<typename TRet, template ...TArgs>
// struct FuncTraits
// {
// 
// };

template<typename TFake, typename TRet = function<int(int)>>
TRet YC(TFake x)
{
    using TFunc = RecursiveFunc<TRet>;
    TFunc r = 
    {
        [=](TFunc procedure)
        {
            return TRet([=](int arg)
            {
                return x([=](int arg)
                {
                    return procedure.o(procedure)(arg);
                }, arg);
            });
        }
    };

    return r.o(r);
}





int _tmain(int argc, _TCHAR* argv[])
{
//     auto fib = Y([](function<int(int)> self, int index)
//     {
//         return index < 2
//             ? 1
//             : self(index - 1) + self(index - 2);
//     });
// 
// 
//     for (int i = 0; i < 10; i++)
//     {
//         cout << fib(i) << " ";
//     }
//     cout << endl;

    auto fib = YC([](function<int(int)> self, int index)
    {
        return index < 2
            ? 1
            : self(index - 1) + self(index - 2);
    });


    for (int i = 0; i < 10; i++)
    {
        cout << fib(i) << " ";
    }
    cout << endl;

    return 0;
}

