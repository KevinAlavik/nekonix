import os
import subprocess
import shutil
import sys

CC = 'x86_64-elf-gcc'
LD = 'x86_64-elf-ld'
AR = 'x86_64-elf-ar'
NASM = 'nasm'
CFLAGS = '-ffreestanding -nostdlib -fno-builtin -I../libc/include'
LDFLAGS = '-m elf_x86_64 -nostdlib -z text -z max-page-size=0x1000'
ASMFLAGS = '-f elf64'

APPS = ['init']
BUILD_DIR = 'build'
BIN_DIR = 'bin'
LIBC_DIR = '../libc'

def run_command(command, cwd=None):
    subprocess.run(command, shell=True, cwd=cwd, check=True)

def ensure_dir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def build_libc():
    run_command(f'CC={CC} AR={AR} make', cwd=LIBC_DIR)

def build_crt0():
    ensure_dir(BUILD_DIR)
    run_command(f'{NASM} {ASMFLAGS} crt0.asm -o {BUILD_DIR}/crt0.o')
	
def build_app(app):
    app_build_dir = os.path.join(BUILD_DIR, app)
    ensure_dir(app_build_dir)
    
    c_sources = [os.path.join(app, f) for f in os.listdir(app) if f.endswith('.c')]
    for src in c_sources:
        obj_file = os.path.join(app_build_dir, os.path.basename(src).replace('.c', '.o'))
        run_command(f'{CC} {CFLAGS} -c {src} -o {obj_file}')
    
    asm_sources = [os.path.join(app, f) for f in os.listdir(app) if f.endswith('.asm')]
    for src in asm_sources:
        obj_file = os.path.join(app_build_dir, os.path.basename(src).replace('.asm', '.o'))
        run_command(f'{NASM} {ASMFLAGS} {src} -o {obj_file}')
    
    obj_files = [os.path.join(app_build_dir, f) for f in os.listdir(app_build_dir) if f.endswith('.o')]
    ensure_dir(BIN_DIR)
    binary_file = os.path.join(BIN_DIR, app)
    
    linker_script = os.path.join(app, 'linker.ld')
    run_command(f'{LD} {LDFLAGS} -T {linker_script} -o {binary_file} {" ".join(obj_files)} {BUILD_DIR}/crt0.o {LIBC_DIR}/lib/libc.a')

def build_all_apps():
    for app in APPS:
        build_app(app)

def clean():
    shutil.rmtree(BUILD_DIR, ignore_errors=True)
    shutil.rmtree(BIN_DIR, ignore_errors=True)
    run_command('make clean', cwd=LIBC_DIR)

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)

    if len(sys.argv) < 2:
        print("Usage: python build.py [all|clean]")
        sys.exit(1)
    
    command = sys.argv[1]
    if command == 'all':
        build_libc()
        build_crt0()
        build_all_apps()
    elif command == 'clean':
        clean()
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()