LLVM_CONFIG = llvm-config
CXX = clang++

.PHONY: all clean

TARGET = tips

all: $(TARGET)

clean:
	rm -f $(TARGET)

INCLUDE = -I.

CXXFLAGS = -O2 -g \
	$(shell $(LLVM_CONFIG) --cxxflags) \
	-Wall -Wextra \
	-pedantic \
	-Wno-unused-parameter \

LIBS = -L$(shell $(LLVM_CONFIG) --libdir) \
	-lclangTooling \
	-lclangFrontend -lclangSerialization -lclangDriver \
	-lclangParse -lclangSema \
	-lclangAnalysis -lclangRewriteFrontend -lclangRewrite \
	-lclangEdit -lclangAST -lclangLex -lclangBasic \
	-lclangFrontendTool -lclangASTMatchers \
	-lclang -ldl -ltinfo -lz -lpthread \
	$(shell $(LLVM_CONFIG) --libs mcparser bitreader support mc option profiledata)

OBJS = main.o pragma_dead_handler.o

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LIBS) -o $(TARGET)
