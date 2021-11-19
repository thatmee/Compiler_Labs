/******** 自顶向下递归调用分析程序 *******
 *              【注意】                *
 *      递归调用分析程序与文法形式相关    *
 *        因此本程序仅针对一种文法       *
 ************* 文法如下 *****************
 *           E->E+T|E-T|T              *
 *           T->T*F|T/F|F              *
 *           F->(E)|n                  *
 ********** 消除左递归后文法如下 *********
 *           E->TE'                    *
 *           E'->+TE'|-TE'|~           *
 *           T->FT'                    *
 *           T'->*FT'|/FT'|~           *
 *           F->(E)|n                  *
 ***************************************/

#include <iostream>
#include <string>
#include "Grammar.h"

using namespace std;

/// @brief 自顶向下递归调用分析程序
void Grammar::recursiveAnalysis()
{
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "消除左递归后的文法：" << std::endl;
    std::cout << "E->TE'" << std::endl;
    std::cout << "E'->+TE'|-TE'|~" << std::endl;
    std::cout << "T->FT'" << std::endl;
    std::cout << "T'->*FT'|/FT'|~" << std::endl;
    std::cout << "F->(E)|num" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "请输入要分析的字符串：";
    inputS();
    forwardPointer();
    std::cout << "起始状态：当前字符 " + this->ch + ", 等待归约出 E" << std::endl;
    this->procE();
    std::cout << "归约出 E，识别成功！" << std::endl;
}

/// @brief 指针前移，并更新 ch 中的值
void Grammar::forwardPointer()
{
    pointer++;
    if (pointer < s.size())
        ch = s[pointer];
    else
        ch = "\0";
}

/// @brief 错误处理函数
/// @param errState 传入错误状态码
void Grammar::error(int errState)
{
    switch (errState)
    {
    case Grammar::ERR_MISSING_R_BRACKET:
        std::cout << "错误：括号不匹配" << std::endl;
        break;
    case Grammar::ERR_MISSING_OBJECT:
        std::cout << "错误：缺少运算对象" << std::endl;
        break;
    default:
        std::cout << "错误" << std::endl;
        break;
    }
    exit(0);
}

/// @brief 非终结符 E 的处理程序
void Grammar::procE()
{
    std::cout << "状态 0：当前字符 " + ch + ", 等待归约出 T" << std::endl;
    procT();
    std::cout << "归约出 T，进入状态 1" << std::endl;
    if (ch == "+" || ch == "-")
    {
        std::cout << "状态 1：识别字符 " + ch + ", 进入状态 0" << std::endl;
        forwardPointer();
        procE();
    }
}

/// @brief 非终结符 T 的处理程序
void Grammar::procT()
{
    std::cout << "状态 7：当前字符 " + ch + ", 等待归约出 F" << std::endl;
    procF();
    std::cout << "归约出 F，进入状态 8" << std::endl;
    if (ch == "*" || ch == "/")
    {
        std::cout << "状态 8：识别字符 " + ch + ", 进入状态 7" << std::endl;
        forwardPointer();
        procT();
    }
}

/// @brief 非终结符 F 的处理程序
void Grammar::procF()
{
    if (ch == "(")
    {
        std::cout << "状态 14：识别字符 (，进入状态 15" << std::endl;
        forwardPointer();
        std::cout << "状态 15：当前字符 " + ch + ", 等待归约出 E" << std::endl;
        procE();
        std::cout << "归约出 E，进入状态 16" << std::endl;
        if (ch == ")")
        {
            std::cout << "状态 16：识别字符 " + ch + "，使用 F->(E) 归约，到达终结状态 17，返回上一级递归" << std::endl;
            forwardPointer();
            return;
        }
        else
            error(ERR_MISSING_R_BRACKET);
    }
    else if (ch == "n")
    {
        std::cout << "状态 14：当前字符 " + ch + "，使用 F->n 归约，到达终结状态 17，返回上一级递归" << std::endl;
        forwardPointer();
        return;
    }
    else
        error(ERR_MISSING_OBJECT);
}
