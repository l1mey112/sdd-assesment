DEMOS_SRC = $(wildcard src/*.c)

CFLAGS = 
FCFLAGS =

ifeq ($(PROD),1)
	CFLAGS += -Oz --closure=1 -flto
	FCFLAGS += -sMINIFY_HTML -sEVAL_CTORS=2
endif

.PHONY: all
all: build/ public/ public/index.html

.PHONY: run
run: all
	emrun public/index.html

.PHONY: clean
clean:
	find public/ \
		! -name 'main.css' \
		! -name 'jl.svg' \
		-type f -exec rm -f {} +
	rm -rf build/

build/:
	mkdir build
public/:
	mkdir public

public/index.html: $(DEMOS_SRC) build/libsokol.a build/libcimgui.a src/demos.h
	emcc -o $@ $(DEMOS_SRC) $(CFLAGS) $(FCFLAGS) \
		--shell-file sokol/shell.html \
		-sASSERTIONS=0 \
		-sMALLOC=emmalloc \
		-Isokol -Icimgui -Iinclude \
		--use-preload-plugins --preload-file ./assets@/ \
		build/libsokol.a build/libcimgui.a
ifeq ($(PROD),1)
	wasm-opt $(patsubst %.html, %.wasm, $@) -o $(patsubst %.html, %.wasm, $@) \
		--dce --gufa --flatten --rereloop -Oz --gufa --dce -Oz --converge -Oz
endif

build/libsokol.a:
	emcc -c sokol/sokol.c -o build/libsokol.o $(CFLAGS) -std=gnu11 -Isokol -Icimgui
	emar qc build/libsokol.a build/libsokol.o
	emranlib build/libsokol.a

build/libcimgui.a:
	em++ -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/cimgui.cpp -o build/cimgui.o
	em++ -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/imgui/imgui.cpp -o build/imgui.o
	em++ -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/imgui/imgui_widgets.cpp -o build/imgui_widgets.o
	em++  -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/imgui/imgui_draw.cpp -o build/imgui_draw.o
	em++ -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/imgui/imgui_tables.cpp -o build/imgui_tables.o
	em++ -std=gnu++11 -Isokol -Icimgui $(CFLAGS) \
		-c cimgui/imgui/imgui_demo.cpp -o build/imgui_demo.o
		
	emar qc build/libcimgui.a \
		build/cimgui.o \
		build/imgui.o \
		build/imgui_widgets.o \
		build/imgui_draw.o \
		build/imgui_tables.o \
		build/imgui_demo.o \
	
	emranlib build/libcimgui.a
