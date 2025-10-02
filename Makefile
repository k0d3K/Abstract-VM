#===================== BASE =====================#

BUILD_DIR		:= .build/
NAME			:= avm
DEBUG_NAME		:= $(NAME)_debug
TESTER_NAME		:= tester
CXX				:= c++
CXXFLAGS		+= -Wall -Wextra -Werror -g

#==================== SOURCE ====================#

SRC_DIR			:= src/
SRC				:= CommandsExecutor Exceptions Lexer main OperandFactory Parser
SRC_TESTER		:= Tester runTest

SRC				:= $(addsuffix .cpp, $(SRC))
SRC_TESTER		:= $(addsuffix .cpp, $(SRC_TESTER))

#==================== OBJECT ====================#

OBJ				:= $(SRC:%.cpp=$(BUILD_DIR)%.o)
OBJ_TESTER		:= $(SRC_TESTER:%.cpp=$(BUILD_DIR)%.o)
OBJ_DIR			:= $(sort $(shell dirname $(OBJ)))
DEBUG_OBJ		:= $(OBJ:.o=_debug.o)

#==================== COLOR =====================#

INIT			:= \e[0m
BLACK			:= \e[30m
RED				:= \e[31m
GREEN			:= \e[32m
YELLOW			:= \e[33m
BLUE			:= \e[34m
MAGENTA			:= \e[35m
CYAN			:= \e[36m

#===================== RULE =====================#

all:	$(NAME)

$(NAME):	$(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) -Iinc
	@echo "$(GREEN)$(NAME) ready ✅️$(INIT)"

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -Iinc

$(BUILD_DIR)%_debug.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling debug : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) -D DEBUG=1 -c $< -o $@ -Iinc

$(BUILD_DIR)%.o:	avm_tester/%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -Iinc

$(DEBUG_NAME):	$(DEBUG_OBJ)
	@$(CXX) $(CXXFLAGS) -D DEBUG=1 $(DEBUG_OBJ) -o $(DEBUG_NAME) -Iinc
	@echo "$(GREEN)$(DEBUG_NAME) ready ✅️$(INIT)"

debug: $(DEBUG_NAME)

$(TESTER_NAME):	$(NAME) $(OBJ_TESTER)
	@$(CXX) $(CXXFLAGS) $(OBJ_TESTER) -o tester -Iinc -Itester
	@echo "$(GREEN)tester ready ✅️$(INIT)"

test : $(TESTER_NAME)

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "$(BLUE)Cleanning ✨🧹$(INIT)"
	@if [ -d $(BUILD_DIR) ]; then\
		echo "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)";\
		rm -rf $(BUILD_DIR);\
	fi;

fclean:	clean
	@if [ -f $(NAME) ]; then\
		echo "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)";\
		rm -f $(NAME) ;\
	fi;
	@if [ -f $(DEBUG_NAME) ]; then\
		echo "$(RED)Removing : $(MAGENTA)$(DEBUG_NAME)$(INIT)";\
		rm -f $(DEBUG_NAME) ;\
	fi;
	@if [ -f tester ]; then\
		echo "$(RED)Removing : $(MAGENTA)tester$(INIT)";\
		rm -f tester;\
	fi;

re:		fclean all
red:	fclean debug
ret:	fclean test

.PHONY: all clean fclean test re ret