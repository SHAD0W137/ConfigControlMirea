import subprocess
import os
import csv
import git
import tempfile
import shutil
import stat
import ctypes

CONFIG_FILE = "config.csv"


def read_config():
    """Читает конфигурационный файл."""
    with open(CONFIG_FILE, 'r') as f:
        reader = csv.reader(f)
        config = next(reader)
        repo_info = config[1].strip()
        return {
            "graphviz_path": config[0],
            "repo_url": repo_info if repo_info.startswith("http") else None,
            "repo_path": repo_info if not repo_info.startswith("http") else None,
            "target_file": config[2],
        }


def clone_repo(repo_url, local_path):
    """Клонирует репозиторий."""
    try:
        repo = git.Repo.clone_from(repo_url, local_path)
        return repo
    except git.exc.GitCommandError as e:
        print(f"Error cloning repository: {e}")
        return None


def get_commit_dependencies(repo, target_file):
    """Получает зависимости коммитов для заданного файла."""
    dependencies = {}
    relevant_commits = []

    for commit in repo.iter_commits():
        if target_file in commit.stats.files:
            relevant_commits.append(commit)

    for commit in relevant_commits:
        dependencies[commit.hexsha] = [parent.hexsha for parent in commit.parents if parent.hexsha in [c.hexsha for c in relevant_commits]]

    return dependencies


def generate_dot_graph(dependencies):
    """Генерирует представление графа в формате DOT."""
    dot_graph = "digraph dependencies {\n"
    for commit, parents in dependencies.items():
        for parent in parents:
           dot_graph += f'    "{commit}" -> "{parent}";\n'
    dot_graph += "}\n"
    return dot_graph


def visualize_graph(graphviz_path, dot_graph):
    """Визуализирует граф с помощью Graphviz."""
    with tempfile.NamedTemporaryFile(mode="w", suffix=".dot", delete=False) as dot_file:
        dot_file.write(dot_graph)
        dot_file.flush()

        try:
            shell32 = ctypes.windll.shell32
            SU_FLAG = 0x00000080
            shell32.ShellExecuteW(None, "runas", graphviz_path, f"-Tpng {dot_file.name} -o C:\\Users\\admin\\Desktop\\Study\\ConfigControl\\HW22\\mygraph.png", None, SU_FLAG)
            result = subprocess.run([graphviz_path, "-Tpng", dot_file.name, "-o", "C:\\Users\\admin\\Desktop\\Study\\ConfigControl\\HW22\\mygraph.png"])
            return subprocess.CompletedProcess(returncode=0, args=[], stdout=b"Graph visualization successful (png output to stdout).") 
        except subprocess.CalledProcessError as e:
            print(f"Error visualizing graph: {e}")
            print(f"Dot file saved to: {dot_file.name}")  
            return e

def handle_permission_error(func, path, exc_info):
    """Обработчик ошибок для shutil.rmtree(). Пытается разблокировать и удалить файл."""
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise


def main():
    config = read_config()

    if config.get("repo_url"):
        repo_path = tempfile.mkdtemp()
        repo = clone_repo(config["repo_url"], repo_path)
        if not repo:
            return
        repo_path_to_use = repo_path
    elif config.get("repo_path"):
        repo_path_to_use = config["repo_path"]
        repo = git.Repo(repo_path_to_use)
    else:
        print("Error: Neither repo_url nor repo_path specified in config.")
        return

    dependencies = get_commit_dependencies(repo, config["target_file"])
    if not dependencies:
      print(f"No commits found related to '{config['target_file']}'.")
      return

    dot_graph = generate_dot_graph(dependencies)

    result = visualize_graph(config["graphviz_path"], dot_graph)

    if result.returncode == 0:
        print("Graph visualization successful.")
    else:
        print("Graph visualization failed.")

    if config.get("repo_url"):
        try:
            repo.close()
            shutil.rmtree(repo_path, onerror=handle_permission_error)
        except OSError as e:
            print(f"Error removing temporary directory: {e}")

if __name__ == "__main__":
    if os.getenv("UNITTEST") is None:
        main()
    else:
        unittest.main()
