FRONT = Makefront
BACK  = Makeback
MID   = Makemid

all: front back mid

front:
	make -f $(FRONT) makedirs && make -f $(FRONT)

back:
	make -f $(BACK) makedirs && make -f $(BACK)

mid:
	make -f $(MID) makedirs && make -f $(MID)

.PHONY: clean run

clean:
	make -f $(FRONT) clean && make -f $(BACK) clean && make -f $(MID) clean

run:
	./spu/asm/asm spu/assets/asm_code.txt spu/assets/byte_code.txt spu/assets/byte_code.bin
	./spu/spu/spu spu/assets/byte_code.bin

