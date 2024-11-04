### Интерпретатор для учебного языка ASSEMBLER. 
Входные данные: текстовый файл с кодом.
Выходные данные: двоичное представление программы (промежуточный этап), лог операций, итог состояния памяти

```
Разработать ассемблер и интерпретатор для учебной виртуальной машины 
(УВМ). Система команд УВМ представлена далее. 
Для ассемблера необходимо разработать читаемое представление команд 
УВМ. Ассемблер принимает на вход файл с текстом исходной программы, путь к 
которой задается из командной строки. Результатом работы ассемблера является 
бинарный файл в виде последовательности байт, путь к которому задается из 
командной строки. Дополнительный ключ командной строки задает путь к файлу
логу, в котором хранятся ассемблированные инструкции в духе списков 
“ключ=значение”, как в приведенных далее тестах. 
Интерпретатор принимает на вход бинарный файл, выполняет команды УВМ 
и сохраняет в файле-результате значения из диапазона памяти УВМ. Диапазон 
также указывается из командной строки. 
Форматом для файла-лога и файла-результата является xml. 
Необходимо реализовать приведенные тесты для всех команд, а также 
написать и отладить тестовую программу.
```

### Операции:
```
- Загрузка константы 
- Чтение из памяти 
- Запись в память 
- Унарная операция: abs()
```

Тестовая программа 
Выполнить поэлементно операцию abs() над вектором длины 8. Результат 
записать в новый вектор.

### Структура языка:
```
<операция> <параметр> <параметр>
<операция> <параметр> <параметр>
...
```
### Существующие команды:
```
var <data> <registry>				    // writing data[11] to registry[5]
get <memory> <registry>			    // writing data from memory[24] to registry[5]
put <registry> <memory>				  // writing data from registry[5] to memory[24]
abs <registry> <memory>			  	// writing data from registry[5] to memory[24] with abs()
```

### Фото-отчёт:
![image](https://github.com/user-attachments/assets/18971a5e-8067-44f6-8caa-06017e0d6f47)