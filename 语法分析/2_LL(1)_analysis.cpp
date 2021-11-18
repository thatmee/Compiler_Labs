#include "Grammar.h"
#include <iostream>

/// @brief LL(1) 分析程序
void Grammar::LL1Analysis() {
    // rewriteGrammar();
    buildFirst();
    buildFollow();
    buildLL1AnaTable();




    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "请输入要分析的字符串：";
    inputS();

}

/// @brief 构造 first 集合
void Grammar::buildFirst() {
    // 遍历所有的非终结符
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        vectorRHS rhss = P[(*iter)];
        // 遍历一个非终结符的所有右部产生式
        for (int j = 0; j < rhss.size(); j++) {
            RHS oneRHS = rhss[j];
            SymbolSet firstOfOneRHS;
            // 找到当前右部产生式的 first 集合，保存在 firstOfOneRHS 中
            firstOfVecSymbol(oneRHS, firstOfOneRHS);
            // 将当前右部产生式的 first 集合加入到对应左部的 first 集合中
            first[*iter].insert(firstOfOneRHS.begin(), firstOfOneRHS.end());
        }
    }
}

/// @brief 找一连串符号的 first 集合
/// @param std::vector<Symbol> vecSymbol 输入的一连串符号
/// @param SymbolSet& firstSet 结果保存的集合 
void Grammar::firstOfVecSymbol(std::vector<Symbol> vecSymbol, SymbolSet& firstSet) {
    // 遍历符号串的所有符号
    for (int k = 0; k < vecSymbol.size(); k++) {
        SymbolSet tmpFirstSet;
        // 从符号串的第一个符号开始找 first 集
        firstOfOneSymbol(vecSymbol[k], tmpFirstSet);
        firstSet.insert(tmpFirstSet.begin(), tmpFirstSet.end());
        // 如果针对当前符号找到的 first 集里没有 ~，则结束寻找
        if (tmpFirstSet.find("~") == tmpFirstSet.end())
            break;
    }
}

/// @brief 找某一个符号的 first 集合
/// @param a 需要分析 first 集合的符号
/// @param firstSet 结果保存的集合
void Grammar::firstOfOneSymbol(Symbol a, SymbolSet& firstSet) {
    // 要分析的符号是终结符，则其 first 集合是它本身
    if (T.find(a) != T.end()) {
        firstSet.insert(a);
        return;
    }
    // 要分析的符号是非终结符，递归查找 first 集合
    else {
        vectorRHS rhss = P[a];
        for (int i = 0; i < rhss.size(); i++) {
            RHS oneRHS = rhss[i];
            firstOfVecSymbol(oneRHS, firstSet);
        }
    }
}

/// @brief 构造 follow 集合
void Grammar::buildFollow() {

}

/// @brief 构造预测分析表
void Grammar::buildLL1AnaTable() {

}

/// @brief 消除直接左递归
/// @return 1-存在左递归且消除成功
///         0-不存在左递归
int Grammar::removDirectRecur() {
    return 0;
}

/// @brief 提取左公因子
/// @return 1-有左公因子并且已修改成功
///         0-不存在左公因子
int Grammar::extractFactor() {
    return 0;
}

/// @brief 改写文法
void Grammar::rewriteGrammar() {
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "消除直接左递归..." << std::endl;
    if (removDirectRecur())
        std::cout << "消除成功" << std::endl;
    else
        std::cout << "当前文法无左递归" << std::endl;
    std::cout << "提取左公因子..." << std::endl;
    if (extractFactor())
        std::cout << "提取成功" << std::endl;
    else
        std::cout << "当前文法无左公因子" << std::endl;
    std::cout << "当前文法：" << std::endl;
    output();
}
