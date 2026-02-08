# Makefile for Kria DMA Test (Linux Userspace)
CC = gcc
CFLAGS = -O2 -Wall -g
TARGET = dma_kria_test
SRC = src/dma_kria_test.c

# Si tienes una carpeta deploy en la raíz del repo
DEPLOY_DIR = ./deploy

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "✅ Build success! Executable: ./$(TARGET)"

clean:
	rm -f $(TARGET)

# Copia el ejecutable a la carpeta deploy (si existe)
install: $(TARGET)
	mkdir -p $(DEPLOY_DIR)
	cp $(TARGET) $(DEPLOY_DIR)
	@echo "📦 Installed to $(DEPLOY_DIR)"