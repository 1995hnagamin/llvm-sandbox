LLVM_CONFIG = llvm-config
CXX = clang++

.PHONY: all clean

TARGET = lt

all: $(TARGET)

clean:
	rm -f $(TARGET)

INCLUDE = -I. -I $(shell $(LLVM_CONFIG) --includedir)

CXXFLAGS = -O2 -g \
	-std=c++14 \
	-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS \
	-Wall -Wextra \
	-pedantic \
	-Wno-unused-parameter \
	-fno-rtti \
	$(INCLUDE)

LIBS = -L$(shell $(LLVM_CONFIG) --libdir) \
	-lclangFrontend -lclangSerialization -lclangDriver \
	-lclangTooling -lclangParse -lclangSema \
	-lclangAnalysis -lclangRewriteFrontend -lclangRewrite \
	-lclangEdit -lclangAST -lclangLex -lclangBasic \
	-lclangFrontendTool -lclangASTMatchers \
	-lclang -ldl -ltinfo -lz -lpthread \
	$(shell $(LLVM_CONFIG) --libs mcparser bitreader support mc option profiledata)

$(TARGET): main.cpp
	$(CXX) main.cpp $(CXXFLAGS) $(LIBS) -o $(TARGET)
