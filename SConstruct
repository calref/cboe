
import os.path as path
import os
import subprocess

platform = ARGUMENTS.get('OS', Platform())
toolset = ARGUMENTS.get('toolset', 'default')
sixty_four = ARGUMENTS.get('64bit', false)
arch = 'x86_64' if sixty_four else 'x86'

if str(platform) not in ("darwin", "win32"):
	print "Sorry, your platform is not supported."
	print "Platform is:", platform
	print "Specify OS=<your-platform> if you believe this is incorrect."
	print "(Supported platforms are: darwin, win32)"
	Exit(1)

print 'Building for:', platform

if toolset != 'default':
	env = Environment(TARGET_ARCH=arch,ENV=os.environ, tools = [toolset])
else:
	env = Environment(TARGET_ARCH=arch,ENV=os.environ)
env.VariantDir('#build/obj', 'src')
env.VariantDir('#build/obj/test', 'test')

# This command generates the header with git revision information
def gen_gitrev(env, target, source):
	revid = subprocess.check_output(["git", "rev-parse", "HEAD"]);
	fulltag = subprocess.check_output(["git", "tag", "--sort=v:refname"]).split('\n')[-1]
	tagrev = subprocess.check_output(["git", "rev-parse", fulltag]) if fulltag else ""
	with open(target[0].path, 'w') as gitrev_hpp:
		print >>gitrev_hpp
		print >>gitrev_hpp, '#define GIT_REVISION "' + revid[0:7] + '"'
		print >>gitrev_hpp, '#define GIT_TAG "' + fulltag + '"'
		print >>gitrev_hpp, '#define GIT_TAG_REVISION "' + tagrev[0:7] + '"'
		print >>gitrev_hpp
if path.exists(".git"):
	git_refs = ['.git/HEAD']
	with open('.git/HEAD') as git_head:
		git_head = git_head.read().split()
		if git_head[0] == 'ref:':
			git_refs.append(path.join('.git', git_head[1]))
	env.Command('src/tools/gitrev.hpp', git_refs, gen_gitrev)
else:
	# Zipped source downloads from github do not include the repo (probably a good thing)
	# TODO: This does not work on Windows
	env.Command('src/tools/gitrev.hpp', '', r"""
		echo -e "\n#define GIT_REVISION \"\"\n#define GIT_TAG \"\"\n#define GIT_TAG_REVISION \"\"\n" > #TARGET
	""")

if str(platform) == "darwin":
	env.Append(CXXFLAGS="-std=c++11 -stdlib=libc++", RPATH='../Frameworks')
	env["CC"] = 'clang'
	env["CXX"] = 'clang++'
	env.Append(BUILDERS={
		'Nib': Builder(
			action = "ibtool --compile $TARGET $SOURCE",
			suffix = ".nib",
			src_suffix = ".xib"
		)
	}, LIBPATH=Split("""
		/usr/lib
		/usr/local/lib
	"""), CPPPATH=Split("""
		/usr/include
		/usr/local/include
	"""), FRAMEWORKPATH=Split("""
		/System/Library/Frameworks
		/Library/Frameworks
		%s/Library/Frameworks
	""" % os.environ['HOME']))
	def build_app_package(env, source, build_dir, info):
		source_name = source[0].name
		pkg_path = path.join(build_dir, "%s.app/Contents/" % source_name)
		nib = env.Nib(info['nib'].replace('rsrc', 'build/obj'), info['nib'])
		icons = [path.join("#rsrc/icons/mac/", x + ".icns") for x in Split(info['icons'])]
		env.Install(path.join(pkg_path, "MacOS"), source)
		env.Install(path.join(pkg_path, "Resources"), icons + nib)
		env.InstallAs(path.join(pkg_path, "Info.plist"), info['plist'])
		env.Command(path.join(pkg_path, 'PkgInfo'), '', action="echo 'APPL%s' > $TARGET" % info['creator'])
	def change_install_name(lib, source, target):
		subprocess.call(["install_name_tool", "-change", source, target, lib])
	system_prefixes = ('/System', '/usr/lib')
	def is_user_lib(lib):
		return all(not lib.startswith(x) for x in system_prefixes)
	def get_deps_for(source):
		deps = subprocess.check_output(['otool', '-L', source]).splitlines()[1:]
		deps = map(str.strip, deps)
		deps = filter(is_user_lib, deps)
		deps = [x.split()[0] for x in deps]
		return deps
	def check_deps(source):
		direct_deps = get_deps_for(source)
		deps = set()
		for i in xrange(len(direct_deps)):
			dep = direct_deps[i]
			if dep.startswith('@rpath/'):
				direct_deps[i] = dep = dep.split('/', 1)[1]
				if dep.startswith('../Frameworks/'):
					direct_deps[i] = dep = dep.split('/', 2)[2]
			else:
				pass#change_install_name(dep, path.join('@rpath', dep), source)
			if dep == source:
				continue
			deps.add(dep)
		return deps
	def split_path(lib):
		while '.' not in path.basename(lib):
			lib = path.dirname(lib)
		return path.dirname(lib), path.basename(lib)
	def bundle_libraries_for(target, source, env):
		if not path.exists(target[0].path):
			Execute(Mkdir(target))
		deps = check_deps(source[0].path)
		for dep in deps:
			if 'framework' in dep:
				paths = Split(env["FRAMEWORKPATH"])
			else:
				paths = env["LIBPATH"]
			for search_path in paths:
				check_path = path.join(search_path, dep)
				if path.exists(check_path):
					check_path = path.realpath(check_path)
					src_dir, basefile = split_path(check_path)
					src_path = path.join(src_dir, basefile)
					dest_path = path.join(target[0].path, basefile)
					if path.exists(dest_path):
						break
					Execute(Copy(dest_path, src_path))
					bundle_libraries_for(target, [File(check_path)], env)
					break
elif str(platform) == "win32":
	if 'msvc' in env['TOOLS']:
		env.Append(
			LINKFLAGS='/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X86',
			CXXFLAGS='/EHsc /MD',
			LIBPATH=("C:\Program Files (x86)\Microsoft Visual Studio " + env['MSVC_VERSION'] + "\VC\lib"),
			LIBS=Split("""
				kernel32
				user32
				gdi32
				winspool
				comdlg32
				advapi32
				shell32
				ole32
				oleaut32
				uuid
				odbc32
				odbccp32
			""")
		)
	def build_app_package(env, source, build_dir, info):
		env.Install(build_dir, source)

env.AddMethod(build_app_package, "Package")

# Allow user to specify additional library/include paths
env.Append(
	LIBPATH = ARGUMENTS.get('LIBPATH', '').split(path.pathsep),
	CPPPATH = ARGUMENTS.get('INCLUDEPATH', '').split(path.pathsep)
)
if str(platform) == 'darwin':
	env.Append(FRAMEWORKPATH=ARGUMENTS.get('FRAMEWORKPATH', '').split(path.pathsep))
	# If any package managers are installed, add their dirs too.
	if subprocess.call(['which', '-s', 'port']) == 0: # MacPorts
		env.Append(
			LIBPATH = '/opt/local/lib',
			CPPPATH = '/opt/local/include',
			FRAMEWORKPATH = '/opt/local/Library/Frameworks'
		)

	if subprocess.call(['which', '-s', 'fink']) == 0: # Fink
		env.Append(
			LIBPATH = '/sw/lib',
			CPPPATH = '/sw/include'
		)

	# pretty sketchy, but should point to your boost install
	if subprocess.call(['which', '-s', 'brew']) == 0: # HomeBrew
		brew_boost_version = '1.58.0'
		env.Append(
			LIBPATH = '/usr/local/Cellar/boost/'+brew_boost_version+'/lib',
			CPPPATH = '/usr/local/Cellar/boost/'+brew_boost_version+'/include')

# Sometimes it's easier just to copy the dependencies into the repo dir
# We try to auto-detect this.
if path.exists('deps/lib'):
	env.Append(LIBPATH='deps/lib')
	if str(platform) == 'darwin':
		env.Append(FRAMEWORKPATH='deps/lib')

if path.exists('deps/include'):
	env.Append(CPPPATH='deps/include')

env['CONFIGUREDIR'] = '#build/conf'
env['CONFIGURELOG'] = '#build/conf/config.log'
if not env.GetOption('clean'):
	conf = Configure(env)

	if not conf.CheckCC() or not conf.CheckCXX():
		print "There's a problem with your compiler!"
		Exit(1)

	if not conf.CheckLib('zlib' if (str(platform) == "win32" and 'mingw' not in env["TOOLS"]) else 'z'):
		print 'zlib must be installed!'
		Exit(1)

	def check_lib(lib, disp, suffixes=[], versions=[]):
		if str(platform) == "win32" and lib.startswith("boost"):
			lib = "lib" + lib
		if "mingw" in env["TOOLS"] and lib.startswith("sfml"):
			lib = "lib" + lib
		possible_names = [lib]
		if str(platform) == "win32":
			if 'msvc' in env['TOOLS']:
				vc_suffix = '-vc' + env['MSVC_VERSION'].replace('.','')
				possible_names.append(lib + vc_suffix)
		n = len(possible_names)
		for i in xrange(n):
			for suff in suffixes:
				possible_names.append(possible_names[i] + suff)
		for test in possible_names:
			if conf.CheckLib(test, language='C++'):
				bundled_libs.append(test)
				return # Success!
			for ver in versions:
				if conf.CheckLib(test + ver, language='C++'):
					bundled_libs.append(test + ver)
					return # Success!
		print disp, 'must be installed!'
		Exit(1)

	def check_header(header, disp):
		if not conf.CheckCXXHeader(header, '<>'):
			print disp, 'must be installed!'
			Exit(1)

	boost_versions = ['-1_55', '-1_56', '-1_57', '-1_58'] # This is a bit of a hack. :(
	bundled_libs = []

	check_header('boost/lexical_cast.hpp', 'Boost.LexicalCast')
	check_header('boost/optional.hpp', 'Boost.Optional')
	check_header('boost/ptr_container/ptr_container.hpp', 'Boost.PointerContainer')
	check_header('boost/any.hpp', 'Boost.Any')
	check_header('boost/math_fwd.hpp', 'Boost.Math')
	check_header('boost/spirit/include/classic.hpp', 'Boost.Spirit.Classic')
	check_lib('boost_system', 'Boost.System', ['-mt'], boost_versions)
	check_lib('boost_filesystem', 'Boost.Filesystem', ['-mt'], boost_versions)
	check_lib('boost_thread', 'Boost.Thread', ['-mt'], boost_versions)
	check_lib('sfml-system', 'SFML-system')
	check_lib('sfml-window', 'SFML-window')
	check_lib('sfml-audio', 'SFML-audio')
	check_lib('sfml-graphics', 'SFML-graphics')

	env = conf.Finish()

env.Append(CPPDEFINES="TIXML_USE_TICPP")

# Include directories

env.Append(CPPPATH=Split("""
	#src/
	#src/classes/
	#src/tools/
	#src/tools/gzstream/
	#src/tools/resmgr/
	#src/dialogxml/
	#src/dialogxml/xml-parser/
"""))

if str(platform) == "win32":
	# For the *resource.h headers
	env.Append(CPPPATH="#rsrc/menus")

if str(platform) == "darwin":
	env.Append(LIBS=Split("""
		objc
		c++
	"""))
	env.Append(FRAMEWORKS=Split("""
		OpenGL
		Cocoa
	"""))
else:
	env.Append(LIBS=Split("""
		opengl32
	"""))

Export("env platform")

# The VariantDir directives near the top mean that the SConscript files are
# copied from src/ and test/ into build/obj/ and build/obj/test respectively.
# Thus, any edits to them should be made to the originals in src/ or test/.
# However, when referencing them we have to reference the copies.

# Gather common sources

common_classes, party_classes = SConscript("build/obj/classes/SConscript")
tools = SConscript("build/obj/tools/SConscript")
dlog_util = SConscript("build/obj/dialogxml/SConscript")
common_sources = common_classes + dlog_util + tools
install_dir = "#build/Blades of Exile"
Export("install_dir party_classes common_sources")

# Programs

SConscript([
	"build/obj/SConscript",
	"build/obj/pcedit/SConscript",
	"build/obj/scenedit/SConscript",
	"build/obj/test/SConscript"
])

# Data files

data_dir = path.join(install_dir, "data")
Export("data_dir")
SConscript(["rsrc/SConscript", "doc/SConscript"])

# Bundle required frameworks and libraries

if str(platform) == "darwin":
	targets = [
		"Blades of Exile",
		"BoE Character Editor",
		"BoE Scenario Editor",
	]
	for targ in targets:
		target_dir = path.join(install_dir, targ + '.app', 'Contents/Frameworks')
		binary = path.join(install_dir, targ + '.app', 'Contents/MacOS', targ)
		env.Command(Dir(target_dir), binary, [Delete(target_dir), bundle_libraries_for])
elif str(platform) == "win32":
	bundled_libs = Split("""
		libsndfile-1
		openal32
		sfml-audio-2
		sfml-graphics-2
		sfml-system-2
		sfml-window-2
		zlib1
	""")
	target_dirs = ["#build/Blades of Exile", "#build/test"]
	for lib in bundled_libs:
		for lpath in env['LIBPATH']:
			src_file = path.join(lpath, lib + ".dll")
			if path.exists(src_file):
				for targ in target_dirs:
					env.Install(targ, src_file)
				break
			elif 'lib' in lpath:
				src_file = path.join(lpath.replace('lib', 'bin'), lib + ".dll")
				if path.exists(src_file):
					for targ in target_dirs:
						env.Install(targ, src_file)
					break
	# Extra: Microsoft redistributable libraries installer
	if 'msvc' in env["TOOLS"]:
		if path.exists("dep/VCRedistInstall.exe"):
			env.Install("build/Blades of Exile/", "dep/VCRedistInstall.exe")
		else:
			print "WARNING: Cannot find installer for the MSVC redistributable libraries for your version of Visual Studio."
			print "Please download it from Microsoft's website and place it at:"
			print "      dep/VCRedistInstall.exe"
			# Create it so its lack doesn't cause makensis to break
			# (Because the installer is an optional component.)
			open("build/Blades of Exile/VCRedistInstall.exe", 'w').close()

if str(platform) == "darwin":
	env.VariantDir("#build/pkg", "pkg/mac")
elif str(platform) == "win32" and subprocess.call(['where', '/Q', 'makensis']) == 0:
	env.VariantDir("#build/pkg", "pkg/win")
SConscript("build/pkg/SConscript")

# Cleanup

env.Clean('.', 'build')
env.Clean('.', '.sconsign.dblite')
