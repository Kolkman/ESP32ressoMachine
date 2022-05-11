#!/usr/bin/env python

import sys
import re
import os.path
import gzip

def bytes_to_c_arr(data):
    return [format(b, '#04x') for b in data]

path=(sys.argv[1])
filename= os.path.basename(path)
p = re.compile('\.')
_name=p.sub("_", filename)

_file = open(path, "r")
_data = _file.read()
_file.close()


print ("#ifndef WEB"+_name+"_H")
print ("#define WEB"+_name+"_H")
print()
# server->on("/EspressoMachine.svg", HTTP_GET, std::bind(&WebInterface::handleEspressoSVG, this, std::placeholders::_1));

#
_compr = gzip.compress(bytes(_data, 'utf-8'))

print("unsigned char "+_name+"[] = {{{}}};".format(",".join(bytes_to_c_arr(_compr))))
print("unsigned int "+_name+"_len = ", len(_compr),";")


print("\n\n")
print("#define DEF_HANDLE_"+_name + "  server->on(\"/"+filename, end='')
print("\", HTTP_GET, std::bind (&WebInterface::handleFile, this, std::placeholders::_1", end='')
print(",\""+ sys.argv[2]+"\","+_name+","+_name+"_len));")


print ("#endif")