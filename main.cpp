#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>
#include <cctype>

using namespace std;

using VariableValue = std::variant<int, double, std::string>;

struct Variable
{
    std::string type;
    VariableValue value;
};

std::unordered_map<std::string, Variable> variables;

void printVariable(const std::string &varName)
{
    if (variables.find(varName) != variables.end())
    {
        const Variable &var = variables[varName];
        std::cout << "Valor de " << varName << " (" << var.type << "): ";

        if (std::holds_alternative<int>(var.value))
        {
            std::cout << std::get<int>(var.value);
        }
        else if (std::holds_alternative<double>(var.value))
        {
            std::cout << std::get<double>(var.value);
        }
        else if (std::holds_alternative<std::string>(var.value))
        {
            std::cout << std::get<std::string>(var.value);
        }

        std::cout << std::endl;
    }
    else
    {
        std::cerr << "Erro: variável " << varName << " não definida.\n";
    }
}

void interpreter(const string &line)
{

    string current;

    for (size_t i = 0; i < line.size(); i++)
    {
        char ch = line[i];

        if (isspace(ch))
        {
            continue;
        }

        if (line.substr(i, 5) == "PRINT" && (i + 5 >= line.size() || isspace(line[i + 5])))
        {
            i += 5;

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            if (i < line.size() && line[i] == '"')
            {
                i++;
                std::string strContent;

                while (i < line.size() && line[i] != '"')
                {
                    strContent += line[i++];
                }

                if (i < line.size() && line[i] == '"')
                {
                    std::cout << strContent << std::endl;
                    i++;
                }
                else
                {
                    std::cerr << "Erro de sintaxe: string não fechada.\n";
                }
            }

            else if (i < line.size() && isalpha(line[i]))
            {
                std::string varName;
                varName += line[i];

                while (i + 1 < line.size() && (isalnum(line[i + 1]) || line[i + 1] == '_'))
                {
                    varName += line[++i];
                }

                printVariable(varName);
            }

            else
            {

                std::cerr << "Erro de sintaxe: esperado um identificador ou string após PRINT.\n";
            }
        }

        else if (line.substr(i, 3) == "LET" && (i + 3 >= line.size() || isspace(line[i + 3])))
        {
            i += 3;

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            std::string varAtt;

            std::string varName;
            if (i < line.size() && isalpha(line[i]))
            {
                varName += line[i];
                while (i + 1 < line.size() && (isalnum(line[i + 1]) || line[i + 1] == '_'))
                {
                    varName += line[++i];
                }
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado um nome de variável após LET.\n";
                return;
            }

            i += 1;

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            if (i < line.size() && line[i] == '=')
            {

                i++;

                while (i < line.size() && isspace(line[i]))
                {
                    i++;
                }

                if (i < line.size() && line[i] == '"')
                {

                    i++;
                    std::string strValue;
                    while (i < line.size() && line[i] != '"')
                    {
                        strValue += line[i++];
                    }

                    if (i < line.size() && line[i] == '"')
                    {
                        i++;
                        variables[varName] = {"string", strValue};
                    }
                    else
                    {
                        std::cerr << "Erro de sintaxe: string não fechada.\n";
                    }
                }
                else if (isdigit(line[i]) || line[i] == '-')
                {

                    std::string numValue;
                    while (i < line.size() && (isdigit(line[i]) || line[i] == '.'))
                    {
                        numValue += line[i++];
                    }

                    if (numValue.find('.') != std::string::npos)
                    {
                        variables[varName] = {"double", std::stod(numValue)};
                    }
                    else
                    {
                        variables[varName] = {"int", std::stoi(numValue)};
                    }
                }
                else if (i < line.size() && isalpha(line[i]))
                {
                    varAtt += line[i];
                    while (i + 1 < line.size() && (isalnum(line[i + 1]) || line[i + 1] == '_'))
                    {
                        varAtt += line[++i];
                    }
                }

                else
                {
                    std::cerr << "Erro de sintaxe: valor inválido.\n";
                }
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado '=' após nome da variável. Posição: " << i << " -> " << line[i] << std::endl;
            }
        }

        else if (line.substr(i, 5) == "INPUT" && (i + 5 >= line.size() || isspace(line[i + 5])))
        {
            i += 5;

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            std::string varName;
            if (i < line.size() && isalpha(line[i]))
            {
                varName += line[i];
                while (i + 1 < line.size() && (isalnum(line[i + 1]) || line[i + 1] == '_'))
                {
                    varName += line[++i];
                }
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado um nome de variável após INPUT.\n";
                return;
            }

            std::cout << "Digite um valor para " << varName << ": ";
            std::string input;
            std::getline(std::cin, input);

            try
            {
                if (input.find('.') != std::string::npos)
                {
                    variables[varName] = {"double", std::stod(input)};
                }
                else
                {
                    variables[varName] = {"int", std::stoi(input)};
                }
            }
            catch (const std::invalid_argument &e)
            {

                variables[varName] = {"string", input};
            }
        }

        else if (line.substr(i, 2) == "IF" && (i + 2 >= line.size() || isspace(line[i + 2])))
        {
            i += 2;

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            std::string var1;
            if (i < line.size() && isalpha(line[i]))
            {
                var1 += line[i++];
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado uma variável.\n";
                return;
            }

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            std::string op;
            if (line.substr(i, 1) == ">" || line.substr(i, 1) == "<")
            {
                op = line[i++];
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado um operador de comparação.\n";
                return;
            }

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            std::string var2;
            if (i < line.size() && isalpha(line[i]))
            {
                var2 += line[i++];
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado uma segunda variável.\n";
                return;
            }

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            if (line.substr(i, 4) == "THEN")
            {
                i += 4;
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado 'THEN'.\n";
                return;
            }

            while (i < line.size() && isspace(line[i]))
            {
                i++;
            }

            if (line.substr(i, 5) == "PRINT")
            {
                i += 5;

                while (i < line.size() && isspace(line[i]))
                {
                    i++;
                }

                if (line[i] == '"')
                {
                    i++;
                    std::string message;
                    while (i < line.size() && line[i] != '"')
                    {
                        message += line[i++];
                    }

                    if (line[i] == '"')
                    {
                        i++;
                    }
                    else
                    {
                        std::cerr << "Erro de sintaxe: esperado fechamento de aspas.\n";
                        return;
                    }

                    bool conditionMet = false;
                    if (op == ">" && std::get<int>(variables[var1].value) > std::get<int>(variables[var2].value))
                    {
                        conditionMet = true;
                    }
                    else if (op == "<" && std::get<int>(variables[var1].value) < std::get<int>(variables[var2].value))
                    {
                        conditionMet = true;
                    }

                    if (conditionMet)
                    {
                        std::cout << message << std::endl;
                    }
                    else
                    {
                        std::cout << "Condição falsa! Nenhuma ação tomada.\n";
                    }
                }
                else
                {
                    std::cerr << "Erro de sintaxe: esperado uma mensagem entre aspas após PRINT.\n";
                }
            }
            else
            {
                std::cerr << "Erro de sintaxe: esperado 'PRINT' após 'THEN'.\n";
            }
        }
    }
}

int main()
{
    vector<string> buffer;

    string filename = "program.basic";

    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        buffer.push_back(line);
    }

    for (const auto &line : buffer)
    {
        interpreter(line);
    }

    file.close();

    return 0;
}