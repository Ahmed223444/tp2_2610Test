# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread

# Dossier des exécutables
EXEC_DIR = exec

# Source files
SRC = $(wildcard *.c)

# Séparer le fichier principal (avec main) des autres fichiers
MAIN_SRC = systeme.c
LIB_SRC = $(filter-out $(MAIN_SRC), $(SRC))

# Nom de l'exécutable
TARGET = $(EXEC_DIR)/systeme

# Par défaut : build l'exécutable
all: $(EXEC_DIR) $(TARGET)

# Crée le dossier exec si nécessaire
$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

# Build de l'exécutable principal
$(TARGET): $(MAIN_SRC) $(LIB_SRC)
	$(CC) $(CFLAGS) -o $@ $(MAIN_SRC) $(LIB_SRC)

# Lancer l'exécutable
run: $(TARGET)
	./$(TARGET)

# Nettoyage
clean:
	rm -rf $(EXEC_DIR)

# # Compiler and flags
# CC = gcc
# CFLAGS = -Wall -Wextra -pthread

# # Target executable
# TARGET = systeme

# # Source files
# SRC = systeme.c

# # Default rule
# all: $(TARGET)

# # Build the executable
# $(TARGET): $(SRC)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# # Run the program
# run: $(TARGET)
# 	./$(TARGET)

# # Clean up compiled files
# clean:
# 	rm -f $(TARGET)
