#!/bin/bash
for f in test/test_programs/*.tas;
do
filename=$(basename -- "$f");
filename="${filename%.*}";
python3 src/triangulate/triangulate.py test/test_programs/$filename.tas -o test/tern/$filename.tri;
done 