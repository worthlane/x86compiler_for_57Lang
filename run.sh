if [ "$#" -lt 1 ]; then
    echo "EXPECTED: $0 <file.txt> [flags]"
    exit 1
fi

filename=$(basename -- "$1")
extension="${filename##*.}"
filename="${filename%.*}"

flag1=$2
flag2=$3


./front $filename.$extension $flag1 $flag2
./mid $flag1 $flag2
./back $flag1 $flag2

chmod +x $filename
