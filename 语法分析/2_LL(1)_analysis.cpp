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
    for (int i = 0; i < vecSymbol.size(); i++) {
        SymbolSet tmpFirstSet;
        // 从符号串的第一个符号开始找 first 集
        firstOfOneSymbol(vecSymbol[i], tmpFirstSet);
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
    // 保存 follow 集之间的更新式
    UpdateMap update;
    // 遍历所有产生式，完成：
    // 1. 解决与 first 集合相关的所有 follow 集合更新
    // 2. 将所有 follow 集之间的相互更新式保存下来
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        vectorRHS rhss = P[(*iter)];
        // 遍历一个非终结符的所有右部产生式
        for (vectorRHS::iterator iterJ = rhss.begin(); iterJ != rhss.end(); iterJ++) {
            RHS oneRHS = *iterJ;
            // 遍历一个右部产生式的所有符号
            for (RHS::iterator iterK = oneRHS.begin(); iterK != oneRHS.end(); iterK++) {
                // 对每一个非终结符，将它后面的符号串的 first 集合赋给它的 follow 集
                if (N.find(*iterK) != N.end()) {
                    SymbolSet tmpFirstSet;
                    std::vector<Symbol> behind = std::vector<Symbol>(iterK + 1, oneRHS.end());
                    firstOfVecSymbol(behind, tmpFirstSet);
                    follow[*iterK].insert(tmpFirstSet.begin(), tmpFirstSet.end());
                }
            }
            // 倒着遍历一个右部产生式的所有符号
            for (RHS::reverse_iterator iterP = oneRHS.rbegin(); iterP != oneRHS.rend(); iterP++) {
                // 如果遇到了终结符，不产生更新式
                if (T.find(*iterP) != T.end())
                    break;
                // 遇到了非终结符
                else
                {
                    // 当前非终结符 ！= 左部时，加入新的更新式：当前左部的 follow 集赋给当前非终结符
                    if (*iterP != *iter)
                        update[*iter].insert(*iterP);
                    // 当前非终结符不能推出空，则不再继续产生更新式，否则需要继续向前扫描
                    SymbolSet tmpFirstSet;
                    firstOfOneSymbol(*iterP, tmpFirstSet);
                    if (tmpFirstSet.find("~") == tmpFirstSet.end())
                        break;
                }
            }
        }
    }
    // 将 $ 符号赋给起始符号的 follow 集
    follow[S].insert("$");
    // 根据更新式不断更新 follow 集
    for (UpdateMap::iterator iter = update.begin(); iter != update.end(); iter++)
        recurUpdFollow(iter->first, update);
    // follow 集去空
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        follow[*iter].erase("~");
    }
}

/// @brief 从a符号开始递归更新 follow 集
void Grammar::recurUpdFollow(Symbol a, UpdateMap& update) {
    UpdateMap::iterator aLoc = update.find(a);
    if (aLoc == update.end())
        return;
    for (SymbolSet::iterator iter = aLoc->second.begin(); iter != aLoc->second.end(); iter++) {
        follow[*iter].insert(follow[a].begin(), follow[a].end());
        recurUpdFollow(*iter, update);
    }
}

/// @brief 构造预测分析表
void Grammar::buildLL1AnaTable() {
    // 遍历所有产生式
    for (SymbolSet::iterator iterLeft = N.begin(); iterLeft != N.end(); iterLeft++) {
        vectorRHS rhss = P[(*iterLeft)];
        // 遍历一个非终结符的所有右部产生式
        for (vectorRHS::iterator iterRhs = rhss.begin(); iterRhs != rhss.end(); iterRhs++) {
            RHS oneRHS = *iterRhs;
            SymbolSet tmpFirstSet;
            firstOfVecSymbol(oneRHS, tmpFirstSet);
            // 对每个终结符号 a∈FIRST(右部)，A->α 放入 M[A,a]
            for (SymbolSet::iterator a = tmpFirstSet.begin(); a != tmpFirstSet.end(); a++) {
                if ((*a) != "~")
                    anaTable[*iterLeft][*a] = oneRHS;
                else {
                    for (SymbolSet::iterator b = follow[*iterLeft].begin(); b != follow[*iterLeft].end(); b++)
                        anaTable[*iterLeft][*b] = oneRHS;
                }
            }
        }
    }
    // 给所有无定义的 M[A,a] 标上错误标识
    SymbolSet rowSet = T;
    rowSet.erase("~");
    rowSet.insert("$");
    for (SymbolSet::iterator line = N.begin(); line != N.end(); line++) {
        for (SymbolSet::iterator row = rowSet.begin(); row != rowSet.end(); row++) {
            if (anaTable[*line].find(*row) == anaTable[*line].end())
                anaTable[*line].insert(std::pair<Symbol, RHS>(*row, { "ERR" }));
        }
    }
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
