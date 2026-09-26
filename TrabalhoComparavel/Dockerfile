# Imagem inicial
FROM mcr.microsoft.com/windows/servercore:ltsc2022

#Definindo diretório dentro do container. Criando diretório
WORKDIR C:\app

# Copiando para a imagem o arquivo .cpp dentro do diretório /app
COPY TrabalhoComparavel/output/Sequencial.exe .
COPY TrabalhoComparavel/output/MultiThread.exe .
COPY TrabalhoComparavel/output/MultiProcess.exe .

# Compilando arquivo .cpp
RUN g++ -std=c++17 -O2 -Wall -Wextra Sequencial.cpp -o Sequencial \ 
 && g++ -std=c++17 -O2 -Wall -Wextra MultiThread.cpp -o MultiThread \
 && g++ -std=c++17 -O2 -Wall -Wextra MultiProcess.cpp -o MultiProcess

# Executando somente o Sequencial, para testar o Dockerfile.

CMD ["cmd", "/S", "/C", "Sequencial.exe && MultiThread.exe 4 && MultiProcess.exe 4"]
