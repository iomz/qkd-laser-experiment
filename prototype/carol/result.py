#:coding=utf-8:

import subprocess
from shlex import split
import sys

CGI = 'http://aqua-cat.sfc.wide.ad.jp/ORF2012/result.cgi'

cmd = "wget -q -O - --no-check-certificate --post-data 'result=" + sys.argv[1] + "' " + CGI
subprocess.call( split(cmd) )
