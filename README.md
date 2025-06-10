# Projeto de Jogos 2D com C++ e Raylib

Este projeto foi desenvolvido com o objetivo de aplicar os conceitos de programação gráfica e desenvolvimento de jogos utilizando a linguagem **C++** e a biblioteca **Raylib**. 

O projeto contém dois jogos distintos:

1. **Vida Universitária** – Simula situações do cotidiano de um estudante, utilizando menus e escolhas interativas.
2. **CriptoRush** – Um jogo de reflexos rápidos onde o jogador coleta moedas representando criptomoedas, com progressão de fases.



## Como Executar

Certifique-se de que a biblioteca **Raylib** esteja instalada corretamente no seu sistema.  
No terminal, acesse o diretório `src` e execute o comando abaixo para compilar o projeto:

cd src
g++ main.cpp \
game_vida_universitaria/vida_universitaria.cpp \
game_criptomoedas/cripto.cpp \
-o jogo -lraylib -std=c++17 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

./jogo
