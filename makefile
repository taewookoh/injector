.PHONY: check_distorm test clean $(BENCH) $(BENCH)-clean

DISTORM_DIR:=$(CURDIR)/distorm3

RESIL_BASE=/home/hanjunk/
BASEDIR=$(RESIL_BASE)/injector

BENCHDIR=$(BASEDIR)/polybench-c-3.2/linear-algebra/kernels/$(BENCH)/
RESULTDIR=$(BASEDIR)/$(BENCH).results
SCRIPTDIR=$(BASEDIR)/script

all: injector

clean: 
	rm -f *.o injector

$(BENCH): $(BENCH)-clean guard-DISTORM_DIR guard-BENCH setup injector $(BENCHDIR)/$(BENCH).check.exe $(RESULTDIR)/$(BENCH).check.time $(RESULTDIR)/$(BENCH).base.out
	$(eval reftime := $(shell cat $(RESULTDIR)/$(BENCH).check.time))
	$(eval timeout := $(shell echo $(reftime)\*512 | bc))
	#/usr/bin/time --output=$(BENCH).exp.time -f "%e" python $(SCRIPTDIR)/run.py -i ./injector -p $(BENCHDIR)/$(BENCH).check.exe -c $(SCRIPTDIR)/cmp.py -r $(RESULTDIR)/$(BENCH).base.out -t $(timeout) -o $(RESULTDIR)

$(BENCH)-clean:
	cd $(BENCHDIR); \
	make clean; \
	cd -

setup:
	mkdir -p $(RESULTDIR)

injector: injector.o decoder.o bpmanager.o flipper.o regmap.o
	g++ -O3 -o $@ $^ -L$(DISTORM_DIR)/build/lib/distorm3 -ldistorm3

%.o: %.cpp
	g++ -O3 -c -o $@ $< -I$(DISTORM_DIR)/include

guard-%:
	@if [ "${${*}}" = "" ]; then \
    echo "Environment variable $* not set"; \
    exit 1; \
  fi

$(BENCHDIR)/$(BENCH).check.exe:
	cd $(BENCHDIR); \
	make $(BENCH).check.exe FIFLAG=-DFAULT_INJECTION; \
	cd -

$(BENCHDIR)/$(BENCH).exe:
	cd $(BENCHDIR); \
	make $(BENCH).exe; \
	cd -

$(RESULTDIR)/$(BENCH).check.time: $(BENCHDIR)/$(BENCH).check.exe
	/usr/bin/time --output=$@ -f "%e" $(BENCHDIR)/$(BENCH).check.exe 2> /dev/null

$(RESULTDIR)/$(BENCH).base.out : $(BENCHDIR)/$(BENCH).exe
	$(BENCHDIR)/$(BENCH).exe 2> $@
