while read -r line; do echo $line | iconv -f utf8 -t big5 | iconv -f big5  -t gb2312 | iconv -f gb2312 -t utf8; done
