from fabric.api import *
env.user = 'hcyang'
env.hosts = ['hcyang@141.223.121.77:2200']
def make():
    with cd('/home/hcyang/bitvisor/bitvisor-1.3/'):
    		run('make')