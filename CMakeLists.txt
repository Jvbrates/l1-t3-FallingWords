cmake_minimum_required(VERSION 3.22)
project(l1_t3_Jvrates C)

set(CMAKE_C_STANDARD 11)


include_directories("/usr/include/allegro5")
target_link_libraries(ProjectName "/usr/include/allegro5/allegro.h)
add_executable(l1_t3_Jvrates
        buttons.c
        buttons.h
        gravidade.h
        tela.c
        tela.h
        trabalho3.c)
