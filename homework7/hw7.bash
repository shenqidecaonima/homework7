#!/bin/bash

#PBS -N hw07_Marcum_08
#PBS -l walltime=11:00:00
#PBS -l mem=2GB 
#PBS -l nodes=1:ppn=8
#PBS -S /bin/bash 
#PBS -j oe

cd $PBS_O_WORKDIR

export OMP_NUM_THREADS=$PBS_NP

echo "Test"
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result.png true 75 16

echo "Test Case 1"
/usr/bin/time -v ./homework7 Mammogram.png Cancer_mask.png result1.png true 75 32
/usr/bin/time -v ./homework7 Mammogram.png Cancer_mask.png result1.png true 75 32
/usr/bin/time -v ./homework7 Mammogram.png Cancer_mask.png result1.png true 75 32
/usr/bin/time -v ./homework7 Mammogram.png Cancer_mask.png result1.png true 75 32
/usr/bin/time -v ./homework7 Mammogram.png Cancer_mask.png result1.png true 75 32

echo "Test Case 2"
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result2.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result2.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result2.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result2.png true 75 16
/usr/bin/time -v ./homework7 TestImage.png and_mask.png result2.png true 75 16


echo "Test Case 3"
/usr/bin/time -v ./homework7 MiamiMarcumCenter.png WindowPane_mask.png result3.png true 50 64 
/usr/bin/time -v ./homework7 MiamiMarcumCenter.png WindowPane_mask.png result3.png true 50 64
/usr/bin/time -v ./homework7 MiamiMarcumCenter.png WindowPane_mask.png result3.png true 50 64
/usr/bin/time -v ./homework7 MiamiMarcumCenter.png WindowPane_mask.png result3.png true 50 64
/usr/bin/time -v ./homework7 MiamiMarcumCenter.png WindowPane_mask.png result3.png true 50 64

