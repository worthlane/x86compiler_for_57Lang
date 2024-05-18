CXX = g++

FRONT = front
MID   = mid
BACK  = back

CXXFLAGS  = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
			-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts       \
			-Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal  \
			-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline          \
			-Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked            \
			-Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo  \
			-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn                \
			-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default      \
			-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast           \
			-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing   \
			-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation    \
			-fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192         \
			-Wstack-usage=8192 -fPIE -Werror=vla

HOME = $(shell pwd)

SOURCE_DIR = src
CXXFLAGS  += -I $(HOME)/$(SOURCE_DIR)

IMAGE       = $(SOURCE_DIR)/assets/img
BUILD_DIR   = $(SOURCE_DIR)/build/bin

FRONT_OBJECTS_DIR = $(SOURCE_DIR)/build/$(FRONT)
MID_OBJECTS_DIR   = $(SOURCE_DIR)/build/$(MID)
BACK_OBJECTS_DIR  = $(SOURCE_DIR)/build/$(BACK)

DOXYFILE = Doxyfile
DOXYBUILD = doxygen $(DOXYFILE)

# ==============================================================
# 						 FILE GROUPS
# ==============================================================

EXPRESSION_SOURCES = tree.cpp visual.cpp tree_output.cpp tree_input.cpp
EXPRESSION_DIR     = $(SOURCE_DIR)/tree

FRONTEND_SOURCES = analysis.cpp syntax_parser.cpp frontend.cpp main.cpp
FRONTEND_DIR     = $(SOURCE_DIR)/frontend

MIDDLEEND_SOURCES = main.cpp middleend.cpp
MIDDLEEND_DIR     = $(SOURCE_DIR)/middleend

BACKEND_SOURCES = main.cpp backend.cpp
BACKEND_DIR     = $(SOURCE_DIR)/backend

COMPILER_SOURCES = ir.cpp byte_code.cpp
COMPILER_DIR     = $(SOURCE_DIR)/backend/compiler

X86_SOURCES = x86_asm.cpp x86_elf.cpp x86_encode.cpp
X86_DIR     = $(SOURCE_DIR)/backend/compiler/x86-64

COMMON_SOURCES = logs.cpp errors.cpp input_and_output.cpp file_read.cpp
COMMON_DIR     = $(SOURCE_DIR)/common

STACK_SOURCES = stack.cpp hash.cpp nametable.cpp
STACK_DIR     = $(SOURCE_DIR)/stack

# ==============================================================
#                           FRONT
# ==============================================================

FRONT_EXPRESSION_OBJECTS = $(EXPRESSION_SOURCES:%.cpp=$(FRONT_OBJECTS_DIR)/%.o)
FRONT_FRONTEND_OBJECTS   = $(FRONTEND_SOURCES:%.cpp=$(FRONT_OBJECTS_DIR)/%.o)
FRONT_COMMON_OBJECTS     = $(COMMON_SOURCES:%.cpp=$(FRONT_OBJECTS_DIR)/%.o)
FRONT_STACK_OBJECTS      = $(STACK_SOURCES:%.cpp=$(FRONT_OBJECTS_DIR)/%.o)

# ==============================================================
#                            MID
# ==============================================================

MID_EXPRESSION_OBJECTS = $(EXPRESSION_SOURCES:%.cpp=$(MID_OBJECTS_DIR)/%.o)
MID_MIDDLEEND_OBJECTS  = $(MIDDLEEND_SOURCES:%.cpp=$(MID_OBJECTS_DIR)/%.o)
MID_COMMON_OBJECTS     = $(COMMON_SOURCES:%.cpp=$(MID_OBJECTS_DIR)/%.o)
MID_STACK_OBJECTS      = $(STACK_SOURCES:%.cpp=$(MID_OBJECTS_DIR)/%.o)

# ==============================================================
#                           BACK
# ==============================================================

BACK_EXPRESSION_OBJECTS = $(EXPRESSION_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)
BACK_BACKEND_OBJECTS    = $(BACKEND_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)
BACK_COMPILER_OBJECTS   = $(COMPILER_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)
BACK_X86_OBJECTS        = $(X86_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)
BACK_COMMON_OBJECTS     = $(COMMON_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)
BACK_STACK_OBJECTS      = $(STACK_SOURCES:%.cpp=$(BACK_OBJECTS_DIR)/%.o)

# ==============================================================

.PHONY: all
all: $(FRONT) $(MID) $(BACK)

# ----------------------------------- FRONT --------------------------------------------

$(FRONT): $(FRONT_EXPRESSION_OBJECTS) $(FRONT_COMMON_OBJECTS) $(FRONT_FRONTEND_OBJECTS) $(FRONT_STACK_OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(FRONT_OBJECTS_DIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(FRONT_OBJECTS_DIR)/%.o : $(EXPRESSION_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(FRONT_OBJECTS_DIR)/%.o : $(FRONTEND_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(FRONT_OBJECTS_DIR)/%.o : $(STACK_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

# ------------------------------------- MID ------------------------------------------

$(MID): $(MID_EXPRESSION_OBJECTS) $(MID_COMMON_OBJECTS) $(MID_MIDDLEEND_OBJECTS) $(MID_STACK_OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(MID_OBJECTS_DIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(MID_OBJECTS_DIR)/%.o : $(EXPRESSION_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(MID_OBJECTS_DIR)/%.o : $(MIDDLEEND_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(MID_OBJECTS_DIR)/%.o : $(STACK_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

# ------------------------------------ BACK -------------------------------------------

$(BACK): $(BACK_EXPRESSION_OBJECTS) $(BACK_COMMON_OBJECTS) $(BACK_BACKEND_OBJECTS)	\
									$(BACK_STACK_OBJECTS) $(BACK_COMPILER_OBJECTS) $(BACK_X86_OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(EXPRESSION_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(BACKEND_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(COMPILER_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(X86_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BACK_OBJECTS_DIR)/%.o : $(STACK_DIR)/%.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

# -------------------------------------------------------------------------------

.PHONY: doxybuild clean makedirs

doxybuild:
	$(DOXYBUILD)

clean:
	rm -rf 		$(FRONT) $(FRONT_OBJECTS_DIR)/*.o \
		   		$(MID)   $(MID_OBJECTS_DIR)/*.o   \
		   		$(BACK)  $(BACK_OBJECTS_DIR)/*.o  \
		   		*.html *.log $(IMAGE)/*.png *.dot *.gpl *.log *.pdf *.aux *.s

makedirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(FRONT_OBJECTS_DIR)
	mkdir -p $(MID_OBJECTS_DIR)
	mkdir -p $(BACK_OBJECTS_DIR)
	mkdir -p $(IMAGE)




