setup(){ R=$1; rm -rf $R; mkdir -p $R/ur0/shell/db $R/ur0/tai $R/ux0 $R/app0/payload; python3 -I make_db.py $R/ur0/shell/db/app.db $2; cp ${3:-c1.txt} $R/ur0/tai/config.txt; cp /home/claude/proj/build/nestfolder.suprx $R/app0/payload/; }
png(){ python3 -I -c "
from PIL import Image;import sys,os
for f in sys.argv[2:]:
  p=f'{sys.argv[1]}/{f}.ppm'
  if os.path.exists(p): Image.open(p).save(f'{sys.argv[1]}_{f}.png')" "$@"; }
