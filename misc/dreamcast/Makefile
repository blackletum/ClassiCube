BUILD_DIR		:= build-dc
SOURCE_DIRS		:= src third_party/bearssl/src misc/dreamcast

S_FILES := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.S))
C_FILES := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))
OBJS 	:= $(addprefix $(BUILD_DIR)/, $(notdir $(C_FILES:%.c=%.o) $(S_FILES:%.S=%.o)))
CFLAGS	:=-g -O2 -pipe -fno-math-errno -Ithird_party/bearssl/inc

# Dependency tracking
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d
DEPFILES := $(OBJS:%.o=%.d)

GLDC_LIB=third_party/gldc/libGLdc.a
LDFLAGS=-g
LIBS=-lm $(GLDC_LIB) -lppp -lkosfat

TARGET := ClassiCube-dc
CC_TEXTURES = misc/dreamcast/classicube.zip

ifeq ($(strip $(KOS_BASE)),)
$(error "Please set KOS variables in your environment.")
endif

default: $(CC_TEXTURES) $(GLDC_LIB) $(BUILD_DIR) $(TARGET).cdi

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	
$(GLDC_LIB): FORCE
	$(MAKE) -C third_party/gldc
FORCE: ;

# TODO add textures to misc folder ?	
$(CC_TEXTURES): 
	curl http://www.classicube.net/static/default.zip -o $@
	
$(BUILD_DIR)/%.o: src/%.c
	kos-cc $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: misc/dreamcast/%.S
	kos-cc -c $< -o $@

$(BUILD_DIR)/%.o: third_party/bearssl/src/%.c
	kos-cc $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	kos-cc $(LDFLAGS) $^ -o $@ $(LIBS)
	
$(TARGET).bin: $(TARGET).elf
	sh-elf-objcopy -R .stack -O binary $(TARGET).elf $(TARGET).bin
	
# https://dcemulation.org/phpBB/viewtopic.php?t=105269
$(TARGET)-scr.bin: $(TARGET).bin
	$(KOS_BASE)/utils/scramble/scramble $(TARGET).bin $(TARGET)-scr.bin
	
$(TARGET).iso: $(TARGET)-scr.bin
	mkdir -p ISO_FILES
	cp $(TARGET)-scr.bin ISO_FILES/1ST_READ.BIN
	mkdir -p ISO_FILES/audio
	mkdir -p ISO_FILES/maps
	mkdir -p ISO_FILES/texpacks
	mkdir -p ISO_FILES/texturecache
	cp $(CC_TEXTURES) ISO_FILES/texpacks/default.zip
	cp misc/dreamcast/IP.BIN IP.BIN
	mkisofs -G IP.BIN -C 0,11702 -J -l -r -o $(TARGET).iso ISO_FILES
	# genisoimage -V ClassiCube -G IP.BIN -joliet -rock -l -o $(TARGET).iso ISO_FILES
	
$(TARGET).cdi: $(TARGET).iso
	cdi4dc $(TARGET).iso $(TARGET).cdi

# Dependency tracking
$(DEPFILES):

include $(wildcard $(DEPFILES))
