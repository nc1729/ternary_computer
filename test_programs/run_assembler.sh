#!/bin/bash
for f in *.tas;
do
filename=$(basename -- "$f");
filename="${filename%.*}";
python3 ../triangulate/triangulate.py $filename.tas -o $filename.tri;
done 