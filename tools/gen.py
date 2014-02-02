import sys
import re
import itertools
from bsdconv import Bsdconv
from pyquery import PyQuery as pq

def bsdconv01(dt):
	dt=dt.strip().lstrip("0").upper()
	if len(dt) & 1:
		return "010"+dt
	else:
		return "01"+dt

stp = re.compile(r"^(U\+|0[Xx])")
bcv = Bsdconv("UTF-8:INSERT#AFTER=002C:BSDCONV-KEYWORD,BSDCONV")

d = pq(open(sys.argv[1]).read())

fonttype = {}
fontdata = {}

for font in d.find("font"):
	font = pq(font)
	ftype = font.attr("type")
	fonttype[ftype] = {"hidden": font.attr("hidden"), "inherit": font.attr("inherit")}
	for m in itertools.chain(font.find("global").find("map"), font.find("tounicode").find("map")):
		m = pq(m)
		if ftype not in fontdata:
			fontdata[ftype] = []
		fontdata[ftype].append((m.attr("legacy"), m.attr("unicode")))

def fwalk(ftype):
	ret = []
	if fonttype[ftype]["inherit"]:
		ret = itertools.chain(ret, fwalk(fonttype[ftype]["inherit"]))
	ret = itertools.chain(ret, fontdata[ftype])
	return ret



for ftype in fonttype:
	if fonttype[ftype]["hidden"]:
		continue
	fout = open("modules/inter/KHMER-"+ftype.upper()+".txt", "w")
	data=[]
	for f, t in  fwalk(ftype):
		f = ",".join([bsdconv01(stp.sub("", x)) for x in f.split(";")])
		t = bcv.conv(t.encode("utf-8")).rstrip(",")
		data.append((f, t))
	data = sorted(data)
	for f, t in data:
		fout.write("%s\t%s\n" % (f, t))
	fout.close()
