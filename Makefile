FRONT = makefront
BACK  = makeback
MID   = makemid

all: front back mid proc

proc:
	cd spu && make && cd ..

front:
	make -f $(FRONT) makedirs && make -f $(FRONT)

back:
	make -f $(BACK) makedirs && make -f $(BACK)

mid:
	make -f $(MID) makedirs && make -f $(MID)

.PHONY: clean run

clean:
	make -f $(FRONT) clean && make -f $(BACK) clean && make -f $(MID) clean && cd spu && make clean && cd ..

run:
	./spu/asm/asm spu/assets/asm_code.txt spu/assets/byte_code.txt spu/assets/byte_code.bin
	./spu/spu/spu spu/assets/byte_code.bin

