import os
import sys
import string
import shutil

def isBuild() -> bool:
  file = os.access("scb", os.F_OK)
  return file

def isC(file: str) -> bool:
  l = len(file) - 1
  while l > 0 and file[l] != '.':
    l -= 1
  if file[l + 1] == 'c' and file[l] == '.':
    return True
  else:
    return False

def addFile(pwd: str) -> str:
  files = ""
  for f in os.scandir(pwd):
    if f.is_file() and isC(f.name):
      files += os.path.join(pwd, f.name) + " "
    if f.is_dir():
      files += addFile(os.path.join(pwd, f.name))
  return files

def askCC() -> str:
  s = input("witch compile you want to use? ")
  return  shutil.which(s)


def main() -> int:
  path = os.path.join(os.getcwd(), "src")
  cc = askCC()
  if cc == None:
    print("invalid compiler\n")
    os.sys.exit(1)
  filesList = addFile(path)
  spl = filesList.split()
  args = [cc, "-o", "scb"]
  for f in spl:
    args.append(f)
    print("->", f)
  print("file found!\n")
  if isBuild() == True:
    print("rebuilding\n")
  pid = os.fork()
  if pid == 0:
    print(cc)
    os.execvp(cc, args)
    #fail safe
    sys.exit(0)
  os.waitpid(pid, 0)
  print("finish")





if __name__ == "__main__":
  main()