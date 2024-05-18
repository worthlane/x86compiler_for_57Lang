if [ "$#" -ne 1 ]; then
    echo "EXPECTED: $0 <file.txt> [flags]"
    exit 1
fi

filename=$(basename -- "$1")
extension="${filename##*.}"
filename="${filename%.*}"

asm_flag=$2

./front $filename.$extension
./mid
./back $asm_flag

chmod +x $filename
