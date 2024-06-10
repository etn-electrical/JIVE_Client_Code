@echo off

echo %TIME%
echo %TIME% > duration.txt
cd C:\Pythia\Babelfish\Tools\PC-lint

echo BCM.lnt
echo BCM.lnt > C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv
C:\AccuRev\PC-lint\lint-nt.exe C:\Pythia\Babelfish\Tools\PC-lint\BCM.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo %TIME%
echo MM.lnt
echo MM.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv
C:\AccuRev\PC-lint\lint-nt.exe C:\Pythia\Babelfish\Tools\PC-lint\MM.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo BUI.lnt
echo BUI.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv
C:\AccuRev\PC-lint\lint-nt.exe C:\Pythia\Babelfish\Tools\PC-lint\BUI.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo %TIME%
echo ETH.lnt
echo ETH.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv
C:\AccuRev\PC-lint\lint-nt.exe C:\Pythia\Babelfish\Tools\PC-lint\ETH.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo %TIME%
echo PROFI.lnt
echo PROFI.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv
C:\AccuRev\PC-lint\lint-nt.exe C:\Pythia\Babelfish\Tools\PC-lint\PROFI.lnt >> C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo %TIME%
echo Remove Duplicates
Remove_overlap.pl C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv

echo Find errors
perl -ne "print if /Worst/" C:\Pythia\Babelfish\Tools\PC-lint\Overall_PC-lint_output.csv > C:\Pythia\Babelfish\Tools\PC-lint\Finish.txt

echo %TIME% >> duration.txt