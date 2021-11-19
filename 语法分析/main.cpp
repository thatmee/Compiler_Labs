#include "Grammar.h"
#include <iostream>

void splitLine();

int main(void)
{
    // 将 DOS 窗口编码格式修改为 UTF-8，支持 ~ 的输出
    //std::cout << "为支持 ~ 的输出，自动将本窗口编码修改为 UTF-8：";
    //system("chcp 65001");
    // 选择语法分析方案
    int n = -1;
    std::cout << "-------------------请选择语法分析方案----------------------" << std::endl;
    std::cout << "            1    递归调用分析【使用给定文法】               " << std::endl;
    std::cout << "            2    LL(1) 分析方法【自行输入文法】             " << std::endl;
    std::cout << "            3    LR(1) 分析方法【自行输入文法】             " << std::endl;
    std::cout << "请输入对应的编号：";
    std::cin >> n;
    Grammar g = Grammar();

    switch (n)
    {
    case 1:
        splitLine();
        std::cout << "给定文法为：" << std::endl;
        g.output();
        g.recursiveAnalysis();
        break;
    case 2:
        splitLine();
        g.input();
        splitLine();
        g.output();
        g.LL1Analysis();
        break;
    case 3:
        splitLine();
        g.input();
        g.LR1Analysis();
        break;
    default:
        break;
    }
    return 0;
}

void splitLine() {
    std::cout << "----------------------------------------------------------" << std::endl;
}
