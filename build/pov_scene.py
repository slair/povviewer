#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys, struct

sys.path.append("/home/slair/lib")
from vec3d import vec3d

ru = lambda x: unicode(x,'utf-8')

CHAR_SIZE =		1
CHAR_FORMAT =	"c"
SHORT_SIZE =	2
SHORT_FORMAT =	"h"
USHORT_SIZE =	2
USHORT_FORMAT =	"H"
INT_SIZE =		4
INT_FORMAT =	"i"
UINT_SIZE =		4
UINT_FORMAT =	"I"
FLOAT_SIZE =	4
FLOAT_FORMAT =	"f"
DBL_SIZE =		8
DBL_FORMAT =	"d"

def DEF__STR__(self):
	res = "%s {"%(self.__class__)
	for attr in dir(self):
		if not attr[:2] == "__":
			value = getattr(self, attr)
			if isinstance(value, list) \
				or isinstance(value, tuple):
				if len(value)>10:
					pvalue = value[:10]
				else:
					pvalue = value
			else:
				pvalue = value
			res += "\n\t%s = %s"%(attr, pvalue)
	res += "\n%s }"%(self.__class__)
	return res

class POVBBox:
	def __init__(self):
		self._bbox = [0.0]*6

	def read(self,df):
		res = 0
		res += 6*FLOAT_SIZE; self._bbox=struct.unpack(FLOAT_FORMAT*6, df.read(6*FLOAT_SIZE))
		return res

	def __str__(self):
		res = "POVBBox: <%s, %s, %s> - <%s, %s, %s>"%tuple(self._bbox)
		return res

class POVColor:
	def __init__(self):
		self._color = [0.0]*5

	def read(self,df):
		res = 0
		res += 5*FLOAT_SIZE; self._color = struct.unpack(FLOAT_FORMAT*5, df.read(5*FLOAT_SIZE))
		return res

	def __str__(self):
		res="POVColor: <%s, %s, %s, %s, %s>"%tuple(self._color)
		return res

class POVTransform:
	def __init__(self):
		self._transform = [0.0] * 16

	def read(self,df):
		res = 0
		res += 16*DBL_SIZE; self._transform=struct.unpack(DBL_FORMAT*16, df.read(16*DBL_SIZE))
		return res

	def __str__(self):
		res="POVTransform:\n[%20s,%20s,%20s,%20s]\n[%20s,%20s,%20s,%20s]\n[%20s,%20s,%20s,%20s]\n[%20s,%20s,%20s,%20s]"%tuple(self._transform)
		return res

class POVBase:
	def __init__(self):
		self.__str__ = DEF__STR__
		self._is_base_object = True
		self._transform = None
		self._bbox = None
		self._color = None

	def read(self,df):
		res=0
		for i in xrange(3):
			res += INT_SIZE; token=struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]

			if token==0x534E5254:	# TRNS
				# load transformation matrix
				self._transform=POVTransform()
				res += self._transform.read(df)

			elif token==0x584F4242:	# BBOX
				# read bounding box
				self._bbox=POVBBox()
				res += self._bbox.read(df)

			elif token==0x524C4F43:	# COLR
				# read color
				self._color=POVColor()
				res += self._color.read(df)

			elif token==0:
				pass

			else:
				print "POVBase Error! At position %04X:%04X, bad token - '%s' (0x%X)"%(int((res-4)/65536),(res-4)-int((res-4)/65536)*65536, repr(token))
		return res

class POVSphereStruct:
	def __init__(self):
		self._center = vec3d(0,0,0)
		self._radius = 0.0
		self.__str__ = DEF__STR__

	def read(self,df):
		res = 0
		res += self._center.read(df)
		res += DBL_SIZE; self._radius = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		return res

class POVSphereSegment:
	def __init__(self):
		self.__str__ = DEF__STR__
		self._closing_sphere = [POVSphereStruct(), POVSphereStruct()]
		self._center_deriv = [vec3d(0,0,0), vec3d(0,0,0)]
		self._radius_deriv = [0.0, 0.0]
		self._num_coefs = 0
		self._center_coefs = []
		self._radius_coefs = []
		self._depth_tolerance = 0.0

	def read(self,df):
		res = 0
		res += self._closing_sphere[0].read(df)
		res += self._closing_sphere[1].read(df)
		res += self._center_deriv[0].read(df)
		res += self._center_deriv[1].read(df)
		res += 2*DBL_SIZE; self._radius_deriv = struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
		res += INT_SIZE; self._num_coefs = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		for i in xrange(self._num_coefs):
			new_vec3d = vec3d(0, 0, 0)
			res += new_vec3d.read(df)
			res += DBL_SIZE; new_dbl = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
			self._center_coefs.append(new_vec3d)
			self._radius_coefs.append(new_dbl)
		res += DBL_SIZE; self._depth_tolerance = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		return res

class POVSphereSweep(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self._is_base_object = False
		self._interpolation = 0
		self._num_modeling_spheres = 0
		self._modeling_spheres = []
		self._num_spheres = 0
		self._spheres = []
		self._num_segments = 0
		self._segments = []
		self.__str__ = DEF__STR__

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += 2*INT_SIZE; self._interpolation, self._num_modeling_spheres=struct.unpack(INT_FORMAT*2, df.read(2*INT_SIZE))
		for i in xrange(self._num_modeling_spheres):
			new_sphere_struct = POVSphereStruct()
			res += new_sphere_struct.read(df)
			self._modeling_spheres.append(new_sphere_struct)
		res+=INT_SIZE; self._num_spheres=struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		for i in xrange(self._num_spheres):
			new_sphere_struct = POVSphereStruct()
			res += new_sphere_struct.read(df)
			self._spheres.append(new_sphere_struct)
		res+=INT_SIZE; self._num_segments=struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		for i in xrange(self._num_segments):
			new_sphere_segment = POVSphereSegment()
			res += new_sphere_segment.read(df)
			self._segments.append(new_sphere_segment)
		return res

class POVCone(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self._is_base_object=False
		self._apex = vec3d(0,0,0)
		self._apex_radius = 0.0
		self._base = vec3d(0,0,0)
		self._base_radius = 0.0
		self._dist = 0.0
		self.__str__ = DEF__STR__

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._apex.read(df)
		res += DBL_SIZE; self._apex_radius=struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += self._base.read(df)
		res += 2*DBL_SIZE; self._base_radius, self._dist = struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
		return res

class POVCamera(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self._is_base_object=False
		self._type = 0
		self._h_angle = 0.0
		self._v_angle = 0.0
		self._location = vec3d(0,0,0)
		self._direction = vec3d(0,0,0)
		self._up = vec3d(0,0,0)
		self._right = vec3d(0,0,0)
		self._sky = vec3d(0,0,0)
		self._look_at = vec3d(0,0,0)
		self._angle = 0.0
		self.__str__ = DEF__STR__

	def read(self,df):
		res = 0
		res += INT_SIZE; self._type=struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		res += 2*DBL_SIZE; self._h_angle,self._v_angle=struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
		res += self._location.read(df)
		res += self._direction.read(df)
		res += self._up.read(df)
		res += self._right.read(df)
		res += self._sky.read(df)
		res += self._look_at.read(df)
		res += DBL_SIZE; self._angle=struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		return res

class POVLightSource(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._colour = [0.0]*3
		self._position = vec3d(0,0,0)
		self._direction = vec3d(0,0,0)
		self._points_at = vec3d(0,0,0)
		self._axis1 = vec3d(0,0,0)
		self._axis2 = vec3d(0,0,0)
		self._type = 0
		self._coeff = 0.0
		self._radius = 0.0
		self._falloff = 0.0
		self._fade_distance = 0.0
		self._fade_power = 0.0
		self._area_size1 = 0
		self._area_size2 = 0
		self._area_light = 0
		self._parallel = 0

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += FLOAT_SIZE*3; self._colour=struct.unpack(FLOAT_FORMAT*3, df.read(FLOAT_SIZE*3))
		res += self._position.read(df)
		res += self._direction.read(df)
		res += self._points_at.read(df)
		res += self._axis1.read(df)
		res += self._axis2.read(df)
		res += CHAR_SIZE; self._type = ord(struct.unpack(CHAR_FORMAT, df.read(CHAR_SIZE))[0])
		res += DBL_SIZE; self._coeff = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += DBL_SIZE; self._radius = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += DBL_SIZE; self._falloff = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += DBL_SIZE; self._fade_distance = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += DBL_SIZE; self._fade_power = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += INT_SIZE; self._area_size1 = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		res += INT_SIZE; self._area_size2 = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		res += CHAR_SIZE; self._area_light = ord(struct.unpack(CHAR_FORMAT, df.read(CHAR_SIZE))[0])
		res += CHAR_SIZE; self._parallel = ord(struct.unpack(CHAR_FORMAT, df.read(CHAR_SIZE))[0])
		return res

class POVBox(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self._is_base_object=False
		self._corner1=vec3d(0,0,0)
		self._corner2=vec3d(0,0,0)
		self.__str__ = DEF__STR__

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._corner1.read(df)
		res += self._corner2.read(df)
		return res

class POVTTFContour:
	def __init__(self):
		self.__str__ = DEF__STR__
		self._count = 0		#	ushort
		self._coords = []	# x,y doubles

	def read(self,df):
		res = 0
		res += USHORT_SIZE; self._count = struct.unpack(USHORT_FORMAT, df.read(USHORT_SIZE))[0]
		for i in xrange(self._count):
			res += 2*DBL_SIZE; x,y = struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
			self._coords.append([x,y])
		return res

class POVIsosurface(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._tri_count = 0
		self._triangles = []

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += INT_SIZE; self._tri_count = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		p1 = vec3d(0,0,0)
		p2 = vec3d(0,0,0)
		p3 = vec3d(0,0,0)
		for i in xrange(self._tri_count):
			res += p1.read(df)
			res += p2.read(df)
			res += p3.read(df)
			self._triangles.append([p1, p2, p3])
		return res

class POVPolygon(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._s_normal = vec3d(0,0,0)
		self._number = 0	# int
		self._points = []	# UV_VECT DBL,DBL

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._s_normal.read(df)
		res += INT_SIZE; self._number = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]

		res += DBL_SIZE*self._number*2
		self._points = struct.unpack(DBL_FORMAT*self._number*2, df.read(DBL_SIZE*self._number*2))
		return res

class POVDisc(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._center = vec3d(0,0,0)
		self._normal = vec3d(0,0,0)
		self._radiuses = [0.0, 0.0]

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._center.read(df)
		res += self._normal.read(df)
		res += DBL_SIZE*2; self._radiuses = struct.unpack(DBL_FORMAT*2, df.read(DBL_SIZE*2))
		return res

class POVHeightfield(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._bounding_corner1 = vec3d(0, 0, 0)
		self._bounding_corner2 = vec3d(0, 0, 0)
		self._max_x = 0			# int
		self._max_z = 0			# int
		self._min_y = 0			# short
		self._max_y = 0			# short
		self._block_max_x = 0	# int
		self._block_max_z = 0	# int
		self._block_width_x = 0	# int
		self._block_width_z = 0	# int
		self._map = []			# DBL[max_z * max_x]
		self._normals = []		# vec3s[max_z * max_x]

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._bounding_corner1.read(df)
		res += self._bounding_corner2.read(df)
		res += 2*INT_SIZE
		self._max_x, self._max_z = struct.unpack(INT_FORMAT*2, df.read(2*INT_SIZE))
		self._max_x += 2
		self._max_z += 2
		res += 2*SHORT_SIZE
		self._min_y, self._max_y = struct.unpack(SHORT_FORMAT*2, df.read(SHORT_SIZE*2))
		res += 4*INT_SIZE
		self._block_max_x, self._block_max_z, self._block_width_x, self._block_width_z = struct.unpack(INT_FORMAT*4, df.read(4*INT_SIZE))

		data = df.read(UINT_SIZE)
		res += UINT_SIZE; token = struct.unpack(UINT_FORMAT, data)[0]
		if token == 0x50414D48:		# 'HMAP'
			res += DBL_SIZE*self._max_z*self._max_x
			self._map = struct.unpack(DBL_FORMAT*self._max_z*self._max_x, df.read(DBL_SIZE*self._max_z*self._max_x))
		elif token == 0:
			pass
		else:
			print "Error! Bad heightfield token - %s (0x%X)"%(repr(data),token)

		data = df.read(UINT_SIZE)
		res += UINT_SIZE; token = struct.unpack(UINT_FORMAT, data)[0]
		if token == 0x524F4E48:		# 'HNOR'
			res += SHORT_SIZE*self._max_z*self._max_x*3
			self._normals = struct.unpack(SHORT_FORMAT*self._max_z*self._max_x*3, df.read(SHORT_SIZE*self._max_z*self._max_x*3))
		elif token == 0:
			pass
		else:
			print "Error! Bad heightfield token - %s (0x%X)"%(repr(data),token)

		#~ p1 = vec3d(0,0,0)
		#~ p2 = vec3d(0,0,0)
		#~ p3 = vec3d(0,0,0)
		#~ for i in xrange(self._tri_count):
			#~ res += p1.read(df)
			#~ res += p2.read(df)
			#~ res += p3.read(df)
			#~ self._triangles.append([p1, p2, p3])
		return res

class POVMesh(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._tri_count = 0
		self._triangles = []

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += INT_SIZE; self._tri_count = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		p1 = vec3d(0,0,0)
		p2 = vec3d(0,0,0)
		p3 = vec3d(0,0,0)
		for i in xrange(self._tri_count):
			res += p1.read(df)
			res += p2.read(df)
			res += p3.read(df)
			self._triangles.append([p1, p2, p3])
		return res

class POVBlob(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._tri_count = 0
		self._triangles = []

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += INT_SIZE; self._tri_count = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		p1 = vec3d(0,0,0)
		p2 = vec3d(0,0,0)
		p3 = vec3d(0,0,0)
		for i in xrange(self._tri_count):
			res += p1.read(df)
			res += p2.read(df)
			res += p3.read(df)
			self._triangles.append([p1, p2, p3])
		return res

class POVPlane(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._normal = vec3d(0,0,0)
		self._distance = 0.0

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._normal.read(df)
		res += DBL_SIZE
		self._distance = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))
		return res

class POVPrism(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._number = 0		# int
		self._spline_type = 0	# int
		self._sweep_type = 0	# int
		self._height1 = 0.0		# DBL
		self._height2 = 0.0		# DBL
		self._spline_entry = []	# DBL x1, y1, x2, y2 DBL,DBL A, B, C, D

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += 3*INT_SIZE
		self._number,self._spline_type,self._sweep_type = struct.unpack(INT_FORMAT*3, df.read(3*INT_SIZE))
		res += 2*DBL_SIZE
		self._height1, self._height2 = struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
		for i in xrange(self._number):
			res += 12*DBL_SIZE
			x1, y1, x2, y2, au, av, bu, bv, cu, cv, du, dv = struct.unpack(DBL_FORMAT*12, df.read(12*DBL_SIZE))
			self._spline_entry.append([x1, y1, x2, y2, au, av, bu, bv, cu, cv, du, dv])
		return res

class POVLathe(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._spline_type = 0	# int
		self._number = 0		# int
		self._radius1 = 0.0		# DBL
		self._radius2 = 0.0		# DBL
		self._height1 = 0.0		# DBL
		self._height2 = 0.0		# DBL
		self._spline_entry = []	# DBL,DBL A, B, C, D

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += INT_SIZE; self._spline_type = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		res += INT_SIZE; self._number = struct.unpack(INT_FORMAT, df.read(INT_SIZE))[0]
		res += 4*DBL_SIZE
		self._radius1, self._radius2, self._height1, self._height2 = struct.unpack(DBL_FORMAT*4, df.read(4*DBL_SIZE))
		for i in xrange(self._number):
			res += 8*DBL_SIZE
			au, av, bu, bv, cu, cv, du, dv = struct.unpack(DBL_FORMAT*8, df.read(8*DBL_SIZE))
			self._spline_entry.append([au, av, bu, bv, cu, cv, du, dv])
		return res

class POVTTF(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._depth = 0.0
		self._num_contours = 0	# short
		self._contours = []

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += DBL_SIZE; self._depth = struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		res += SHORT_SIZE; self._num_contours = struct.unpack(SHORT_FORMAT, df.read(SHORT_SIZE))[0]
		for i in xrange(self._num_contours):
			new_contour = POVTTFContour()
			res += new_contour.read(df)
			self._contours.append(new_contour)
		return res

class POVTorus(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._major_radius = 0.0
		self._minor_radius = 0.0

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += 2*DBL_SIZE
		self._major_radius, self._minor_radius=struct.unpack(DBL_FORMAT*2, df.read(2*DBL_SIZE))
		return res

class POVBicubicpatch(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._control_points = []

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		for i in xrange(16):
			new_vec3d = vec3d(0, 0, 0)
			res += new_vec3d.read(df)
			self._control_points.append(new_vec3d)
		return res

class POVSphere(POVBase):
	def __init__(self):
		POVBase.__init__(self)
		self.__str__ = DEF__STR__
		self._is_base_object = False
		self._center = vec3d(0, 0, 0)
		self._radius = 0.0

	def read(self,df):
		res = 0
		res += POVBase.read(self,df)
		res += self._center.read(df)
		res += DBL_SIZE; self._radius=struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		return res

class POVFrame:
	def __init__(self):
		self._clock = 0
		self._objects = []

	def read(self,df):
		res=0
		res+=DBL_SIZE; self._clock=struct.unpack(DBL_FORMAT, df.read(DBL_SIZE))[0]
		return res

	def add_object(self, obj):
		self._objects.append(obj)

	def __str__(self):
		res="\tClock: %s\n"%(self._clock)
		res+="\tObjects count: %d\n"%(len(self._objects))
		for obj in self._objects:
			res+="-------\n%s\n"%obj.__str__(obj)
		return res

class POVScene:
	def __init__(self):
		self._size_shortint = 0
		self._size_int = 0
		self._size_float = 0
		self._size_double = 0
		self._frames=[]

	def LoadDump(self,dumpfilename):
		res=True
		df=open(dumpfilename,"rb")
		readden=0
		dfsize=os.path.getsize(dumpfilename)
		readden+=1; self._size_shortint=struct.unpack("b", df.read(1))[0]
		readden+=1; self._size_int=struct.unpack("b", df.read(1))[0]
		readden+=1; self._size_float=struct.unpack("b", df.read(1))[0]
		readden+=1; self._size_double=struct.unpack("b", df.read(1))[0]
		readden+=4; self._scene_id=struct.unpack(UINT_FORMAT, df.read(UINT_SIZE))[0]

		#~ print "readden=%s	dfsize=%s"%(readden, dfsize)
		newframe = None
		while (readden<dfsize)and(res):

			data = df.read(UINT_SIZE)
			#~ print "readden=%s	dfsize=%s	len(data)=%s	data=%s"%(readden, dfsize, len(data), repr(data))
			readden += UINT_SIZE; token = struct.unpack(UINT_FORMAT, data)[0]
			if token==0x454D5246:	# 'FRME'
				if newframe:
					self._frames.append(newframe)
					newframe = None
				newframe = POVFrame()
				readden += newframe.read(df)

			elif token==0x524D4143:	# 'CAMR'
				if newframe:
					newobj=POVCamera()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x43534944:	# 'DISC'
				if newframe:
					newobj=POVDisc()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x594C4F50:	# 'POLY'
				if newframe:
					newobj=POVPolygon()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x444C4648:	# 'HFLD'
				if newframe:
					newobj=POVHeightfield()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x4853454D:	# 'MESH'
				if newframe:
					newobj=POVMesh()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x424F4C42:	# 'BLOB'
				if newframe:
					newobj=POVBlob()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x41504942:	# 'BIPA'
				if newframe:
					newobj=POVBicubicpatch()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x53524F54:	# 'TORS'
				if newframe:
					newobj=POVTorus()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x4E414C50:	# 'PLAN'
				if newframe:
					newobj=POVPlane()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x4D535250:	# 'PRSM'
				if newframe:
					newobj=POVPrism()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x4854414C:	# 'LATH'
				if newframe:
					newobj=POVLathe()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x534F5349:	# 'ISOS'
				if newframe:
					newobj=POVIsosurface()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x5F465454:	# 'TTF_'
				if newframe:
					newobj=POVTTF()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x5448474C:	# 'LGHT'
				if newframe:
					newobj=POVLightSource()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x45534142:	# 'BASE'
				if newframe:
					newobj=POVBase()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x57535053:	# 'SPSW'
				if newframe:
					newobj=POVSphereSweep()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x454E4F43:	# 'CONE'
				if newframe:
					newobj=POVCone()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x20584F42:	# 'BOX '
				if newframe:
					newobj=POVBox()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			elif token==0x52485053:	# 'SPHR'
				if newframe:
					newobj=POVSphere()
					readden+=newobj.read(df)
					newframe.add_object(newobj)
				else:
					print "No frame!!!"
					res = False

			else:
				print "Error! At position %04X:%04X, bad token - %s (0x%X)"%(
				int((readden-4)/65536),
				(readden-4)-int((readden-4)/65536)*65536,
				repr(data),
				token)
				res = False

		if (newframe):
			self._frames.append(newframe)

		df.close()
		return res

	def __str__(self):
		res=""
		res+="Size of short int:%4d\n"%self._size_shortint
		res+="Size of int:      %4d\n"%self._size_int
		res+="Size of float:    %4d\n"%self._size_float
		res+="Size of double:   %4d\n"%self._size_double
		res+="Scene ID:   0x%X\n"%self._scene_id
		res+="Frames:        %7d\n----------------------\n"%len(self._frames)
		count=0
		for frame in self._frames:
			res+="Frame %d\n"%count+str(frame)
			count+=1
		return res

def main():
	#~ f=open("scene.dump","rb")
	S=POVScene()
	print "\n\nParsing dump file '%s':\n"%"scene.dump"
	if not S.LoadDump("scene.dump"):
		print "LoadDump failed!\n\n"
	#~ else:
	print S
if __name__=='__main__':
	main()
