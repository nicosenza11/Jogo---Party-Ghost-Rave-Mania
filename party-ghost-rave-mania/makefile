# Nome do compilador
CC = gcc

# Nome do seu executável final
TARGET = meu_jogo

# Diretórios
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Encontra todos os arquivos .c no diretório SRCDIR
# Ex: src/main.c src/player.c
SRCS = $(wildcard $(SRCDIR)/*.c)

# Gera os nomes dos arquivos objeto .o no diretório OBJDIR
# Ex: obj/main.o obj/player.o
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Flags de compilação
# -Wall: Ativa todos os avisos comuns
# -g: Inclui informações de debug
# -I$(INCDIR): Diz ao compilador para procurar arquivos de cabeçalho (.h) no diretório INCDIR
# $(shell pkg-config ...): Pega as flags necessárias para o Allegro
ALLEGRO_CFLAGS = $(shell pkg-config --cflags allegro-5 allegro_primitives-5 allegro_font-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_ttf-5)
CFLAGS = -Wall -g -I$(INCDIR) $(ALLEGRO_CFLAGS)

# Flags de linkagem
# $(shell pkg-config ...): Pega as bibliotecas necessárias para o Allegro
ALLEGRO_LIBS = $(shell pkg-config --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_ttf-5)
LDFLAGS = $(ALLEGRO_LIBS) -lm

# Regra padrão: construir o alvo principal (TARGET)
all: $(TARGET)

# Regra para linkar o executável final
# Depende de todos os arquivos objeto (.o)
$(TARGET): $(OBJS)
	@echo "Ligando $(TARGET)..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "$(TARGET) criado com sucesso!"

# Regra genérica para compilar arquivos .c (em SRCDIR) para arquivos .o (em OBJDIR)
# Ex: obj/main.o depende de src/main.c e de qualquer .h em INCDIR que ele inclua
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR) # A dependência "| $(OBJDIR)" garante que OBJDIR é criado
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para criar o diretório de objetos, se não existir
# Usada como uma "order-only prerequisite" na regra de compilação acima
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Regra para limpar os arquivos gerados
clean:
	@echo "Limpando arquivos compilados..."
	rm -f $(OBJDIR)/*.o $(TARGET)
	@echo "Limpeza concluída."

# Marcar alvos que não são nomes de arquivos reais
.PHONY: all clean

# Prevenir que o make delete arquivos intermediários (os .o)
.SECONDARY: $(OBJS)