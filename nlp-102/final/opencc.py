"""
opencc.py

Interface to the opencc command
"""
import subprocess

def t2s(text):
    return subprocess.check_output(['opencc', '-c', 'tw2s.json'],
            input=bytes(text, 'utf-8')).decode('utf-8').strip()

def s2t(text):
    return subprocess.check_output('opencc',
            input=bytes(text, 'utf-8')).decode('utf-8').strip()
