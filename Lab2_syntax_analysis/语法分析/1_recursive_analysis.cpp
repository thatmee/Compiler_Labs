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

#include "Grammar.h"

using namespace std;

/// @brief 自顶向下递归调用分析程序
void Grammar::recursiveAnalysis()
{
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "消除左递归后的文法：" << std::endl;
    std::cout << "E -> T E'" << std::endl;
    std::cout << "E' -> + T E' | - T E' | ~" << std::endl;
    std::cout << "T -> F T'" << std::endl;
    std::cout << "T' -> * F T' | / F T' | ~" << std::endl;
    std::cout << "F -> ( E ) | num" << std::endl;
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "请输入要分析的字符串，符号之间使用空格分隔：";
    inputS();
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
    std::cout << "分析过程如下：" << std::endl;
    forwardPointer();
    std::cout << "起始状态：当前字符 " + this->ch + ", 等待归约出 E" << std::endl;
    this->procE();
    std::cout << "归约出 E，识别成功！" << std::endl;
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
    else if (ch == "$" || ch == ")")
        return;
    else
        error(Grammar::ERR_MISSING_OPERATOR);
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
    else if (ch == "num")
    {
        std::cout << "状态 14：当前字符 " + ch + "，使用 F->n 归约，到达终结状态 17，返回上一级递归" << std::endl;
        forwardPointer();
        return;
    }
    else
        error(ERR_MISSING_OBJECT);
}
