import glob
import os

os.chdir('/home/luge/wahoo/twrp/src')

files = glob.glob('build/soong/**/legacy_proto_support.go', recursive=True)
print('found', len(files))
for p in files:
    s = open(p).read()
    o = s
    s = s.replace('"google.golang.org/protobuf/protoadapt"', '"github.com/golang/protobuf/protoadapt"')
    s = s.replace('"google.golang.org/protobuf/encoding/prototext"', '"github.com/golang/protobuf/encoding/prototext"')
    if s != o:
        open(p, 'w').write(s)
        print('fixed', p)

for p in ('build/soong/cmd/sbox/sbox.go',):
    s = open(p).read()
    o = s
    s = s.replace('"google.golang.org/protobuf/encoding/prototext"', '"github.com/golang/protobuf/encoding/prototext"')
    s = s.replace('"google.golang.org/protobuf/protoadapt"', '"github.com/golang/protobuf/protoadapt"')
    if s != o:
        open(p, 'w').write(s)
        print('fixed', p)

p = 'external/golang-protobuf/proto/compat_legacy.go'
s = open(p).read()
s = s.replace('"google.golang.org/protobuf/protoiface"', '"github.com/golang/protobuf/runtime/protoiface"')
open(p, 'w').write(s)
print('compat ok')
