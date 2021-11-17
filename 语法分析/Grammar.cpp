/**
 * @file Grammar.cpp
 * @author Nanyafeng
 * @brief Grammar 类中构造函数、输出函数的实现
 * @version 1.0
 * @date 2021-11-16
 */

#include "Grammar.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

/// @brief 默认的 Grammar 构造函数
/// @note 默认使用如下的文法：
///       E->E+T|E-T|T
///       T->T*F|T/F|F
///       F->(E)|n
Grammar::Grammar() {
    this->N = { "E", "T", "F" };
    this->T = { "+", "-", "*", "/", "(", ")", "num" };
    this->S = "E";
    this->P["E"] = { { "E", "+", "T" }, { "E", "-", "T" }, { "T" } };
    this->P["T"] = { { "T", "*", "F" }, { "T", "/", "F" }, { "F" } };
    this->P["F"] = { { "(", "E", ")" }, { "num" } };
}

/// @brief 输出文法
void Grammar::output() const {
    for (auto iter1 = this->P.begin(); iter1 != this->P.end(); iter1++) {
        std::cout << iter1->first << "->"; // 输出每一条产生式的左部和推导符号，如：E->
        vectorRHS tmp = iter1->second;
        int len = static_cast<int>(tmp.size()), j = 0;
        for (j = 0; j < len - 1; j++) { // 输出每一条产生式的所有右部产生式，如：E+T|E-T|T
            for (int k = 0; k < tmp[j].size(); k++) { // 输出一个右部产生式，如：E+T
                std::cout << tmp[j][k];
            }
            std::cout << "|";
        }
        for (int k = 0; k < tmp[j].size(); k++) { // 输出最末尾的一个右部产生式
            std::cout << tmp[j][k];
        }
        std::cout << endl;
    }
}

/// @brief 读取用户输入的文法
/// @datails 读取之前会先删除原有的文法
void Grammar::input() {
    // 清空原有文法
    this->N.clear();
    this->T.clear();
    this->P.clear();
    this->S.clear();
    std::string inputLine;

    // 读入非终结符集合
    std::cout << "请输入非终结符集合，以空格分隔：";
    getchar();
    std::getline(cin, inputLine);
    std::vector<Symbol> symbolVec;
    boost::split(symbolVec, inputLine, boost::is_any_of(" "));
    this->N = SymbolSet(symbolVec.begin(), symbolVec.end());

    // 读入起始符号
    std::cout << "请输入文法起始符号：";
    std::cin >> inputLine;
    this->S = inputLine;

    // 读入终结符集合
    std::cout << "请输入终结符集合，以空格分隔：";
    getchar();
    std::getline(cin, inputLine);
    symbolVec.clear();
    boost::split(symbolVec, inputLine, boost::is_any_of(" "));
    this->T = SymbolSet(symbolVec.begin(), symbolVec.end());

    // 读入文法
    std::cout << "请输入文法，要求如下：" << std::endl;
    std::cout << "1. 不只 1 个字符的终结符/非终结符请使用 @ 括起，如 F->@num@" << std::endl;
    std::cout << "2. 请输入没有二义性、不含【间接】左递归的文法" << std::endl;
    std::cout << "3. 在新行中敲入 # 来结束文法输入" << std::endl;
    std::cin >> inputLine;

    // 逐行读入，进行处理
    while (inputLine != "#") {
        Symbol begin = "";
        int i = 0, len = static_cast<int>(inputLine.length());

        // 分离出一行输入的左部符号
        for (i = 0; i < len - 1; i++)
        {
            if (inputLine[i] == '-' && inputLine[i + 1] == '>')
                break;
            begin += inputLine[i];
        }

        // 一行的左部没有出现在非终结符集合中，出现错误，跳过本行的产生式
        if (N.find(begin) == N.end())
        {
            std::cout << "错误，出现了未知的非终结符：" << begin << std::endl;
            exit(0);
            /*std::cin >> inputLine;
            continue;*/
        }

        // 剩下的部分为所有右部产生式
        std::string rightStr = inputLine.substr(i + 2); // i 和 i+1 是 -> 符号，从 i+2 截取右部产生式字符串
        vectorRHS allRHS;

        //将所有右部产生式按照 | 进行分成单独的产生式字符串
        std::vector<std::string> allStrRHS;
        boost::split(allStrRHS, rightStr, boost::is_any_of("|"));

        // 将单独的产生式字符串分割出终结符、非终结符，存入 allRHS
        for (int j = 0; j < allStrRHS.size(); j++) {
            RHS oneRHS;
            std::string oneStrRHS = allStrRHS[j];
            std::string tmpSymbol = "";
            for (int k = 0; k < oneStrRHS.length(); k++) {
                if (oneStrRHS[k] == '@') { // 如果碰到 @ 符号，从这个 @ 到下一个 @，提取出完整的一个非终结符/终结符
                    k++;
                    while (oneStrRHS[k] != '@') {
                        tmpSymbol += oneStrRHS[k];
                        k++;
                    }
                }
                else
                    tmpSymbol = std::string(1, oneStrRHS[k]); // 不是 @ 符号，则单个字符成为非终结符/终结符
                if (tmpSymbol.length() > 0) {// 防止出现 @@ 输入产生的空符号
                    oneRHS.push_back(tmpSymbol);
                    tmpSymbol = "";
                }
            }
            allRHS.push_back(oneRHS);
        }

        // 这一行的左部如果已经存在，那么把右部合并进去，如果不存在则创建新的键值对
        Productions::iterator iter = this->P.find(begin);
        if (iter == P.end()) // 没有找到
            this->P.insert(iter, std::pair<Symbol, vectorRHS>(begin, allRHS));
        else // 找到了
            iter->second.insert(iter->second.end(), allRHS.begin(), allRHS.end());
        std::cin >> inputLine;
    }
}

/// @brief 读取要分析的字符串
void Grammar::inputS() {
    std::string str;
    std::cin >> str;
    int i = 0, len = static_cast<int>(str.size());
    Symbol tmpSymbol = "";
    for (i = 0; i < len; i++) { //逐字符遍历
        tmpSymbol = "";
        if (str[i] == '$') { // 如果碰到 $ 符号，从这个 $ 到下一个 $，提取出完整的一个非终结符/终结符
            i++;
            while (str[i] != '$')
                tmpSymbol += str[i];
        }
        else
            tmpSymbol = std::string(1, str[i]); // 不是 $ 符号，则单个字符成为非终结符/终结符
        if (tmpSymbol.length() > 0) // 防止出现 $$ 输入产生的空符号
            this->s.push_back(tmpSymbol);
    }
}
