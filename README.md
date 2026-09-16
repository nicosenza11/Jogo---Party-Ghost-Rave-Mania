# Party Ghost Rave Mania

Jogo 2D do gênero Run 'n Gun desenvolvido em C com a biblioteca Allegro 5, inspirado na jogabilidade de clássicos como Contra e Metal Slug. O projeto foi criado como trabalho prático da disciplina de Programação 2 (UFPR).

## Visão Geral do Jogo

O jogador controla um personagem que deve se movimentar pelo cenário, desviar de ataques e eliminar hordas de inimigos enquanto avança pelo mapa. O objetivo é derrotar os inimigos regulares para liberar o chefe da fase e vencer o jogo.

### Funcionalidades Implementadas
* **Modo Singleplayer:** Campanha individual com fluxo completo de jogo (Menu Inicial, Gameplay e Game Over / Vitória).
* **Controle e Movimentação:** Personagem com suporte a andar, pular, abaixar e mirar/atirar em múltiplas direções (incluindo atirar enquanto pula ou abaixa).
* **Sistema de Combate:** Disparo de projéteis com sprite própria e sistema de vida progressivo para o jogador e inimigos.
* **Inimigos e Chefes:** Inimigos regulares com movimentação e ataques à distância, além de batalha contra chefe com padrão de combate dedicado.
* **Cenário Dinâmico:** Imagens de fundo integradas com suporte a rolagem de tela (rolling background).

## Estrutura do Projeto

```text
party-ghost-rave-mania/
├── assets/
│   ├── fontes/
│   │   └── fonte.ttf
│   ├── boss.png
│   ├── chao.png
│   ├── fantasma.png
│   ├── fantasma1.png
│   ├── fantasmaframe2.png
│   ├── fundo_arena.png
│   └── fundo_arena2.png
├── src/            # Código-fonte modularizado (.c e .h)
├── Makefile
└── README.md
```

## Requisitos do Sistema

* Compilador C (`gcc` ou `clang`)
* Make
* Allegro 5

### Instalação das Dependências (Ubuntu / Debian / Linux Mint)
```bash
sudo apt-get update
sudo apt-get install build-essential liballegro5-dev liballegro-image5-dev liballegro-ttf5-dev liballegro-audio5-dev liballegro-acodec5-dev
```

## Compilação e Execução

1. Acesse o diretório base do jogo:
   ```bash
   cd party-ghost-rave-mania
   ```

2. Compile o código com o `make`:
   ```bash
   make
   ```

3. Execute o jogo:
   ```bash
   ./meu_jogo
   ```

4. Para limpar os binários e objetos compilados:
   ```bash
   make clean
   ```

## Controles Padrão

* `A` / `D` ou `Setas`: Mover para a esquerda / direita
* `W` ou `Seta para Cima`: Mirar para cima
* `S` ou `Seta para Baixo`: Abaixar
* `Espaço`: Pular
* `J` / `Z` ou `Botão Esquerdo do Mouse`: Atirar
* `ESC`: Pausar / Voltar ao menu
