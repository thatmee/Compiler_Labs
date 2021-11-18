#include "Grammar.h"
#include <iostream>

/// @brief LL(1) ��������
void Grammar::LL1Analysis() {
    // rewriteGrammar();
    buildFirst();
    buildFollow();
    buildLL1AnaTable();




    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "������Ҫ�������ַ�����";
    inputS();

}

/// @brief ���� first ����
void Grammar::buildFirst() {
    // �������еķ��ս��
    for (SymbolSet::iterator iter = N.begin(); iter != N.end(); iter++) {
        vectorRHS rhss = P[(*iter)];
        // ����һ�����ս���������Ҳ�����ʽ
        for (int j = 0; j < rhss.size(); j++) {
            RHS oneRHS = rhss[j];
            SymbolSet firstOfOneRHS;
            // �ҵ���ǰ�Ҳ�����ʽ�� first ���ϣ������� firstOfOneRHS ��
            firstOfVecSymbol(oneRHS, firstOfOneRHS);
            // ����ǰ�Ҳ�����ʽ�� first ���ϼ��뵽��Ӧ�󲿵� first ������
            first[*iter].insert(firstOfOneRHS.begin(), firstOfOneRHS.end());
        }
    }
}

/// @brief ��һ�������ŵ� first ����
/// @param std::vector<Symbol> vecSymbol �����һ��������
/// @param SymbolSet& firstSet �������ļ��� 
void Grammar::firstOfVecSymbol(std::vector<Symbol> vecSymbol, SymbolSet& firstSet) {
    // �������Ŵ������з���
    for (int k = 0; k < vecSymbol.size(); k++) {
        SymbolSet tmpFirstSet;
        // �ӷ��Ŵ��ĵ�һ�����ſ�ʼ�� first ��
        firstOfOneSymbol(vecSymbol[k], tmpFirstSet);
        firstSet.insert(tmpFirstSet.begin(), tmpFirstSet.end());
        // �����Ե�ǰ�����ҵ��� first ����û�� ~�������Ѱ��
        if (tmpFirstSet.find("~") == tmpFirstSet.end())
            break;
    }
}

/// @brief ��ĳһ�����ŵ� first ����
/// @param a ��Ҫ���� first ���ϵķ���
/// @param firstSet �������ļ���
void Grammar::firstOfOneSymbol(Symbol a, SymbolSet& firstSet) {
    // Ҫ�����ķ������ս�������� first ������������
    if (T.find(a) != T.end()) {
        firstSet.insert(a);
        return;
    }
    // Ҫ�����ķ����Ƿ��ս�����ݹ���� first ����
    else {
        vectorRHS rhss = P[a];
        for (int i = 0; i < rhss.size(); i++) {
            RHS oneRHS = rhss[i];
            firstOfVecSymbol(oneRHS, firstSet);
        }
    }
}

/// @brief ���� follow ����
void Grammar::buildFollow() {

}

/// @brief ����Ԥ�������
void Grammar::buildLL1AnaTable() {

}

/// @brief ����ֱ����ݹ�
/// @return 1-������ݹ��������ɹ�
///         0-��������ݹ�
int Grammar::removDirectRecur() {
    return 0;
}

/// @brief ��ȡ������
/// @return 1-�������Ӳ������޸ĳɹ�
///         0-������������
int Grammar::extractFactor() {
    return 0;
}

/// @brief ��д�ķ�
void Grammar::rewriteGrammar() {
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "����ֱ����ݹ�..." << std::endl;
    if (removDirectRecur())
        std::cout << "�����ɹ�" << std::endl;
    else
        std::cout << "��ǰ�ķ�����ݹ�" << std::endl;
    std::cout << "��ȡ������..." << std::endl;
    if (extractFactor())
        std::cout << "��ȡ�ɹ�" << std::endl;
    else
        std::cout << "��ǰ�ķ���������" << std::endl;
    std::cout << "��ǰ�ķ���" << std::endl;
    output();
}
