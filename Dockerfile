# Imagem inicial
FROM gcc:latest

#Definindo diretório dentro do container. Criando diretório
WORKDIR /app

# Copiando para a imagem o arquivo .cpp dentro do diretório /app
COPY TrabalhoComparavel/Sequencial.cpp .
COPY TrabalhoComparavel/MultiThread.cpp .
COPY TrabalhoComparavel/MultiProcess.cpp .

# Compilando arquivo .cpp
RUN g++ -std=c++17 -O2 -Wall -Wextra Sequencial.cpp -o Sequencial \
	&& g++ -std=c++17 -O2 -Wall -Wextra -pthread MultiThread.cpp -o MultiThread \
	&& g++ -std=c++17 -O2 -Wall -Wextra MultiProcess.cpp -o MultiProcess

# Executando
CMD ["./MultiThread", "4"]
# CMD ["time", "./exemplo_thread"]