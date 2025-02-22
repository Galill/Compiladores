#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv) {

    std::string line;
    std::ifstream file;

    file.open(argv[1]);
    if(!file.is_open()){
        std::cout << "Não foi possivel abrir o arquivo" << std::endl; 
        exit(1);
    }
   
    std::getline(file,line);
   
    for(int i = 0; i < line.size(); i++){
       if(!isdigit(line[i])){
          std::cout << "Erro de sintaxe!!!\nNão é uma constante inteira :p" << std::endl;
          exit(1);
       }
    }

    std::string modeloAssembly = R"(.section .text
.globl _start
_start:
aqui
call imprime_num
call sair
.include "runtime.s"
)";

   
    size_t posInsercao = modeloAssembly.find("aqui");

    if (posInsercao != std::string::npos) {
        modeloAssembly.replace(posInsercao, 4 , "mov $" + line + ", %rax");
    }

    std::ofstream arquivoSaida(argv[2]);

    if (arquivoSaida.is_open()) {
        arquivoSaida << modeloAssembly;
        arquivoSaida.close();

        std::cout << "Arquivo assembly gerado com sucesso: " << argv[2] << std::endl;
    } else {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << argv[2] << std::endl;
    }

   std::string comando_1 = "as --64 -o saida.o " + std::string(argv[2]);
   
   system(comando_1.c_str());
   system("ld -o p1 saida.o");
   system("./p1");
   return 0;
}



