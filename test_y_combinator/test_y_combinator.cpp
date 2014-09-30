// test_y_combinator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <memory>
#include <iostream>
#include <string>

using namespace std;



auto Fact = [](int n)->int
{
    return n? 
        1: 
        n * Fact(n - 1);
};


template<typename TFunc, typename TInnerRet>
TInnerRet FactMaker()
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



template<typename TFunc, typename TInnerRet>
TInnerRet FactMaker2()
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



template<typename TFunc, typename TInnerRet, typename TFuncInner>
TInnerRet FactMaker3()
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



template<typename TFunc, typename TInnerRet, typename TFuncInner, typename TFake>
TInnerRet FactMaker4()
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


template <typename TInnerRet>
struct RecursiveFunc 
{
    function<TInnerRet(RecursiveFunc)> o;
};

template<typename TInnerRet, typename ...TArgs, typename TRet = function<TInnerRet(TArgs...)>>
TRet yc_impl(function<TInnerRet(TRet, TArgs...)> x)
{
    using TFunc = RecursiveFunc < TRet >;
    TFunc r =
    {
        [=](TFunc procedure)
        {
            return TRet([=](TArgs ...args)
            {
                return x([=](TArgs ...args)
                {
                    return procedure.o(procedure)(args...);
                }, args...);
            });
        }
    };

    return r.o(r);
}


template<typename T>
struct YcTypeTraits
{
    typedef void TFunc;
    typedef void TReturn;
};

template<typename TInnerRet, typename TClass, typename ...TArgs>
struct YcTypeTraits< TInnerRet(TClass::*)(function<TInnerRet(TArgs...)>, TArgs...)const >
{
    typedef function<TInnerRet(function<TInnerRet(TArgs...)>, TArgs...)> TFunc;
    typedef function<TInnerRet(TArgs...)> TReturn;
};

template<typename TInnerRet, typename TClass, typename ...TArgs>
struct YcTypeTraits < TInnerRet(TClass::*)(function<TInnerRet(TArgs...)>, TArgs...) >
{
    typedef function<TInnerRet(function<TInnerRet(TArgs...)>, TArgs...)> TFunc;
    typedef function<TInnerRet(TArgs...)> TReturn;
};

template<typename TLambda>
using TraitsType = YcTypeTraits < decltype(&TLambda::operator()) > ;

template<typename TLambda>
typename TraitsType<TLambda>::TReturn yc(TLambda x)
{
    return yc_impl(TraitsType<TLambda>::TFunc(x));
}



int _tmain(int argc, _TCHAR* argv[])
{
    auto fib = yc([](function<int(int)> self, int index)
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


    auto fac = yc([](function<int(int)> self, int index)
    {
        return index < 2
            ? 1
            : index * self(index - 1);
    });

    for (int i = 0; i < 10; i++)
    {
        cout << fac(i) << " ";
    }
    cout << endl;


    auto min = yc([](function<int(int, int)> self, int left, int right)
    {
        return left > 0 && right > 0
            ? self(left - 1, right - 1) + 1
            : 0;
    });

    cout << min(9, 5) << endl;
    cout << min(5, 9) << endl;


    auto min_triple = yc([](function<int(int, int, int)> self, int param1, int param2, int param3)
    {
        return param1 > 0 && param2 > 0 && param3 > 0
            ? self(param1 - 1, param2 - 1, param3 - 1) + 1
            : 0;
    });

    cout << min_triple(9, 5, 4) << endl;
    cout << min_triple(5, 9, 4) << endl;

    return 0;
}
