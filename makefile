.PHONY: check_distorm test clean

AFTER_BP_ADDR=$(shell objdump -D ./test/target | grep "<__after_bp>:" | awk '{print $$1}')
ORG_INST_ADDR=$(shell objdump -D ./test/target | grep "<__org_inst>:" | awk '{print $$1}')
TARGET_ADDR=$(shell objdump -D ./test/target | grep "<__target_dynamic_instance>:" | awk '{print $$1}')

all: guard-DISTORM_DIR injector

clean: 
	rm -f *.o injector

injector: injector.o decoder.o bpmanager.o flipper.o regmap.o
	g++ -O3 -o $@ $^ -L$(DISTORM_DIR)/build/lib.linux-x86_64-2.6/distorm3 -ldistorm3

test: injector 
	./injector --type a --prog ./test/target --breakpoint 0x400b27 --target 0 --after_bp_addr 0x$(AFTER_BP_ADDR) --org_inst_addr 0x$(ORG_INST_ADDR) --target_addr 0x$(TARGET_ADDR)

%.o: %.cpp
	g++ -O3 -c -o $@ $< -I$(DISTORM_DIR)/include

guard-%:
	@if [ "${${*}}" = "" ]; then \
    echo "Environment variable $* not set"; \
    exit 1; \
  fi
