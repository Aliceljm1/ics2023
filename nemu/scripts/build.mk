.DEFAULT_GOAL = app

# Add necessary options if the target is a shared library
ifeq ($(SHARE),1)
SO = -so
CFLAGS  += -fPIC
LDFLAGS += -rdynamic -shared -fPIC
endif

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

INC_PATH := $(WORK_DIR)/include $(INC_PATH)
OBJ_DIR  = $(BUILD_DIR)/obj-$(NAME)$(SO)
BINARY   = $(BUILD_DIR)/$(NAME)$(SO)

# Compilation flags
ifeq ($(CC),clang)
CXX := clang++
else
CXX := g++
endif
#强制使用riscv工具链
# CC = riscv64-linux-gnu-gcc
# CXX = riscv64-linux-gnu-g++
# LD = riscv64-linux-gnu-ld
LD := $(CXX) 
# LD = riscv64-linux-gnu-ld

INCLUDES = $(addprefix -I, $(INC_PATH))
CFLAGS  := -O2 -MMD -Wall $(INCLUDES) $(CFLAGS)
LDFLAGS := -O2 $(LDFLAGS)

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o) $(CXXSRC:%.cc=$(OBJ_DIR)/%.o)

# Define preprocessed files
# PREPROCESSED_FILES := $(SRCS:%.c=$(OBJ_DIR)/%.i)

# Compilation patterns
$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)

# @$(CC) $(CFLAGS) -E $< -o $(OBJ_DIR)/$(<:.c=.i)
	$(CC) $(CFLAGS) -c -o $@ $<
# 生成宏展开后的代码并输出到 .i 文件
	$(CC) $(CFLAGS) -E $< -o $(@:.o=.i)
	$(call call_fixdep, $(@:.o=.d), $@)

$(OBJ_DIR)/%.o: %.cc
	@echo + CXX $<
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean

app: $(BINARY)

$(BINARY): $(OBJS) $(ARCHIVES)
	echo + LD $@
	echo "cmd is= $(LD) -o $@ $(OBJS) $(LDFLAGS) $(ARCHIVES) $(LIBS)"
	$(LD) -o $@ $(OBJS) $(LDFLAGS) $(ARCHIVES) $(LIBS)

clean:
	-rm -rf $(BUILD_DIR)
