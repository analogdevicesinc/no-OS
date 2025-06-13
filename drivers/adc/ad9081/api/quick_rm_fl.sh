
for FILE in *; do tail -n +2 $FILE > tmp_$FILE && mv tmp_$FILE $FILE ; done
