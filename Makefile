
.PHONY: build
build:
	mkdir -p build
	cmake -E chdir build cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
	cmake --build build

.PHONY: install
install:
	cmake --build build --target install
