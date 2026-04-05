# Sistema de Gerenciamento de Estações de Metrô
**Trabalho Prático 1 - (SCC0215) Organização de Arquivos**

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![GCC](https://img.shields.io/badge/gcc-%231E293B.svg?style=for-the-badge&logo=gnu&logoColor=white)
![Doxygen](https://img.shields.io/badge/Doxygen-blue?style=for-the-badge)

## Sobre o Projeto
Projeto desenvolvido para a disciplina de Organização de Arquivos. O objetivo principal é a manipulação direta de **arquivos binários** em linguagem C, simulando um banco de dados para gerenciar estações e linhas de metrô.
O sistema não utiliza todo o arquivo em memória RAM, garantindo a manipulação eficiente dos dados diretamente no disco (HDD/SSD).

## Especificações Técnicas
* **Registros de Tamanho Fixo:** Cada registro de dados ocupa exatamente `80 bytes`.
* **Registro de Cabeçalho:** Ocupa os primeiros `17 bytes` do arquivo.
* **Reuso de Espaço (Dinâmico):** Implementação de remoção lógica. Registros apagados são empilhados (Pilha LIFO) atualizando o RRN no cabeçalho, permitindo reaproveitamento de espaço em tempo de execução para o `INSERT`.
* **Lixo Omitido:** O espaço não utilizado dentro dos 80 bytes é preenchido com o caractere especial `$`.

## Funcionalidades Implementadas
1. **CREATE TABLE:** Leitura de um arquivo `.csv` e gravação massiva no arquivo binário.
2. **SELECT (ALL e WHERE):** Recuperação de registros diretamente do binário.
3. **DELETE:** Remoção lógica baseada no reaproveitamento dinâmico com pilha.
4. **INSERT:** Inserção de novos registros priorizando os espaços logicamente removidos.
5. **UPDATE:** Atualização *in-place* de registros, garantida pelo tamanho fixo de 80 bytes.

## Como Compilar e Executar

Para compilar o projeto utilizando o compilador GCC, abra o terminal na raiz do diretório e execute:
```bash
gcc -o trab1 *.c -Wall -Werror
```

## Autoras
Ana Luísa Pereira da Silva Leal <br>
Mariane Ferreira dos Santos

<div align="right">
  <sub> <i>Instituto de Ciências Matemáticas e de Computação (ICMC - USP)</i> </sub>
</div>


