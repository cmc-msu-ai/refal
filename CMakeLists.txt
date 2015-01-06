cmake_minimum_required(VERSION 2.5)

project(Refal2)

include_directories(include)

set(SOURCE src/Executer.cpp
			src/Function.cpp
			src/FunctionCompiler.cpp
			src/Label.cpp
			src/main.cpp
			src/OperationsBuilder.cpp
			src/Parser.cpp
			src/Qualifier.cpp
			src/QualifierBuilder.cpp
			src/Refal2.cpp
			src/Scanner.cpp
			src/Unit.cpp
			src/Variables.cpp)

add_executable(refal2 ${SOURCE})
