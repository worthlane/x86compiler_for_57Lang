if [ "$#" -lt 1 ]; then
    echo "EXPECTED: $0 <file.txt> [flags]"
    exit 1
fi

filename=$(basename -- "$1")
extension="${filename##*.}"
filename="${filename%.*}"


./front "$@"

shift

./mid "$@"
./back "$@"

chmod +x $filename
