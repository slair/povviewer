class base(object):
	__slots__=("_")
	def __init__(self, *args, **keywords):
		super(base, self).__init__(*args, **keywords)
		self._ = None
		for key,value in keywords.iteritems():
			setattr(self, key, value)

	def __str__(self):
		res="%s {"%(self.__class__)
		for attr in dir(self):
			if not attr[0]=="_":
				res += "\n\t%s = %s"%(attr, getattr(self, attr))
		res += "\n%s }"%(self.__class__)
		return res

