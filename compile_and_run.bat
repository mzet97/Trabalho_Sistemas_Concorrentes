@echo off
echo === Compilando Etapa 1 para Windows ===
gcc etapa1_windows.c -o etapa1_windows.exe -lm

if %errorlevel% equ 0 (
    echo Compilacao bem-sucedida!
    echo.
    echo === Executando Etapa 1 ===
    etapa1_windows.exe
) else (
    echo Erro na compilacao!
    pause
)