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

bcv = Bsdconv("UTF-8:INSERT#AFTER=002C:BSDCONV-KEYWORD,BSDCONV")
bcv1252 = Bsdconv("CP1252:INSERT#AFTER=002C:BSDCONV-KEYWORD,BSDCONV")

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
			fontdata[ftype] = {}
		fontdata[ftype][m.attr("legacy")]=m.attr("unicode")

def fwalk(ftype):
	ret = dict(fontdata[ftype])
	if fonttype[ftype]["inherit"]:
		inh = fwalk(fonttype[ftype]["inherit"])
		for k in inh:
			ret[k] = inh[k]
	return ret



for ftype in fonttype:
	if fonttype[ftype]["hidden"]:
		continue
	fout = open("modules/inter/KHMER-"+ftype.upper()+".txt", "w")
	data = []
	mapping = fwalk(ftype)
	for f in  mapping:
		t = mapping[f]
		f = bcv1252.conv(b"".join([chr(eval(x)) for x in f.split(";")])).rstrip(",")
		t = bcv.conv(t.encode("utf-8")).rstrip(",")
		data.append((f, t))
	data = sorted(data)
	for f, t in data:
		fout.write("%s\t%s\n" % (f, t))
	fout.close()
