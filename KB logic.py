# SynthReason - Synthetic Dawn - intelligent symbolic manipulation system
# BSD 2-Clause License
# 
# Copyright (c) 2022, GeorgeSEC1 - George Wagenknecht
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
import random
def convert(lst):
    return (lst.split())
def process(file):
    with open(file, encoding='ISO-8859-1') as f:
        text = f.read()
    output = ""
    words = convert(text)
    string = "" 
    db = []
    db.append("or")
    dbX = []
    dbX.append("such as")
    dbX.append("because of")
    dbX.append("regarding")
    dbX.append("concerning")
    dbX.append("in reference to")
    dbX.append("as for")
    dbX.append("as to")
    dbY = []
    dbY.append("is")
    dbY.append("will")
    dbY.append("will not")
    dbY.append("has")
    dbY.append("infers against")
    for i in range(len(words)-3):
        if len(words[i-1]) > 5 and len(words[i+1]) > 5 and words[i] == db[random.randint(0,len(db)-1)] and len(words[i-2]) > 5:
            db.append(words[i-1])
            var = words[i-1] + " " + dbX[random.randint(0,len(dbX)-1)] + " " + words[i+1] + " " + dbY[random.randint(0,len(dbY)-1)] + " " + words[i-2] + "\n"
            string += var
            print(var )
    return string 
with open("fileList.conf", encoding='ISO-8859-1') as f:
    files = f.readlines()
print("SynthReason - Synthetic logic")
filename = "logic.conf"
random.shuffle(files)
for file in files:
    selection = []
    sync = process(file.strip())                
    f = open(filename, "a", encoding="utf8")
    f.write("\n")
    f.write(sync)
    f.write("\n")
    f.close()