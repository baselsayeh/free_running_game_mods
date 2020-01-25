#!/bin/bash

command -v stat >/dev/null 2>&1 || { echo >&2 "I require stat but it's not installed.  Aborting."; exit 1; }
command -v gcc >/dev/null 2>&1 || { echo >&2 "I require gcc but it's not installed.  Aborting."; exit 1; }
command -v ffmpeg >/dev/null 2>&1 || { echo >&2 "I require ffmpeg but it's not installed.  Aborting."; exit 1; }

if [[ ! -f "eur.pc" ]]; then
    echo "eur.pc is required to extract the music!"
	echo "copy it into this folder, then re run this script"
	exit 1
fi

eurfilesize=$(stat --format=%s "eur.pc")
if [[ "$eurfilesize" -ne "502065280" ]]; then
	echo "Your eur.pc file seems damaged, its size is $eurfilesize, and should be 502065280"
	echo "Continuing anyway..."
fi

gcc music_extractor.c -o music_extractor
if [[ $? -ne 0 ]]; then
	echo "Cannot compile music_extractor.c!"
	echo "Exiting..."
	exit 1
fi

rm -Rf "out"
rm -Rf "*.raw"
mkdir out

./music_extractor eur.pc "6 (Zongamin - Spiral).raw" 15018032 9526272
./music_extractor eur.pc "18 (Wagon Christ - Chri Chana).raw" 24544304 12156416
./music_extractor eur.pc "14 (Clyde - Broken Slang).raw" 36700720 15042344
./music_extractor eur.pc "2 (Evil Nine - Happy Ending).raw" 51743064 11562496
./music_extractor eur.pc "10 (Furry Phreaks - Paw Power).raw" 63305560 12941576
./music_extractor eur.pc "23 (Skrum - Akting Up).raw" 76247136 10157128
./music_extractor eur.pc "4 (Mr Scruff - Champion Nibble).raw" 86404264 8347648
./music_extractor eur.pc "8 (EL-P - Constellation).raw" 94751912 10885376
./music_extractor eur.pc "15 (Martin Iveson - Breakin' Techno).raw" 105637288 7298760
./music_extractor eur.pc "21 (Martin Iveson - Jump Street Dub).raw" 112936048 10740800
./music_extractor eur.pc "3 (Martin Iveson - Jump).raw" 123676848 7401536
./music_extractor eur.pc "9 (Martin Iveson - Jungle Tech).raw" 131078384 10497792
./music_extractor eur.pc "12 (Martin Iveson - Whodini Boyz).raw" 141576176 11424384
./music_extractor eur.pc "20 (Martin Iveson - 5th Avenue).raw" 153000560 11866112
./music_extractor eur.pc "22 (Martin Iveson - B Boyz).raw" 164866672 11161600
./music_extractor eur.pc "17 (Martin Iveson - Breakin').raw" 176028272 7949032
./music_extractor eur.pc "11 (Martin Iveson - Hot Beez).raw" 183977304 8263120
./music_extractor eur.pc "5 (Martin Iveson - Mustard 9).raw" 192240424 8650752
./music_extractor eur.pc "1 (Martin Iveson - Foolish).raw" 200891176 5997600
./music_extractor eur.pc "16 (Martin Iveson - Free Thoughts).raw" 206888776 6584064
./music_extractor eur.pc "19 (Martin Iveson - Midnight Flight).raw" 213472840 7528448
./music_extractor eur.pc "13 (Martin Iveson - Slo Mo).raw" 221001288 9541248
./music_extractor eur.pc "7 (Martin Iveson - Zen).raw" 230542536 8100880

for file in *.raw; do
	name=${file%%.raw}
	ffmpeg -f s16le -acodec pcm_s16le -ar 44100 -ac 2 -i "$file" "out/$name.wav"
	rm -f "$file"
done

echo "Your extracted music should be in out folder now"

