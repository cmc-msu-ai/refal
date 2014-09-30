cmake_minimum_required(VERSION 2.5)

project(Refal2)

include_directories(include)

set(SOURCE src/Common.cpp
            src/Compiler.cpp
            src/Executer.cpp
            src/FieldOfView.cpp
            src/FunctionBuilder.cpp
            src/LinkBuilder.cpp
            src/OperationsBuilder.cpp
            src/Qualifier.cpp
            src/QualifierBuilder.cpp
            src/Reader.cpp
            src/Refal.cpp
            src/main.cpp)

add_executable(refal2 ${SOURCE})
