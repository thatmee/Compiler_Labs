/**
 * @file main.cpp
 * @author NanYafeng
 * @brief main 函数
 * @version 1.0
 * @date 2021-11-21
 *
 */
#include "Grammar.h"

void splitLine();

int main(void)
{
    // 选择语法分析方案
    int run = 1;
    while (run) {
        int n = -1;
        std::cout << "-------------------请选择语法分析方案----------------------" << std::endl;
        std::cout << "            1    递归调用分析【使用给定文法】               " << std::endl;
        std::cout << "            2    LL(1) 分析方法【自行输入文法】             " << std::endl;
        std::cout << "            3    LR(1) 分析方法【自行输入文法】             " << std::endl;
        std::cout << "            4             退出                            " << std::endl;
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
            g.LL1Analysis();
            break;
        case 3:
            splitLine();
            g.input();
            g.LR1Analysis();
            break;
        case 4:
            run = 0;
            break;
        default:
            break;
        }
    }
    system("pause");
    return 0;
}

void splitLine()
{
    std::cout << std::endl
        << setfill('-') << setw(Grammar::SPLIT_LINE_WIDTH) << "" << std::endl;
}
