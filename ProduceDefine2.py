#!/usr/bin/env python
#

import sys
import re
import os.path
import gzip




path = sys.argv[1]

print("//Generated from "+path)

_inputname = os.path.basename(path)

p1 = re.compile('-min\.')
filename = p1.sub(".",_inputname)

p2 = re.compile('\.')
_name = p2.sub("_", filename)




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

print("const char "+_name + "[]=\"<style>"+_data+"</style>\";")


print("#endif")
