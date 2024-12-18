import os
import sys
import codecs

def detect_encoding(file_path):
    """
    检测文件编码
    """
    encodings = ['utf-8', 'utf-8-sig', 'gbk', 'gb2312', 'ascii']
    
    for enc in encodings:
        try:
            with open(file_path, 'r', encoding=enc) as f:
                content = f.read()
                return content, enc
        except UnicodeDecodeError:
            continue
        except Exception as e:
            print(f"Error reading {file_path} with {enc}: {e}")
            continue
    
    return None, None

def convert_file(filepath):
    """
    转换文件为VS最佳格式：UTF-8 with BOM + CRLF
    """
    try:
        content, src_encoding = detect_encoding(filepath)
        if content is None:
            print(f"Failed to detect encoding for: {filepath}")
            return False
        
        # 统一换行符为CRLF
        content = content.replace('\r\n', '\n').replace('\r', '\n').replace('\n', '\r\n')
            
        # 写入文件，使用UTF-8（带BOM）和CRLF行尾
        with open(filepath, 'w', encoding='utf-8-sig', newline='\r\n') as f:
            f.write(content)
        
        print(f"Converted {filepath}: {src_encoding} -> utf-8-sig(BOM), CRLF")
        return True
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return False

def convert_directory(directory):
    """
    转换Classes目录下的.cpp和.h文件
    """
    cpp_extensions = {'.cpp', '.h'}
    converted_count = 0
    total_count = 0
    
    classes_dir = os.path.join(directory, 'Classes')
    if not os.path.exists(classes_dir):
        print(f"Error: Classes directory not found in {directory}")
        return
    
    for root, dirs, files in os.walk(classes_dir):
        for file in files:
            if os.path.splitext(file)[1].lower() in cpp_extensions:
                total_count += 1
                filepath = os.path.join(root, file)
                if convert_file(filepath):
                    converted_count += 1
    
    print(f"\nConversion complete!")
    print(f"Total files checked: {total_count}")
    print(f"Files converted: {converted_count}")
    print("\nNow configuring git to prevent automatic line ending conversion...")
    
    # 设置git配置
    try:
        os.system('git config core.autocrlf false')
        print("Git configuration updated: core.autocrlf = false")
    except Exception as e:
        print(f"Error updating git config: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python convert.py <project_directory>")
        sys.exit(1)
    
    directory = sys.argv[1]
    if not os.path.isdir(directory):
        print(f"Error: {directory} is not a directory")
        sys.exit(1)
    
    convert_directory(directory)