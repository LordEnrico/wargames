#!/bin/bash

#Install dependencies
sudo apt install libncurses5-dev
#If you will only be running the software locally there is no need to install telnet and telnetd
sudo apt install telnet
sudo apt install telnetd

#Install Lynx text based web-browser
sudo apt install lynx

#Install Vidtex Viewdata Terminal
wget "https://github.com/simonlaszcz/vidtex/blob/master/releases/vidtex-1.3.0.tar.gz?raw=true" -O "vidtex-1.3.0.tar.gz"
tar xvf vidtex-1.3.0.tar.gz
cd vidtex-1.3.0
./configure
sudo make install
cd ..

#Install Viewdata Fonts
wget https://bjh21.me.uk/bedstead/bedstead.otf
wget https://galax.xyz/TELETEXT/MODE7GX3.TTF
sudo cp bedstead.otf /usr/share/fonts/truetype/
sudo cp MODE7GX3.TTF /usr/share/fonts/truetype/

#Install cool-retro-term
sudo apt install cool-retro-term

#Install shell-gpt & WOPR role (needs to be enabled in wopr.c)
#echo you are running Python Version
#python --version
#sudo apt install espeak
#sudo apt install python3-pip
#pip install shell-gpt
#cp C/src/WOPR.json ~/.config/shell_gpt/roles

#Compile and install C code
gcc C/src/imsai8080.c -o ./imsai8080
gcc C/src/school.c -o ./school
gcc C/src/dialer.c -o ./dialer
gcc C/src/pan-am.c -o ./pan-am
gcc C/src/bank.c -o ./bank
gcc C/src/wopr.c -o ./wopr
gcc C/src/tic-tac-toe.c -o ./tic-tac-toe
gcc C/src/starwars.c -o ./starwars

#Install data files
cp C/src/*.txt ./

#Copy config files
cp C/src/imsai8080.json ./

#Make scripts executable
chmod +x ~/wargames/*.sh

echo install complete






