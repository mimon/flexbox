import os
from waflib import TaskGen, Task

out = 'build'
APPNAME = 'flexbox'

@TaskGen.extension('.mm')
def mm_hook(self, node):
		"Bind the c++ file extensions to the creation of a :py:class:`waflib.Tools.cxx.cxx` instance"
		return self.create_compiled_task('mm', node)

class mm(Task.Task):
		"Compile MM files into object files"
		run_str = '${CXX} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CXX_SRC_F}${SRC} ${CXX_TGT_F}${TGT}'
		vars    = ['CXXDEPS'] # unused variable to depend on, just in case
		ext_in  = ['.h'] # set the build order easily by using ext_out=['.h']

def options(opt):
	if opt.path.make_node('ogre').exists():
		opt.recurse('ogre')
	else:
		opt.add_option('--ogre-sdk-path', action='store', default='./ogre', help='Path to sdk directory containing "include/OGRE/Ogre.h" and "lib/"')
	opt.add_option('--yoga-path', action='store', default='./yoga', help='Path to yoga source')

	opt.load('compiler_cxx compiler_c')

def configure(cnf):
	if cnf.path.make_node('ogre').exists():
		cnf.recurse('ogre')
	elif cnf.options.ogre_sdk_path:
		ogre_path = cnf.options.ogre_sdk_path
		cnf.find_file('include/OGRE/Ogre.h', [ogre_path])
		cnf.find_file('lib/libOgreHlmsUnlitStatic.a', [ogre_path])
		cnf.env.append_value('INCLUDES', '%s/include/OGRE' % ogre_path)
		cnf.env.append_value('INCLUDES', '%s/include/OGRE/RenderSystems/NULL' % ogre_path)
		cnf.env.append_value('INCLUDES', '%s/include/OGRE/Hlms/Unlit' % ogre_path)
		cnf.env.append_value('INCLUDES', '%s/include/OGRE/Hlms/Common' % ogre_path)
		cnf.env.append_value('STLIBPATH', '%s/lib' % ogre_path)

		cnf.env.STLIB_OgreMainStatic = 'OgreMainStatic'
		cnf.env.STLIB_OgreHlmsUnlitStatic = 'OgreHlmsUnlitStatic'
		cnf.env.STLIB_RenderSystem_NULL = 'RenderSystem_NULLStatic'

	if cnf.options.yoga_path:
		yoga_path = cnf.options.yoga_path
		cnf.find_file('yoga/Yoga.h', [yoga_path])
		cnf.env.append_value('INCLUDES', '%s/yoga' % yoga_path)

	cnf.check_cfg(package='cairo', uselib_store='cairo', args=['--cflags', '--libs'])
	cnf.check_cfg(package='pangocairo', uselib_store='pangocairo', args=['--cflags', '--libs'])
	cnf.check_cfg(package='pango', uselib_store='pango', args=['--cflags', '--libs'])

	if cnf.env.DEST_OS == 'linux':
		cnf.check_cfg(package='x11', uselib_store='x11', args=['--cflags', '--libs'])
		cnf.check_cfg(package='xaw7', uselib_store='xaw7', args=['--cflags', '--libs'])
		cnf.check_cfg(package='xt', uselib_store='xt', args=['--cflags', '--libs'])

	cnf.check_cfg(package='zziplib', uselib_store='zzip', args=['--cflags', '--libs'])
	cnf.check_cfg(package='sdl2', uselib_store='sdl2', args=['--cflags', '--libs'])

	cnf.env.LIB_freeimage = 'freeimage'

	# Enable non-POSIX pthreads APIs, which by default are not included in the pthreads header
	cnf.env.append_value('CXXFLAGS', ['-D_XOPEN_SOURCE', '-std=c++11', '-D_DEBUG', '-O0', '-stdlib=libc++', '-D_DARWIN_C_SOURCE'])

	cnf.load('compiler_cxx compiler_c')

from waflib import Task
Task.Task.print_include_absolute_paths = lambda self: '\r\n'.join(['-I%s' % x for x in self.generator.includes_nodes])

def build(bld):
	if bld.path.make_node('ogre').exists():
		bld.recurse('ogre')
	common_source = [
		'flexbox/*.cpp',
	]
	common_source_excludes = [
		'**/*.tests.cpp'
	]
	common_includes = [
		'./',
		'./flexbox'
	]
	common_libs = 'cairo pango pangocairo glib freetype OGRE'

	bld(rule='pwd; echo "${tsk.print_include_absolute_paths()} ${CXXFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${DEFINES_ST:DEFINES} ${CXX_SRC_F}${SRC} ${CPPFLAGS}" > ${TGT}', target='../.clang_complete', features='cxx', name = 'clang_complete', always = True, uselib = 'OGRE', includes = common_includes)

	def app (name):
		bld.program(
			target = name,
			source = bld.path.ant_glob(
				incl = common_source + ['app/*.cpp', 'app/*.mm', 'demo/%s/*.cpp' % name],
				excl = common_source_excludes
			),
			includes = common_includes + [

			],
			uselib = 'cairo pango pangocairo glib freetype freeimage',
			use = 'yoga OgreMainStatic OgreHlmsUnlitStatic RenderSystem_NULL',
			cxxflags = ['-std=c++14', '-D_DARWIN_C_SOURCE', '-g'],
			linkflags = '-stdlib=libc++',
			framework = 'Cocoa'
		)

	app('app1')

	bld.stlib(
		target = 'flexbox',
		source = bld.path.ant_glob(
			incl = common_source,
			excl = common_source_excludes
		),
		includes = common_includes + [

		],
		uselib = 'cairo pango pangocairo glib freetype',
		use = 'yoga OgreMainStatic OgreHlmsUnlitStatic RenderSystem_NULL',
		cxxflags = ['-std=c++14', '-D_DARWIN_C_SOURCE', '-g'],
		linkflags = '-stdlib=libc++'
	)

	bld.program(
		target = 'tests',
		source = bld.path.ant_glob(
			incl = common_source + [
				'test/main.cpp',
				'./**/*.tests.cpp'
			]
		),
		includes = common_includes,
		uselib = 'yoga xt xaw7 zzip sdl2 x11',
		use = 'yoga OgreMainStatic OgreHlmsUnlitStatic RenderSystem_NULL cairo pango pangocairo glib freetype',
		linkflags = ['-stdlib=libc++', '-ldl'],
		framework = 'Cocoa'
	)

	bld.stlib(
		target='yoga',
		source=bld.path.ant_glob('yoga/yoga/*.cpp'),
		includes=['yoga']
	)
