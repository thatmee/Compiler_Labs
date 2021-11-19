#include "Grammar.h"

/// @brief LR(1) ��������
void Grammar::LR1Analysis() {

    extensionGrammar();
    buildProjCluster();
    buildLR1AnaTable();

}

/// @brief ������Ŀ�� I �ıհ������� resultI ��
/// @param I ������Ŀ��
/// @param resultI �����Ŀ��
void Grammar::closure(ProjectMap& I, ProjectMap& resultI) {

}

/// @brief ת�ƺ��� go��resultI = go[I, X]
void Grammar::go(ProjectMap I, Symbol X, ProjectMap& resultI) {

}

/// @brief �ع��ķ������ع����ķ�����ʽ���б��
/// @param resultP �ع����ķ�
void Grammar::extensionGrammar() {
    // �� P ������ת��Ϊ�����Ĳ���ʽ�洢�� extensionP �����
    extensionP[0] = ProdSplit("S'", { {S} });
    int number = 1; // ��� 0 Ϊ�ع�������ʼ�� S'
    for (SymbolSet::iterator iterN = N.begin(); iterN != N.end(); iterN++) {
        for (VectorRHS::iterator iterRHS = P[*iterN].begin(); iterRHS != P[*iterN].end(); iterRHS++)
            extensionP[number++] = ProdSplit(*iterN, *iterRHS);
    }
    // �����µ���ʼ�� S'���޸���ص�һЩ����
    N.insert("S'");
    P["S'"] = { {S} };
    S = "S'";
}

/// @brief ���� LR(1) ��Ŀ���淶��
void Grammar::buildProjCluster() {

}

/// @brief ���� LR(1) ������
void Grammar::buildLR1AnaTable() {

}
