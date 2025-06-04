#include"../include/Lexer.h"
#include"../include/Parser.h"
#include"../include/RTLILGen.h"
#include"../include/rtlil_to_dot.h"
#include"../include/Optimization.h"
#include"../include/ILP.h"
#include"../include/Node.h"
#include"../include/ML_RCS.h"
#include"../include/MR_LCS.h"
#include"../include/Blif.h"
#include <fstream>
using namespace std;
map<string, int> ComsumingTime = { {"and", 2},{"or", 3},{"not", 1} };

std::string  readFileAsString(const std::string& filename)
{
    std::ifstream file(filename);             // 打开文件
    if (!file) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();                   // 读取整个文件流到缓冲区
    return buffer.str();                      // 返回字符串
    file.close();
    std::cout << "成功转为字符串" << std::endl;
}
int main(){

    std::string VerilogInput = "VerilogInput.txt";
    std::string VerilogInputString = readFileAsString(VerilogInput);

 /*   Lexer lexer("module carry(a, b, c, d, e, cout);  input a, b, c;  output cout;  assign cout = a - b + d + e - c; endmodule");*/
    Lexer lexer(VerilogInputString);
    //生成语法树
     Parser parser(lexer);
    AstNode* node=parser.parseProgram();

    //结构优化
   /* Optimization opt = Optimization(node);
    opt.optimize_in_Ast();*/

    //生成中间表示，再进行优化
    RTLILGen rtlil(node);
    std::string result = rtlil.generateRTLIL();
 /*   opt.set_rtlil_code(result);
    opt.optimize_in_rtlil();
    result = opt.get_optimized_rtlil();*/
    std::cout << result << std::endl;
    node->print(0);



    std::ofstream outFile("RTLILoutput.txt");  // 打开文件（默认 trunc 清空）
    if (outFile.is_open()) {
        outFile << result;
  
        outFile.close();
    }
    else {
        std::cerr << "无法打开文件写入。\n";
    }


    std::string RTLILinput = "RTLILoutput.txt";
    std::string RTLILInputString=readFileAsString(RTLILinput);


    //生成.dot文件
    Lexer lexer1(RTLILInputString);
    RtlilTrans RTLILTODOT(lexer1);
    RTLILTODOT.read_file();
    RTLILTODOT.Generate_DOT();

    //生成blif文件
    RTLILTODOT.genrate_blif();

    cout << "5" << endl;
   ILP ilp;
  std::string filename="blifoutput.txt";
  ilp.set_delay_map({{"AND",1},{"OR",1},{"NOT",1}});
  ilp.read_blif(filename);
  std::map<std::string,int> constraints;
  constraints["AND"]=2;
  constraints["OR"]=1;
  constraints["NOT"]=2;
  ilp.make_constraints(constraints);



  string Path = "blifoutput.txt";
  ifstream blifInformation(Path);
  //这里有问题,需要修改
  cout << "2" << endl;
  Netlist* netlist = ReadBlif(blifInformation);
  cout << "3" << endl;

  // ML_RCS算法
  unordered_map<string, int>Resouces = { {"and",2},{"or",1},{"not",1} };

  int delta = ML_RCS_Schedule(netlist->NetlistGates, Resouces);

  PrintSchedule(netlist);
  cout << "Minimize Cycle: " << delta + 1 << endl;

  // 重置网表状态
  for (auto& GatePair : netlist->NetlistGates) {
      GatePair.second->IsScheduled = false;
      GatePair.second->IsWorking = false;
      GatePair.second->WorkingTimeLength = 0;
      GatePair.second->level = -1;
  }

  // MR_LCS算法
  int DeadlineCycle = delta + 1; // 使用ML_RCS的结果作为延迟约束
  unordered_map<string, int> MinResources = MR_LCS_Schedule(netlist->NetlistGates, DeadlineCycle);

  if (MinResources.empty()) {
      cout << "无法在给定的延迟约束内完成调度！" << endl;
  }
  else {
      PrintSchedule(netlist);
      cout << "在延迟约束 " << DeadlineCycle << " 内的最小资源需求：" << endl;
      for (auto& resource : MinResources) {
          cout << resource.first << ": " << resource.second << endl;
      }
  }

  system("pause");
  delete netlist;
    return 0;

}