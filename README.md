###Readme: Repositório projetoPP-MPI
Este repositório contém um projeto de análise de desempenho em computação paralela utilizando MPI (Message Passing Interface) em dois ambientes distintos: um cluster de computadores e um computador pessoal multinúcleo. 
O objetivo é comparar o desempenho da execução paralela em diferentes configurações de hardware e tamanhos de carga de trabalho.

##Estrutura do Repositório
paralelo.c: Implementação do código paralelo utilizando MPI para busca de uma senha específica em arquivos grandes.
sequencial.c: Implementação do código sequencial equivalente para comparação de desempenho.
dicionarios(*.txt) : Arquivo contendo uma lista de senhas para teste.
README.md: Este arquivo, fornecendo informações sobre o projeto e guia de utilização.

##Como Executar
Pré-requisitos:

-Compilador MPI instalado (por exemplo, OpenMPI).
-Ambiente adequado para execução paralela (cluster de computadores ou computador pessoal multinúcleo).

#Compilação:
Utilize o compilador MPI para compilar os arquivos codigo_paralelo_mpi.c e codigo_sequencial.c. Exemplo:
  
  mpicc paralelo -o paralelo.c

  gcc -o sequencial sequencial.c

#Execução
  mpirun --oversubscribe -np 4 ./paralelo

  ./sequencial

  Para mais informações e detalhes sobre a implementação, consulte os códigos-fonte fornecidos.

#Contato
jhenrique1587@gmail.com 
thg1988corinthians@gmail.com
