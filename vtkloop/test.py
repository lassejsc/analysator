import cpphelpers as chp
import analysator as pt
import numpy as np


file="/home/siclasse/bulk.0000110.vlsv"
f=pt.vlsvfile.VlsvReader(file)

f._VlsvReader__read_fileindex_for_cellid()
fileindex_for_cellid = f._VlsvReader__fileindex_for_cellid
xc= f._VlsvReader__xcells
yc= f._VlsvReader__ycells
zc= f._VlsvReader__zcells
max_ref_level = f.get_max_refinement_level()
cid_offset= np.zeros(max_ref_level+1,dtype=np.int64)
isum=0
for i in range(0,max_ref_level):
    isum+=2**(3*i)*xc*yc*zc
    cid_offset[i+1]=isum

xcells = np.zeros((max_ref_level+1),dtype=np.int64)
ycells = np.zeros((max_ref_level+1),dtype=np.int64)
zcells = np.zeros((max_ref_level+1),dtype=np.int64)

for r in range(max_ref_level+1):
    xcells[r] = xc*2**(r)
    ycells[r] = yc*2**(r)
    zcells[r] = zc*2**(r)

delta = np.array([[0,0,0],[1,0,0],[0,1,0],[1,1,0]
                  ,[0,0,1],[1,0,1],[0,1,1],[1,1,1]],dtype=np.int32
                  )
    
a=np.array([1,2,3,4,5,6,7],dtype=np.float32)

po=chp.test(a,np.array([-1,-2,-3,-4,-5,-6,-7]))
print(po)
