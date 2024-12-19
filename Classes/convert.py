# -*- coding: utf-8 -*-
import os
import codecs

def convert_to_utf8_with_bom(file_path):
    """将文件转换为 UTF-8 with BOM 格式"""
    try:
        # 读取文件内容
        with open(file_path, 'rb') as f:
            content = f.read()
        
        # 如果已经是 UTF-8 with BOM，跳过
        if content.startswith(codecs.BOM_UTF8):
            print("Already UTF-8 with BOM: %s" % file_path)
            return True
            
        # 尝试检测和解码内容
        try:
            # 如果是 UTF-8 无 BOM
            text = content.decode('utf-8')
        except UnicodeDecodeError:
            try:
                # 如果是 GBK
                text = content.decode('gbk')
            except:
                print("Warning: Could not decode %s" % file_path)
                return False
        
        # 写入 UTF-8 with BOM
        with open(file_path, 'wb') as f:
            f.write(codecs.BOM_UTF8)
            f.write(text.encode('utf-8'))
            
        print("Converted to UTF-8 with BOM: %s" % file_path)
        return True
        
    except Exception as e:
        print("Error processing %s: %s" % (file_path, str(e)))
        return False

def main():
    current_dir = os.getcwd()
    print("Converting files in: %s" % current_dir)
    print("-" * 50)
    
    success_count = 0
    fail_count = 0
    
    for root, dirs, files in os.walk(current_dir):
        for file in files:
            if file.endswith(('.cpp', '.h')):
                file_path = os.path.join(root, file)
                if convert_to_utf8_with_bom(file_path):
                    success_count += 1
                else:
                    fail_count += 1
    
    print("-" * 50)
    print("Conversion completed!")
    print("Successfully converted: %d files" % success_count)
    print("Failed to process: %d files" % fail_count)

if __name__ == '__main__':
    main()