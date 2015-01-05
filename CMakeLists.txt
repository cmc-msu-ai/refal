cmake_minimum_required(VERSION 2.5)

project(Refal2)

include_directories(include)

set(SOURCE src/Common.cpp
			src/Compiler.cpp
			src/Executer.cpp
			src/FieldOfView.cpp
			src/Function.cpp
			src/FunctionCompiler.cpp
			src/Label.cpp
			src/LinkBuilder.cpp
			src/main.cpp
			src/OperationsBuilder.cpp
			src/Parser.cpp
			src/Qualifier.cpp
			src/QualifierBuilder.cpp
			src/Reader.cpp
			src/Refal.cpp
			src/Scanner.cpp
			src/Unit.cpp
			src/Variables.cpp)

add_executable(refal2 ${SOURCE})
