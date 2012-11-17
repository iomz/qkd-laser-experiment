#:coding=utf-8:

import subprocess
from shlex import split
import sys


cmd = "wget -q -O - --no-check-certificate --post-data 'result=" + sys.argv[1] + "' http://malt.dip.jp/AQUA/ORF/result.cgi"
subprocess.call( split(cmd) )
