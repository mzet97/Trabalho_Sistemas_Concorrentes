@echo off
echo === Compilando Etapa 2 para Windows ===
gcc etapa2_windows.c -o etapa2_windows.exe -lm

if %errorlevel% equ 0 (
    echo Compilacao bem-sucedida!
    echo.
    echo === Executando Etapa 2 ===
    etapa2_windows.exe
) else (
    echo Erro na compilacao!
    pause
)