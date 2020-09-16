#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

/**
 * \brief Функция для получения вектора подстрок, разделенных символом
 * \param[in] input Строка, из которой будут вырезаться подстроки
 * \param[in] del Символ, по которому будет происходить деление
 * \return вектор подстрок
 */
vector<string> split(const string& input, char del);
/**
 * \brief Функция для заполнения ассоциативного массива данными из таблицы
 * \param[in] filename Название файла, из которого происходит чтение
 * \param[out] table ассоциативный массив, хранящий в качесте ключа строку название столбца + название строки, а значения - содержимое ячейки
 * \param[out] cellsToCompute вектор ключей, которые надо рассчитать
 * \param[out] columns вектор названий столбцов
 * \param[out] rows вектор названий строк
 */
void fillTable(string filename,map <string,string> &table, vector <string> &cellsToCompute,vector <string> &columns,vector <string> &rows);   
/**
 * \brief Вычисление выражения в ячейке
 * \param[in] table ассоциативный массив, хранящий в качесте ключа строку название столбца + название строки, а значения - содержимое ячейки
 * \param[in] cell Ключ ячейки
 * \return вычисленное значение в ячейке
 */
int computeCell(map <string,string> &table, string cell);
/**
 * \brief Вычисление выражений в ячейках
 * \param[in] table ассоциативный массив, хранящий в качесте ключа строку название столбца + название строки, а значения - содержимое ячейки
 * \param[in] cellsToCompute Вектор ключей ячеек
 */
void computeCells(map <string,string> &table, vector <string> &cellsToCompute);
/**
 * \brief Вывод таблицы в консоль
 * \param[in] table ассоциативный массив, хранящий в качесте ключа строку название столбца + название строки, а значения - содержимое ячейки
 * \param[in] columns вектор названий столбцов
 * \param[in] rows вектор названий строк
 */
void printTable(map <string,string> &table, vector <string> columns, vector <string> rows);

int main (int argc, char* argv [])
{
    map <string,string> table;
    vector <string> cellsToCompute;
    vector <string> columns;
    vector <string> rows;
    if(argc > 1)
        fillTable(argv[1],table,cellsToCompute,columns,rows);
    else
        fillTable("test1.csv",table,cellsToCompute,columns,rows);        
    computeCells(table,cellsToCompute);
    printTable(table,columns,rows);

    return 0;
}
void printTable(map <string,string> &table, vector <string> columns, vector <string> rows){
    map <string,string> :: iterator it = table.begin();
    string row = "";
    string col = "";
    cout.setf(ios::right);
    cout.width(1);
    cout<<"\t";
    for(string col:columns){
        cout<<col<<"\t";
    }
    cout<<endl;
    for(string row:rows){
        cout<<row<<":\t";
        for(string col:columns){
            cout<<table[col+row]<<"\t";
        }        
        cout<<endl;
    }
}
void computeCells(map <string,string> &table, vector <string> &cellsToCompute){
    for(string cell : cellsToCompute){
        try{
            stoi(table[cell]);  
        }
        catch(const std::invalid_argument& ia)
        {
            table[cell] = to_string (computeCell(table,cell));
        }   
           
    }
}
int computeCell(map <string,string> &table, string cell){
    int result=0;
    if(table[cell].at(0)=='='){
        table[cell].erase(0,1);

        if(table[cell].find('+')!= std::string::npos)
        {
            for(string subcell : split(table[cell],'+'))
            {
                try 
                {
                    result+=stoi(table[subcell]);

                }
                catch (const std::invalid_argument& ia) 
                {
                    result+=computeCell(table,subcell);
                }
            }
        }
        else if(table[cell].find('-')!= std::string::npos)
        {
            int counter=0;
            for(string subcell : split(table[cell],'-')){
                try {
                    if(counter==0){
                        result=stoi(table[subcell]);
                        counter++;
                    }
                    else
                    {
                        result-=stoi(table[subcell]);
                    }
                }
                catch (const std::invalid_argument& ia) {
                    if(counter==0){
                        result=computeCell(table,subcell);
                        counter++;
                    }
                    else
                    {
                        result-=computeCell(table,subcell);
                    }
                    
                }
            }
            result=result<0?0:result;
        }
        else if(table[cell].find('*')!= std::string::npos)
        {
            result=1;
            for(string subcell : split(table[cell],'*')){
                try {
                    result*=stoi(table[subcell]);
                }
                catch (const std::invalid_argument& ia) {
                    result*=computeCell(table,subcell);
                }
            }
        }
        else if(table[cell].find('/')!= std::string::npos)
        {
            result=1;
            int counter=0;
            for(string subcell : split(table[cell],'/')){
                
                try 
                {
                    if(counter==0)
                    {
                        result=stoi(table[subcell]);
                        counter++;
                    }
                    else
                    {
                        result/=stoi(table[subcell]);
                    }                    
                }
                catch (const std::invalid_argument& ia) 
                {
                    if(counter==0)
                    {
                        result=computeCell(table,subcell);
                        counter++;
                    }
                    else
                    {
                        result/=computeCell(table,subcell);
                    }
                }
            }       
        }
    }
    table[cell] = to_string(result);
    return result;
        
}
void fillTable(string filename,map <string,string> &table, vector <string> &cellsToCompute,vector <string> &columns,vector <string> &rows){
    ifstream myfile;
    myfile.open(filename);
    string line;
   
    if (myfile.is_open())
    {
        while ( getline(myfile,line) )
        {
            if(line.at(0)==','){
                for(string col : split(line,',')){
                    if(col!="")
                        columns.push_back(col);
                }
                myfile.seekg(0, ios::beg);
                break;
            }
        }
        while ( getline(myfile,line) )
        {
            if(line.at(0)==','){
                continue;
            }
            vector<string> items = split(line,',');
            int counter = 1;
            rows.push_back(items[0]);
            for(string col : columns ){
                if(counter<items.size()){
                    table[col+items[0]] = items[counter];
                    try {
                        stoi(items[counter++]);
                    }
                    catch (const std::invalid_argument& ia) {
                        cellsToCompute.push_back(col+items[0]);
                    }
                }
            }
        }
    }    
    myfile.close();    
}
vector<string> split(const string& input, char del) {
  stringstream ss(input);
  string item;
  vector<std::string> elems;
  while (getline(ss, item, del)) {
    elems.push_back(item);
  }
  return elems;
}