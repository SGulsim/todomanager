FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Копирование файлов проекта
COPY CMakeLists.txt ./
COPY src/ ./src/
COPY include/ ./include/
COPY third_party/ ./third_party/

# Создание директории для сборки
RUN mkdir -p build

# Сборка проекта
WORKDIR /app/build
RUN cmake .. && \
    cmake --build . --config Release || \
    cmake --build .

# Создание директории для данных
RUN mkdir -p /app/data

# Установка рабочей директории
WORKDIR /app

# Копирование исполняемого файла из build директории
RUN cp /app/build/todomanager /app/todomanager && \
    chmod +x /app/todomanager

# Открытие порта
EXPOSE 8080

# Переменные окружения
ENV PORT=8080
ENV HOST=0.0.0.0
ENV DB_PATH=./data/tasks.db

# Запуск приложения
CMD ["./todomanager"]

