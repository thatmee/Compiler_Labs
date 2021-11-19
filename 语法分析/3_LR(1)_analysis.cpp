#include "Grammar.h"

/// @brief LR(1) 分析程序
void Grammar::LR1Analysis() {

    extensionGrammar();
    buildProjCluster();
    buildLR1AnaTable();

}

/// @brief 构造项目集 I 的闭包，放在 resultI 中
/// @param I 输入项目集
/// @param resultI 结果项目集
void Grammar::closure(ProjectMap& I, ProjectMap& resultI) {

}

/// @brief 转移函数 go，resultI = go[I, X]
void Grammar::go(ProjectMap I, Symbol X, ProjectMap& resultI) {

}

/// @brief 拓广文法并对拓广后的文法产生式进行编号
/// @param resultP 拓广后的文法
void Grammar::extensionGrammar() {
    // 将 P 内数据转换为单独的产生式存储到 extensionP 并编号
    extensionP[0] = ProdSplit("S'", { {S} });
    int number = 1; // 编号 0 为拓广后的新起始符 S'
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        for (VectorRHS::iterator iterRHS = P[*iterN].begin(); iterRHS != P[*iterN].end(); iterRHS++)
            extensionP[number++] = ProdSplit(*iterN, *iterRHS);
    }
    // 加入新的起始符 S'，修改相关的一些变量
    N.insert("S'");
    P["S'"] = { {S} };
    S = "S'";
}

/// @brief 构造 LR(1) 项目集规范簇
void Grammar::buildProjCluster() {

}

/// @brief 构造 LR(1) 分析表
void Grammar::buildLR1AnaTable() {

}
