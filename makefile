
# Control Room
control_room_src = "src/control_room";
# Names of files to be compiled for the control room, without the .cpp extension
control_room_filenames = [main];

control_room_release_compiler = "g++"
control_room_release_config_flags="-O3 -s -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused" 
control_rool_release_linker_flags =""
control_room_release_build_path = "build/control_room/release"
control_room_release_build_target = "control_room_release"

control_room_debug_compiler = "g++"
control_room_debug_config_flags = "-g -O0 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused"
control_room_debug_linker_flags = ""
control_room_debug_build_path = "build/control_room/debug"
control_room_debug_build_target = "control_room_debug"


# Engine Control
engine_control_src = "src/engine_control";
# Names of files to be compiled for the engine control, without the .cpp extension
engine_control_filenames = [main];

engine_control_release_compiler = "g++"
engine_control_release_config_flags="-O3 -s -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused"
engine_control_release_linker_flags =""
engine_control_release_build_path = "build/engine_control/release"

engine_control_debug_compiler = "g++"
engine_control_debug_config_flags = "-g -O0 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused"
engine_control_debug_linker_flags = ""
engine_control_debug_build_path = "build/engine_control/debug"

# Macros
define make-control-room-debug-targets
	$(control_room_debug_build_path)/$(1).o: $(control_room_src)/$(1).cpp
		$(control_room_debug_compiler) $(control_room_debug_config_flags) -c $(control_room_src)/$(1).cpp -o $(control_room_debug_build_path)/$(1).o
endef

define make-engine-control-debug-targets
	$(engine_control_debug_build_path)/$(1).o: $(engine_control_src)/$(1).cpp
		$(engine_control_debug_compiler) $(engine_control_debug_config_flags) -c $(engine_control_src)/$(1).cpp -o $(engine_control_debug_build_path)/$(1).o
endef

define make-control-room-release-targets
	$(control_room_release_build_path)/$(1).o: $(control_room_src)/$(1).cpp
		$(control_room_release_compiler) $(control_room_release_config_flags) -c $(control_room_src)/$(1).cpp -o $(control_room_release_build_path)/$(1).o
endef

define make-engine-control-release-targets
	$(engine_control_release_build_path)/$(1).o: $(engine_control_src)/$(1).cpp
		$(engine_control_release_compiler) $(engine_control_release_config_flags) -c $(engine_control_src)/$(1).cpp -o $(engine_control_release_build_path)/$(1).o
endef

# Targets

all_debug: control_room_debug engine_control_debug
	echo "Debug build complete"

all_release: control_room_release engine_control_release 
	echo "Release build complete"

$(foreach element, $(control_room_filenames),$(eval $(call make-control-room-debug-targets, $(element))))
$(foreach element, $(engine_control_filenames),$(eval $(call make-engine-control-debug-targets, $(element))))
$(foreach element, $(control_room_filenames),$(eval $(call make-control-room-release-targets, $(element))))
$(foreach element, $(engine_control_filenames),$(eval $(call make-engine-control-release-targets, $(element))))

build_folders: 
	mkdir -p $(control_room_debug_build_path)
	mkdir -p $(control_room_release_build_path)
	mkdir -p $(engine_control_debug_build_path)
	mkdir -p $(engine_control_release_build_path)

control_room_debug: $(foreach element,$(control_room_filenames),$(control_room_debug_build_path)/$(element).o)
	$(control_room_debug_compiler) $(control_room_debug_config_flags) $(control_room_debug_build_path)/*.o -o $(control_room_debug_build_path)/$(control_room_debug_build_target)

engine_control_debug: $(foreach element,$(engine_control_filenames),$(engine_control_debug_build_path)/$(element).o)
	$(engine_control_debug_compiler) $(engine_control_debug_config_flags) $(engine_control_debug_build_path)/*.o -o $(engine_control_debug_build_path)/$(engine_control_debug_build_target)


control_room_release: $(foreach element,$(control_room_filenames),$(control_room_release_build_path)/$(element).o)
	$(control_room_release_compiler) $(control_room_release_config_flags) $(control_room_release_build_path)/*.o -o $(control_room_release_build_path)/$(control_room_release_build_target)

engine_control_release: $(foreach element,$(engine_control_filenames),$(engine_control_release_build_path)/$(element).o)
	$(engine_control_release_compiler) $(engine_control_release_config_flags) $(engine_control_release_build_path)/*.o -o $(engine_control_release_build_path)/$(engine_control_release_build_target)
