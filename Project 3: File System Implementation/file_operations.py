import shutil
from pathlib import Path

# Handles all file system operations
class FileOperations:

    # Create a new file with optional content
    @staticmethod
    def create_file(file_path, content=""):
        if Path(file_path).exists():
            raise FileExistsError(f"File already exists: {file_path}")

        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return True
        except Exception as e:
            raise Exception(f"Failed to create file: {str(e)}")

    # Read and return file contents
    @staticmethod
    def read_file(file_path):
        if not Path(file_path).exists():
            raise FileNotFoundError(f"File not found: {file_path}")

        try:
            # Try UTF-8 first
            with open(file_path, 'r', encoding='utf-8') as f:
                return f.read()
        except UnicodeDecodeError:
            # Fallback to latin-1 for binary-ish files
            with open(file_path, 'r', encoding='latin-1') as f:
                return f.read()

    # Update existing file with new content
    @staticmethod
    def update_file(file_path, new_content):
        if not Path(file_path).exists():
            raise FileNotFoundError(f"File not found: {file_path}")

        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            return True
        except Exception as e:
            raise Exception(f"Failed to update file: {str(e)}")

    # Delete a file
    @staticmethod
    def delete_file(file_path):
        path = Path(file_path)
        if not path.exists():
            raise FileNotFoundError(f"File not found: {file_path}")

        try:
            path.unlink()
            return True
        except Exception as e:
            raise Exception(f"Failed to delete file: {str(e)}")

    # Create a new directory
    @staticmethod
    def create_directory(dir_path):
        path = Path(dir_path)
        if path.exists():
            raise FileExistsError(f"Directory already exists: {dir_path}")

        try:
            path.mkdir(parents=True)
            return True
        except Exception as e:
            raise Exception(f"Failed to create directory: {str(e)}")

    # Delete a directory and its contents
    @staticmethod
    def delete_directory(dir_path):
        path = Path(dir_path)
        if not path.exists():
            raise FileNotFoundError(f"Directory not found: {dir_path}")

        try:
            shutil.rmtree(dir_path)
            return True
        except Exception as e:
            raise Exception(f"Failed to delete directory: {str(e)}")

    # Rename a file or directory
    @staticmethod
    def rename_item(old_path, new_name):
        old = Path(old_path)
        if not old.exists():
            raise FileNotFoundError(f"Item not found: {old_path}")

        new = old.parent / new_name
        if new.exists():
            raise FileExistsError(f"Item already exists: {new_name}")

        try:
            old.rename(new)
            return str(new)
        except Exception as e:
            raise Exception(f"Failed to rename: {str(e)}")

    # List all items in a directory with metadata
    @staticmethod
    def list_directory(dir_path):
        path = Path(dir_path)
        if not path.exists() or not path.is_dir():
            raise NotADirectoryError(f"Invalid directory: {dir_path}")

        try:
            items = []
            for item in path.iterdir():
                item_info = {
                    'path': str(item),
                    'name': item.name,
                    'is_dir': item.is_dir(),
                    'size': item.stat().st_size if item.is_file() else 0,
                    'modified': item.stat().st_mtime,
                    'created': item.stat().st_ctime,
                    'accessed': item.stat().st_atime
                }
                items.append(item_info)

            # Sort: directories first, then files, alphabetically
            items.sort(key=lambda x: (not x['is_dir'], x['name'].lower()))
            return items
        except Exception as e:
            raise Exception(f"Failed to list directory: {str(e)}")

    # Get detailed properties of a file or directory
    @staticmethod
    def get_item_properties(item_path):
        path = Path(item_path)
        if not path.exists():
            raise FileNotFoundError(f"Item not found: {item_path}")

        stats = path.stat()
        return {
            'name': path.name,
            'path': str(path),
            'parent': str(path.parent),
            'is_dir': path.is_dir(),
            'size': stats.st_size,
            'created': stats.st_ctime,
            'modified': stats.st_mtime,
            'accessed': stats.st_atime
        }