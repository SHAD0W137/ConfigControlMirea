### This is a code parser from custom Configuration Language to TOML language

### Task:
```
Разработать инструмент командной строки для учебного конфигурационного 
языка, синтаксис которого приведен далее. Этот инструмент преобразует текст из 
входного формата в выходной. Синтаксические ошибки выявляются с выдачей 
сообщений. 
Входной текст на учебном конфигурационном языке принимается из 
файла, путь к которому задан ключом командной строки. Выходной текст на 
языке toml попадает в файл, путь к которому задан ключом командной строки. 
Однострочные комментарии: 
\ Это однострочный комментарий 
Словари: 
@{ 
имя = значение; 
имя = значение; 
имя = значение; 
... 
} 
Имена: 
[A-Z]+ 
30 
Значения: 
• Числа. 
• Словари. 
Объявление константы на этапе трансляции: 
значение -> имя; 
Вычисление константы на этапе трансляции: 
|имя| 
Результатом вычисления константного выражения является значение. 
Все конструкции учебного конфигурационного языка (с учетом их 
возможной вложенности) должны быть покрыты тестами. Необходимо показать 3 
примера описания конфигураций из разных предметных областей.
```
### Now, input example:
```
\ CL example
\ This are comments
8800 -> CONST;
20 -> YACONST;
DATABASE = @{
	EXAMPLE = 123;
	ANOTHER = |YACONST|;
	SUBDATA = @{
		YETANOTHEREXAMPLE = 148;
		ANDAGAIN = |CONST|;
	}
}
```
### And output example, created with application:
```
# CL example
# This are comments
CONST = 8800
YACONST = 20

[DATABASE]
EXAMPLE = 123
ANOTHER = 20

[DATABASE.SUBDATA]
YETANOTHEREXAMPLE = 148
ANDAGAIN = 8800
```
### Screenshot report:
![image](https://github.com/user-attachments/assets/6c4e72dc-ed6c-4389-b074-f9c4d37c040a)

