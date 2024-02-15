#!/bin/bash

rm outputData.txt
rm *.dat
make ilthpcm

./ilthpcm.out mainInputs.txt weatherData.txt outputData.txt