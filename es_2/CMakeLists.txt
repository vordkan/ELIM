cmake_minimum_required(VERSION 3.0)
project(OpenCV_Test)

# Trova OpenCV
find_package(OpenCV REQUIRED)

# Aggiungi l'eseguibile del tuo programma
add_executable(es_4 es_4.cpp)

# Collega OpenCV al tuo programma
target_link_libraries(es_4 ${OpenCV_LIBS})

# Include le directory di inclusione di OpenCV solo per il target es_1
target_include_directories(es_4 PRIVATE ${OpenCV_INCLUDE_DIRS})
