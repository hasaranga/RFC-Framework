@echo off
setlocal

echo [1/3] Generating file list...
powershell -NoProfile -Command "[System.IO.File]::WriteAllLines('%CD%\filelist.txt', (Get-ChildItem -Recurse -Path '%CD%\rfc' -Include *.h,*.hpp,*.cpp,*.cc -File | Where-Object { $_.FullName -notmatch '\\(build|out|\.vs|x64|Debug|Release|qrcodegen|plutosvg|totpmcu)\\' } | Select-Object -ExpandProperty FullName))"

if not exist filelist.txt (
    echo ERROR: filelist.txt was not created.
    exit /b 1
)

if exist symbols.json (
    del symbols.json
)

echo [2/3] Running ctags...
ctags --output-format=json --fields=+naSiKl --extras=-F --c++-kinds=+cdfgmstu -o symbols.json -L filelist.txt

if not exist symbols.json (
    echo ERROR: symbols.json was not created. Check ctags output above.
    exit /b 1
)

echo [3/3] Generating symbols.md...
python generate_symbols.py symbols.json symbols.md rfc

if errorlevel 1 (
    echo ERROR: generate_symbols.py failed.
    exit /b 1
)

if exist filelist.txt (
    del filelist.txt
)

if exist symbols.json (
    del symbols.json
)

echo Done. symbols.md generated.
endlocal
