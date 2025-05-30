
#include"../include/rtlil_to_dot.h"

RtlilTrans::Cell* RtlilTrans::find_temp(Cell* cell)
{
    for (auto& cell1 : cells)
    {
        if(cell->ports["Y"].starts_with("tmp"))
        {
            if (cell1->ports["A"] == cell->ports["Y"] || cell1->ports["B"] == cell->ports["Y"])
            {
               
                    return cell1;
            }
        }

    
    }
    return nullptr;
}





void RtlilTrans::genrate_blif()
{
    std::ofstream out("blifoutput.txt");
    out << ".model " << moudle_name << "\n";

    out << ".inputs";
    for (const auto& i : inputs)
        out << " " << i;
    out << "\n";

    out << ".outputs";
    for (const auto& o : outputs)
        out << " " << o;
    out << "\n";

    Cell* temp_cell=nullptr;
    int temp_id = 0;
    std::string temp_var = "A"+std::to_string(temp_id);
    int cell_var_count = 0;

    //将多个只有2个输入端口的有中间值的cell合并
    for (auto it = cells.begin(); it != cells.end();++it)
    {
        //与门
        if ((*it)->type == "$and$") {
            temp_cell = find_temp(*it);
            if (temp_cell != nullptr)
            {
         

                for (auto& p : (*it)->ports)
                {
                    temp_var = temp_var + std::to_string(++temp_id);
                    if(!p.second.starts_with("tmp"))
                     temp_cell->ports[temp_var]=p.second;
                    temp_var = "A";
                }
        
            }
          
        }
        //或门
        if ((*it)->type == "$or$") {
            temp_cell = find_temp(*it);
            if (temp_cell != nullptr)
            {
              
             

                for (auto& p : (*it)->ports)
                {
                    temp_var = temp_var + std::to_string(++temp_id);
                    if (!p.second.starts_with("tmp"))
                        temp_cell->ports[temp_var] = p.second;
                }
               

            }

        }
      
    }

    for (auto it = cells.begin(); it != cells.end();)
    {
        if ((*it)->ports["Y"].starts_with("tmp"))
        {
            it=cells.erase(it);
        }
        else
        {
            ++it;
        }
    }
    //过滤有tmp开头的cell和字符串来消除中间值
    for (auto& cell : cells) {

        //与门

        if (cell->type == "$and$") {
            //生成names后的字母
            out << ".names ";
            for (auto& p : cell->ports)
            {
                if(!p.second.starts_with("tmp"))
                {
                    out << p.second << " ";
                    cell_var_count++;
                }
            }
            out <<  "\n";
            std::string and_output = "";
            // 用于生成name下方的1
            for (int i = 1; i <= cell_var_count-1; i++)
            {
                and_output=and_output + "1";
            }
            out << and_output<<" 1\n";
            cell_var_count = 0;
        }


        //或门
        if (cell->type == "$or$") {
            out << ".names ";
            //生成name后的变量
            for (auto& p : cell->ports)
            {
                if (!p.second.starts_with("tmp"))
                {
                    out << p.second << " ";
                    cell_var_count++;
                }
            }
            out << "\n";
            //生成name下的1
            std::string or_output = "";
            for (int i = 1; i <= cell_var_count - 1; i++)
            {
                for (int m = 1; m <= cell_var_count - 1; m++)
                  or_output = or_output + "-";
               
                or_output[i-1] = '1';
                out << or_output << " 1\n";
                or_output = "";
            }
          

            cell_var_count = 0;
        }
        //非门
        if (cell->type == "$not$") {
            out << ".names " << cell->ports["A"] << " "
                << cell->ports["Y"] << "\n";
            out << "0 1\n";
          
        }
        for (auto& p : cell->ports)
         std::cout << p.first <<p.second <<" ";
        std::cout <<"\n";
    }

    for (auto& conn : connections)
    {
        out << ".names " << conn->src << " "
            << conn->dst << "\n";
        out << "1 1\n";
    }


    out << ".end\n";


    
}