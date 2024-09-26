import os
import tarfile
import csv
import argparse
import time
from datetime import datetime
import shutil
import sys


class ShellEmulator:
    def __init__(self, hostname, fs_archive, log_file, startup_script):
        self.hostname = hostname
        self.log_file = log_file
        self.current_directory = '/'
        self.fs_root = '/tmp/shell_emulator_fs'  # Виртуальная файловая система во временной папке

        # Распаковка виртуальной файловой системы
        self.extract_fs(fs_archive)

        # Открытие лог-файла для записи
        self.log = open(log_file, mode='a', newline='')
        self.logger = csv.writer(self.log)

        # Запуск стартового скрипта, если он задан
        if startup_script:
            self.execute_startup_script(startup_script)

    def extract_fs(self, fs_archive):
        """Распаковывает виртуальную файловую систему в заданную папку."""
        if os.path.exists(self.fs_root):
            shutil.rmtree(self.fs_root)
        os.makedirs(self.fs_root)

        with tarfile.open(fs_archive, 'r') as tar:
            tar.extractall(path=self.fs_root)

    def log_action(self, action):
        """Записывает действия пользователя в лог-файл."""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        self.logger.writerow([timestamp, action])
        self.log.flush()

    def execute_startup_script(self, script_path):
        """Выполняет команды из стартового скрипта."""
        with open(script_path, 'r') as script:
            for line in script:
                self.run_command(line.strip())

    def prompt(self):
        """Показывает приглашение к вводу."""
        return f"{self.hostname}:{self.current_directory}$ "

    def run(self):
        """Основной цикл эмулятора."""
        try:
            while True:
                command = input(self.prompt())
                self.run_command(command)
        except KeyboardInterrupt:
            self.exit()

    def run_command(self, command):
        """Обрабатывает ввод пользователя."""
        self.log_action(command)

        if command.startswith('ls'):
            self.ls()
        elif command.startswith('cd'):
            self.cd(command.split(' ')[1] if len(command.split()) > 1 else '/')
        elif command == 'exit':
            self.exit()
        elif command.startswith('cp'):
            _, src, dst = command.split(' ')
            self.cp(src, dst)
        elif command.startswith('tail'):
            _, file = command.split(' ')
            self.tail(file)
        else:
            print(f"Команда '{command}' не поддерживается.")

    def ls(self):
        """Команда ls."""
        path = os.path.join(self.fs_root, self.current_directory.strip('/'))
        if os.path.exists(path):
            files = os.listdir(path)
            print('  '.join(files))
        else:
            print(f"Путь '{self.current_directory}' не существует.")

    def cd(self, path):
        """Команда cd."""
        # Преобразование пути в абсолютный (относительно корневой директории файловой системы)
        if path.startswith('/'):
            # Если путь абсолютный (начинается с '/')
            new_dir = os.path.join(self.fs_root, path.strip('/'))
        else:
            # Если путь относительный (не начинается с '/')
            new_dir = os.path.join(self.fs_root, self.current_directory.strip('/'), path.strip('/'))

        # Проверка, существует ли новая директория
        if os.path.isdir(new_dir):
            # Если существует, меняем текущую директорию относительно корня виртуальной ФС
            if path.startswith('/'):
                # Абсолютный путь
                self.current_directory = '/' + path.strip('/')
            else:
                # Относительный путь
                self.current_directory = os.path.join(self.current_directory, path).replace('\\', '/')
        else:
            print(f"Директория '{path}' не существует.")

    def cp(self, src, dst):
        """Команда cp."""
        src_path = os.path.join(self.fs_root, self.current_directory.strip('/'),  src.strip('/'))
        dst_path = os.path.join(self.fs_root, self.current_directory.strip('/'), dst.strip('/'))
        if os.path.exists(src_path):
            shutil.copy(src_path, dst_path)
        else:
            print(f"Файл '{src}' не существует.")

    def tail(self, file):
        """Команда tail."""
        file_path = os.path.join(self.fs_root, self.current_directory.strip('/'), file.strip('/'))
        if os.path.exists(file_path):
            with open(file_path, 'r') as f:
                lines = f.readlines()
                print(''.join(lines[-10:]))
        else:
            print(f"Файл '{file}' не существует.")

    def exit(self):
        """Команда exit."""
        print("Выход из эмулятора.")
        self.log.close()
        sys.exit(0)


def generate_files():
    """Функция для генерации всех нужных файлов в директории."""
    
    # Creating virtual filesystem
    virtual_fs_dir = 'virtual_fs'
    if not os.path.exists(virtual_fs_dir):
        os.makedirs(virtual_fs_dir)
        # Creating test files and directories
        with open(os.path.join(virtual_fs_dir, 'file1.txt'), 'w') as f:
            f.write("Это содержимое файла 1.")
        with open(os.path.join(virtual_fs_dir, 'file2.txt'), 'w') as f:
            f.write("Это содержимое файла 2.")
        os.makedirs(os.path.join(virtual_fs_dir, 'subdir'))
        with open(os.path.join(virtual_fs_dir, 'subdir', 'file3.txt'), 'w') as f:
            f.write("Это содержимое файла в поддиректории.")
        
    # Tar archive generation
    fs_tar = 'virtual_fs.tar'
    with tarfile.open(fs_tar, 'w') as tar:
        tar.add(virtual_fs_dir, arcname=os.path.basename(virtual_fs_dir))

    print(f"Создан архив виртуальной файловой системы: {fs_tar}")

    # Log-file generation
    log_file = 'log.csv'
    with open(log_file, mode='w', newline='') as csvfile:
        log_writer = csv.writer(csvfile)
        log_writer.writerow(['Timestamp', 'Action'])
        log_writer.writerow([datetime.now().strftime('%Y-%m-%d %H:%M:%S'), 'Initial log entry'])
    
    print(f"Создан лог-файл: {log_file}")

    # Startup script generation
    startup_script = 'startup_script.sh'
    with open(startup_script, 'w') as f:
        f.write('ls\n')
        f.write('cd subdir\n')
        f.write('ls\n')
    
    print(f"Создан стартовый скрипт: {startup_script}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Эмулятор shell для UNIX-подобных систем.")
    parser.add_argument("--hostname", required=True, help="Имя компьютера для показа в приглашении к вводу.")
    parser.add_argument("--filesystem", required=True, help="Путь к архиву виртуальной файловой системы.")
    parser.add_argument("--logfile", required=True, help="Путь к лог-файлу (формат CSV).")
    parser.add_argument("--startup_script", help="Путь к стартовому скрипту для выполнения команд.")
    parser.add_argument("--generate_files", action="store_true", help="Сгенерировать все необходимые файлы в текущей директории.")

    args = parser.parse_args()

    if args.generate_files:
        generate_files()
    else:
        shell = ShellEmulator(args.hostname, args.filesystem, args.logfile, args.startup_script)
        shell.run()
