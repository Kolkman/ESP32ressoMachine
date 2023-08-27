#!/usr/bin/env python

import sys
import re
import os.path
import gzip


def bytes_to_c_arr(data):
    return [format(b, '#04x') for b in data]


path = sys.argv[1]

print("//Generated from "+path)

_inputname = os.path.basename(path)

p1 = re.compile('-min\.')
filename = p1.sub(".",_inputname)

p2 = re.compile('\.')
_name = p2.sub("_", filename)



try:
    scope=sys.argv[3]
except IndexError:
    scope="class"



try:
    _file = open(path, "r")
    _data = _file.read()

except:
    print("Could not open "+path)
finally:
    _file.close()


print("#ifndef WEB"+_name+"_H")
print("#define WEB"+_name+"_H")
print()
# server->on("/EspressoMachine.svg", HTTP_GET, std::bind(&WebInterface::handleEspressoSVG, this, std::placeholders::_1));
#
_compr = gzip.compress(bytes(_data, 'utf-8'))

print("unsigned char "+_name +
      "[] = {{{}}};".format(",".join(bytes_to_c_arr(_compr))))
print("unsigned int "+_name+"_len = ", len(_compr), ";")

if scope=="class":
#  print("\n\n")
#  print("#define DEF_HANDLE_"+_name + "  " + "server->on(\"/"+filename, end='')
#  print("\", HTTP_GET, EspressoWebServer::handleFile((AsyncWebServerRequest *) request",  end='')
#  print(",\"" + sys.argv[2]+"\","+_name+","+_name+"_len));")


  print("\n\n")
  print("#define DEF_HANDLE_"+_name + "  " + "server->on(\"/"+filename, end='')
  print("\", HTTP_GET, std::bind (&EspressoWebServer::handleFile, server, std::placeholders::_1", end='')
  print(",\"" + sys.argv[2]+"\","+_name+","+_name+"_len));")

else:  ### This is serverscope


  print("\n\n")
  print("#define DEF_HANDLE_"+_name + "  " + "on(\"/"+filename, end='')
  print("\", HTTP_GET, std::bind (&EspressoWebServer::handleFile, this, std::placeholders::_1", end='')
  print(",\"" + sys.argv[2]+"\","+_name+","+_name+"_len));")
print("#endif")
