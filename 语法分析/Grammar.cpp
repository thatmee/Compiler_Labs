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
        VectorRHS tmp = iter1->second;
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
    char ch = getchar();
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
    ch = getchar();
    std::getline(cin, inputLine);
    symbolVec.clear();
    boost::split(symbolVec, inputLine, boost::is_any_of(" "));
    this->T = SymbolSet(symbolVec.begin(), symbolVec.end());

    // 读入文法
    std::cout << "请输入文法，要求如下：" << std::endl;
    std::cout << "1. 符号之间以空格分隔，如 F -> num | （ E ）" << std::endl;
    std::cout << "2. 请输入没有二义性、不含【间接】左递归的文法" << std::endl;
    std::cout << "3. 在新行中敲入 # 来结束文法输入" << std::endl;
    std::getline(cin, inputLine);

    // 逐行读入，进行处理
    while (inputLine != "#") {
        Symbol begin = "";
        int i = 0, len = static_cast<int>(inputLine.length());

        // 分离出一行输入的左部符号
        for (i = 0; i < len - 1; i++)
        {
            if (inputLine[i] == ' ')
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

        // 跳过箭头
        for (; i < len - 1; i++)
        {
            if (inputLine[i] == '>')
                break;
        }

        // 剩下的部分为所有右部产生式
        std::string rightStr = inputLine.substr(i + 1); // i 是 > 符号，从 i+1 截取右部产生式字符串
        VectorRHS allRHS;

        //将所有右部产生式按照 | 进行分成单独的产生式字符串
        std::vector<std::string> allStrRHS;
        boost::split(allStrRHS, rightStr, boost::is_any_of("|"));

        // 每一个产生式字符串按照空格分隔出终结符、非终结符，存入 allRHS
        for (int j = 0; j < allStrRHS.size(); j++) {
            RHS oneRHS;
            std::string oneStrRHS = allStrRHS[j];
            boost::split(oneRHS, oneStrRHS, boost::is_any_of(" "));
            // 清除 split 函数可能产生的空串
            for (RHS::iterator i = oneRHS.begin(); i != oneRHS.end();) {
                if (*i == "")
                    i = oneRHS.erase(i);
                else
                    i++;
            }
            allRHS.push_back(oneRHS);
        }

        // 这一行的左部如果已经存在，那么把右部合并进去，如果不存在则创建新的键值对
        Productions::iterator iter = this->P.find(begin);
        if (iter == P.end()) // 没有找到
            this->P.insert(iter, std::pair<Symbol, VectorRHS>(begin, allRHS));
        else // 找到了
            iter->second.insert(iter->second.end(), allRHS.begin(), allRHS.end());
        std::getline(cin, inputLine);
    }
}

/// @brief 读取要分析的字符串
void Grammar::inputS() {
    std::string str;
    std::getline(cin, str);
    int i = 0, len = static_cast<int>(str.size());
    boost::split(s, str, boost::is_any_of(" "));
    // 清除 split 函数可能产生的空串
    for (RHS::iterator i = s.begin(); i != s.end();) {
        if (*i == "")
            i = s.erase(i);
        else
            i++;
    }
    this->s.push_back("$");
}
