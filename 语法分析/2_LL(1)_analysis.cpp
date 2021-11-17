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
            for (int k = 0; k < oneRHS.size(); k++) {
                SymbolSet tmpFirstSet;
                // �Ӳ���ʽ�ĵ�һ�����ſ�ʼ�� first ��
                findFirst(oneRHS[k], tmpFirstSet);
                firstOfOneRHS.insert(tmpFirstSet.begin(), tmpFirstSet.end());
                // �����Ե�ǰ�����ҵ��� first ����û�� ~�������Ѱ�� 
                if (tmpFirstSet.find("~") == tmpFirstSet.end())
                    break;
            }
            first[*iter].insert(firstOfOneRHS.begin(), firstOfOneRHS.end());
        }
    }
}

/// @brief ��ĳһ�����ŵ� first ����
void Grammar::findFirst(Symbol a, SymbolSet& firstSet) {
    if (T.find(a) != T.end()) {
        firstSet.insert(a);
        return;
    }
    else {
        vectorRHS rhss = P[a];
        for (int i = 0; i < rhss.size(); i++) {
            RHS oneRHS = rhss[i];
            findFirst(oneRHS[0], firstSet);
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
