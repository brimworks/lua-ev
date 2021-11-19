.PHONY: build test
.DEFAULT: test

build:
	luarocks make rockspec/lua-ev-scm-1.rockspec

test: build
	for f in test/test_*.lua; do \
		echo "	testing $$f"; \
		if ! lua $$f test/ . | ./tapview; then \
			echo "$$f test failure, see above"; \
			exit 1; \
		fi; \
	done
