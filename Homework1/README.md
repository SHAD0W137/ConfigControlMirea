All functionality works with single main.py

While using Windows CMD, go to main.py directory
(for instance, "C:\test>" )
![image](https://github.com/user-attachments/assets/c608f69a-7946-43d3-8a5d-dfeacfe90645)

Then type in this command:
python main.py --hostname null --filesystem null --logfile null --startup_script null --generate_files
All necessary demo files are now generated
![image](https://github.com/user-attachments/assets/a76bc51f-9bc1-4f4c-aa81-f50abf42685c)
![image](https://github.com/user-attachments/assets/62ef7459-fe7c-4b47-ab6e-0e5b900ca682)

Now we can actually start the emulator:
python main.py --hostname shad0w137 --filesystem C:\test\virtual_fs.tar --logfile C:\test\log.csv --startup_script C:\test\startup_script.sh

All of the required features have been tested. Tests were successful. 
![image](https://github.com/user-attachments/assets/f315e664-1fb8-4abc-b77f-6c45b193cd0b)

strtup_script contains 
"""
ls
cd subdir
ls
"""

The actual task:
Группа ИКБО-20-23
Вариант №6
Задание №1
Разработать эмулятор для языка оболочки ОС. Необходимо сделать работу 
эмулятора как можно более похожей на сеанс shell в UNIX-подобной ОС. 
Эмулятор должен запускаться из реальной командной строки, а файл с 
виртуальной файловой системой не нужно распаковывать у пользователя. 
Эмулятор принимает образ виртуальной файловой системы в виде файла формата 
tar. Эмулятор должен работать в режиме CLI.
Ключами командной строки задаются:
• Имя компьютера для показа в приглашении к вводу.
• Путь к архиву виртуальной файловой системы.
• Путь к лог-файлу.
• Путь к стартовому скрипту.
Лог-файл имеет формат csv и содержит все действия во время последнего 
сеанса работы с эмулятором. Для каждого действия указаны дата и время.
Стартовый скрипт служит для начального выполнения заданного списка 
команд из файла.
Необходимо поддержать в эмуляторе команды ls, cd и exit, а также 
следующие команды:
1. cp.
2. tail.
Все функции эмулятора должны быть покрыты тестами, а для каждой из 
поддерживаемых команд необходимо написать 2 теста.
