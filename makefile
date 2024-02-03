# general config

debug = 1

# Control Room
control_room_src = src/control_room
# Names of files to be compiled for the control room, without the .cpp extension
control_room_filenames := 

control_room_release_compiler = g++
control_room_release_config_flags= -O3 -s -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused 
control_rool_release_linker_flags =
control_room_release_build_path = build/control_room/release
control_room_release_build_target = control_room_release

control_room_debug_compiler = g++
control_room_debug_config_flags = -g -O0 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
control_room_debug_linker_flags = 
control_room_debug_build_path = build/control_room/debug
control_room_debug_build_target = control_room_debug

control_room_compiler = 
control_room_config_flags = 
control_room_linker_flags = 
control_room_build_path = 
control_room_build_target = 

ifeq ($(debug), 1)
	control_room_compiler = $(control_room_debug_compiler)
	control_room_config_flags = $(control_room_debug_config_flags)
	control_room_linker_flags = $(control_room_debug_linker_flags)
	control_room_build_path = $(control_room_debug_build_path)
	control_room_build_target = $(control_room_debug_build_target)
else
	control_room_compiler = $(control_room_release_compiler)
	control_room_config_flags = $(control_room_release_config_flags)
	control_room_linker_flags = $(control_room_release_linker_flags)
	control_room_build_path = $(control_room_release_build_path)
	control_room_build_target = $(control_room_release_build_target)
endif

# Engine Control
engine_control_src = src/engine_control
# Names of files to be compiled for the engine control, without the .cpp extension
engine_control_filenames := 

engine_control_release_compiler = g++
engine_control_release_config_flags= -O3 -s -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
engine_control_release_linker_flags =
engine_control_release_build_path = build/engine_control/release
engine_control_release_build_target = engine_control_release

engine_control_debug_compiler = g++
engine_control_debug_config_flags = -g -O0 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
engine_control_debug_linker_flags = 
engine_control_debug_build_path = build/engine_control/debug
engine_control_debug_build_target = engine_control_debug

engine_control_compiler = 
engine_control_config_flags = 
engine_control_linker_flags = 
engine_control_build_path = 
engine_control_build_target = 

ifeq ($(debug), 1)
	engine_control_compiler = $(engine_control_debug_compiler)
	engine_control_config_flags = $(engine_control_debug_config_flags)
	engine_control_linker_flags = $(engine_control_debug_linker_flags)
	engine_control_build_path = $(engine_control_debug_build_path)
	engine_control_build_target = $(engine_control_debug_build_target)
else
	engine_control_compiler = $(engine_control_release_compiler)
	engine_control_config_flags = $(engine_control_release_config_flags)
	engine_control_linker_flags = $(engine_control_release_linker_flags)
	engine_control_build_path = $(engine_control_release_build_path)
	engine_control_build_target = $(engine_control_release_build_target)
endif

# Rules

all: control_room engine_control

clean:
	rm -rf build
	mkdir -p build
	cd build; mkdir -p control_room/release; mkdir -p control_room/debug; mkdir -p engine_control/release; mkdir -p engine_control/debug

control_room:  $(control_room_build_path)/main.o $(patsubst %, $(control_room_build_path)/%.o, $(control_room_filenames))
	$(control_room_compiler) $(control_room_config_flags) $(control_room_linker_flags) -o $(control_room_build_path)/$(control_room_build_target) $^

$(control_room_build_path)/main.o: $(control_room_src)/main.cpp
	$(control_room_compiler) $(control_room_config_flags) -c -o $@ $<

$(control_room_build_path)/%.o: $(control_room_src)/%.cpp $(control_room_src)/%.hpp
	$(control_room_compiler) $(control_room_config_flags) -c -o $@ $<

engine_control: $(engine_control_build_path)/main.o $(patsubst %, $(engine_control_build_path)/%.o, $(engine_control_filenames))
	$(engine_control_compiler) $(engine_control_config_flags) $(engine_control_linker_flags) -o $(engine_control_build_path)/$(engine_control_build_target) $^

$(engine_control_build_path)/main.o: $(engine_control_src)/main.cpp 
	$(engine_control_compiler) $(engine_control_config_flags) -c -o $@ $<

$(engine_control_build_path)/%.o: $(engine_control_src)/%.cpp $(engine_control_src)/%.hpp
	$(engine_control_compiler) $(engine_control_config_flags) -c -o $@ $<






