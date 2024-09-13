import os.path as path
import os
import subprocess
import atexit

# Build options
opts = Variables(None, ARGUMENTS)

opts.Add(EnumVariable('OS', "Target platform", str(Platform()), ('darwin', 'win32', 'posix')))
opts.Add('toolset', "Toolset to pass to the SCons builder", 'default')
opts.Add(BoolVariable('debug', "Build with debug symbols and no optimization", False))
opts.Add(EnumVariable('bits', "Build for 32-bit or 64-bit architectures", '64', ('32', '64')))

# Partial build flags -- by default, all targets will be built,
# but if at least one is specified, ONLY the specified targets will be built
partial_options = ('true', 'false', 'default')
opts.Add(EnumVariable('game', 'Build the game', 'default', partial_options))
opts.Add(EnumVariable('pcedit', 'Build the character editor', 'default', partial_options))
opts.Add(EnumVariable('scenedit', 'Build the scenario editor', 'default', partial_options))
opts.Add(EnumVariable('test', 'Build the tests', 'default', partial_options))

# Package build flag -- when explicitly specified, Mac and Windows builds will also
# try to build an installer.
opts.Add(BoolVariable('package', "Build an installer", False))

# Compiler configuration
opts.Add("CXX", "C++ compiler")
opts.Add("CC", "C compiler")
opts.Add("LINK", "Linker")
opts.Add("CCFLAGS", "Custom flags for both the C and C++ compilers")
opts.Add("CXXFLAGS", "Custom flags for the C++ compiler")
opts.Add("CFLAGS", "Custom flags for the C compiler")
opts.Add("LINKFLAGS", "Custom flags for the linker")

# Initialize environment with options and full user environment
env = Environment(variables=opts, ENV=os.environ)
Help(opts.GenerateHelpText(env))

platform = env['OS']
toolset = env['toolset']
arch = 'x86_64' if (env['bits'] == '64') else 'x86'
arch_short = '64' if (env['bits'] == '64') else '86'

# Some kinda gnarly logic required to figure out which targets to build
possible_targets = ['game', 'pcedit', 'scenedit', 'test']
# First, eliminate any which are specified NOT to build
targets = [target for target in possible_targets if env[target] != 'false']

# Then, we will assume the remaining targets should all build by default, UNLESS one
# or more targets are specified TO build.
any_specified_targets=False
for target in targets:
	if env[target] == 'true':
		any_specified_targets = True

if any_specified_targets:
	targets = [target for target in possible_targets if env[target] != 'default']

# Update env based on options
env.Replace(TARGET_ARCH=arch)
env.Replace(tools=[toolset])

# Check for platform support
if platform not in ("darwin", "win32", "posix"):
	print("Sorry, your platform is not supported.")
	print("Platform is:", platform)
	print("Specify OS=<your-platform> if you believe this is incorrect.")
	print("(Supported platforms are: darwin, win32, posix)")
	Exit(1)
print('Building for:', platform)
print('Using toolchain:', toolset)
print('C++ compiler:', env['CXX'])

env.VariantDir('#build/obj', 'src')
env.VariantDir('#build/obj/test', 'test')
env.VariantDir('#build/obj/test/deps', 'deps')

if env['debug']:
	if platform in ['posix', 'darwin']:
		env.Append(CCFLAGS=['-g','-O0'])
	elif platform == 'win32':
		env.Append(CCFLAGS=['/Zi', '/Od'])

# This command generates the header with git revision information
# NOTE: Changes made here must also be made in pkg/gitrev.sh!
def gen_gitrev(env, target, source):
	revid = subprocess.check_output(["git", "rev-parse", "HEAD"], text=True)
	tag_array = subprocess.check_output(["git", "tag", "--sort=v:refname"], text=True).split('\n')
	tag_array = list(filter(None, tag_array))
	fulltag = ""
	if len(tag_array) > 0:
		fulltag = tag_array[-1]
	tagrev = subprocess.check_output(["git", "rev-parse", fulltag], text=True) if fulltag else ""
	status = "clean"
	if subprocess.call(["git", "diff-index", "--quiet", "HEAD", "--"]) != 0:
		status = "modified"
	repo = subprocess.check_output(["git", "remote", "get-url", "origin"], text=True).strip()
	with open(target[0].path, 'w') as gitrev_hpp:
		print(file=gitrev_hpp)
		print('#define GIT_REVISION "' + revid[0:7] + '"', file=gitrev_hpp)
		print('#define GIT_TAG "' + fulltag + '"', file=gitrev_hpp)
		print('#define GIT_TAG_REVISION "' + tagrev[0:7] + '"', file=gitrev_hpp)
		print('#define GIT_STATUS "' + status + '"', file=gitrev_hpp)
		print('#define GIT_REPO "' + repo + '"', file=gitrev_hpp)
		print(file=gitrev_hpp)
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

if platform == "darwin":
	env.Append(CXXFLAGS=["-std=c++11","-stdlib=libc++","-include","global.hpp"], RPATH='../Frameworks')
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
		{SDKROOT}/System/Library/Frameworks
		{SDKROOT}/Library/Frameworks
		{HOME}/Library/Frameworks
	""".format(HOME = os.environ['HOME'], SDKROOT = os.environ['SDKROOT'])))
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
		deps = map(lambda s: s.decode('utf-8'), deps)
		deps = list(map(str.strip, deps))
		deps = list(filter(is_user_lib, deps))
		deps = [x.split()[0] for x in deps]
		return deps
	def check_deps(source):
		direct_deps = get_deps_for(source)
		deps = set()
		for i in range(len(direct_deps)):
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
			if dep.endswith('.framework'):
				paths = Split(env["FRAMEWORKPATH"]) + env["LIBPATH"]
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
					# Copying .frameworks needs to preserve symlinks by using cp -a.
					# SCons provides Copy(symlinks=True) but that doesn't seem to work.
					try:
						print(subprocess.check_output(['cp', '-av', src_path, dest_path], text=True))
					except:
						print('cp -av failed')
						Exit(1)
					# Recursively bundle the dependencies of each dependency:
					bundle_libraries_for(target, [File(check_path)], env)
					break
elif platform == "win32":
	if 'msvc' in env['TOOLS']:
		vcpkg_prefix = path.join((os.environ['HOME'] if 'HOME' in os.environ else 'C:\\'), 'vcpkg')
		vcpkg_installed = path.join(vcpkg_prefix, 'installed', f'x{arch_short}-windows')
		vcpkg_other_packages = Glob(path.join(vcpkg_prefix, 'packages', f'**x{arch_short}-windows'))
		vcpkg_other_includes = [path.join(d.get_abspath(), 'include') for d in vcpkg_other_packages]
		vcpkg_other_includes = list(filter(path.exists, vcpkg_other_includes))
		vcpkg_other_libs = [path.join(d.get_abspath(), 'lib') for d in vcpkg_other_packages]
		vcpkg_other_libs = list(filter(path.exists, vcpkg_other_libs))
		vcpkg_other_bins = [path.join(d.get_abspath(), 'bin') for d in vcpkg_other_packages]
		vcpkg_other_bins = list(filter(path.exists, vcpkg_other_bins))

		include_paths=[path.join(vcpkg_installed, 'include')] + vcpkg_other_includes
		env.Append(
			LINKFLAGS=['/SUBSYSTEM:WINDOWS','/ENTRY:mainCRTStartup',f'/MACHINE:X{arch_short}'],
			CXXFLAGS=['/EHsc','/MD','/FIglobal.hpp'],
			CPPPATH=include_paths,
			LIBPATH=[path.join(vcpkg_installed, 'lib')] + vcpkg_other_libs + vcpkg_other_bins,
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
	else:
		env.Append(CXXFLAGS=["-include","global.hpp"])
	def build_app_package(env, source, build_dir, info):
		env.Install(build_dir, source)
elif platform == "posix":
	env.Append(CXXFLAGS=["-std=c++14","-include","global.hpp"])
	def build_app_package(env, source, build_dir, info):
		env.Install(build_dir, source)

env.AddMethod(build_app_package, "Package")

# Allow user to specify additional library/include paths
env.Append(
	LIBPATH=ARGUMENTS.get('LIBPATH', '').split(path.pathsep),
	CPPPATH=ARGUMENTS.get('INCLUDEPATH', '').split(path.pathsep)
)
if platform == 'darwin':
	env.Append(FRAMEWORKPATH=ARGUMENTS.get('FRAMEWORKPATH', '').split(path.pathsep))
	# If any package managers are installed, add their dirs too.
	if subprocess.call(['which', '-s', 'port']) == 0: # MacPorts
		env.Append(
			LIBPATH=['/opt/local/lib'],
			CPPPATH=['/opt/local/include'],
			FRAMEWORKPATH=['/opt/local/Library/Frameworks']
		)

	if subprocess.call(['which', '-s', 'fink']) == 0: # Fink
		env.Append(
			LIBPATH=['/sw/lib'],
			CPPPATH=['/sw/include']
		)

	# pretty sketchy, but should point to your brew installation directories
	if subprocess.call(['which', '-s', 'brew']) == 0: # HomeBrew
		possible_brew_dirs = ['/usr/local', '/opt/homebrew']
		brew_lib_versions = {
			'boost': '1.85.0',
			'sfml': '2.6.1'
		}
		for dir in possible_brew_dirs:
			if path.exists(f'{dir}/Cellar'):
				for lib, version in brew_lib_versions.items():
					env.Append(
						LIBPATH=[f'{dir}/Cellar/{lib}/{version}/lib'],
						CPPPATH=[f'{dir}/Cellar/{lib}/{version}/include'])

# Sometimes it's easier just to copy the dependencies into the repo dir
# We try to auto-detect this.
if path.exists('deps/lib'):
	env.Append(LIBPATH=[path.join(os.getcwd(), 'deps/lib')])
	if platform == 'darwin':
		env.Append(FRAMEWORKPATH=[path.join(os.getcwd(), 'deps/lib')])

if path.exists('deps/lib64'):
	env.Append(LIBPATH=[path.join(os.getcwd(), 'deps/lib64')])

if path.exists('deps/include'):
	env.Append(CPPPATH=[path.join(os.getcwd(), 'deps/include')])

# Include directories


env.Append(CPPPATH=Split("""
	#src/
	#src/fileio/gzstream/
	#src/fileio/xml-parser/
"""))

env['CONFIGUREDIR'] = '#build/conf'
env['CONFIGURELOG'] = '#build/conf/config.log'

bundled_libs = []
if not env.GetOption('clean'):
	conf = Configure(env)

	if not conf.CheckCC() or not conf.CheckCXX():
		print("There's a problem with your compiler!")
		Exit(1)

	def check_lib(lib, disp, suffixes=[], versions=[], msvc_versions=[]):
		if "mingw" in env["TOOLS"] and lib.startswith("sfml"):
			lib = "lib" + lib
		possible_names = [lib]
		if platform == "win32":
			if 'msvc' in env['TOOLS']:
				msvc_version = env['MSVC_VERSION'].replace('.','')
				vc_suffix = '-vc' + msvc_version
				possible_names.append(lib + vc_suffix)
		n = len(possible_names)
		for i in range(n):
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
		print(disp, 'must be installed!')
		print("  If you're sure it's installed, try passing LIBPATH=...")
		Exit(1)

	def check_header(header, disp):
		if not conf.CheckCXXHeader(header, '<>'):
			print(disp, 'must be installed!')
			print("  If you're sure it's installed, try passing INCLUDEPATH=...")
			Exit(1)

	boost_versions = ['-1_84'] # This is a bit of a hack. :(
	suffixes = ['-mt', f'-mt-x{env["bits"]}']

	zlib = 'zlib' if (platform == "win32" and 'mingw' not in env["TOOLS"]) else 'z'
	check_lib(zlib, 'zlib', [], [])

	check_header('boost/lexical_cast.hpp', 'Boost.LexicalCast')
	check_header('boost/optional.hpp', 'Boost.Optional')
	check_header('boost/ptr_container/ptr_container.hpp', 'Boost.PointerContainer')
	check_header('boost/any.hpp', 'Boost.Any')
	check_header('boost/math_fwd.hpp', 'Boost.Math')
	check_header('boost/spirit/include/classic.hpp', 'Boost.Spirit.Classic')
	check_lib('boost_system', 'Boost.System', suffixes, boost_versions)
	check_lib('boost_filesystem', 'Boost.Filesystem', suffixes, boost_versions)
	sfml_suffixes = ['-d']
	check_lib('sfml-system', 'SFML-system', sfml_suffixes)
	check_lib('sfml-window', 'SFML-window', sfml_suffixes)
	check_lib('sfml-audio', 'SFML-audio', sfml_suffixes)
	check_lib('sfml-graphics', 'SFML-graphics', sfml_suffixes)

	# Make sure Catch2 is cloned
	if not path.exists('deps/Catch2/README.md'):
		subprocess.call(["git", "submodule", "update", "--init", "deps/Catch2"])
	
	# We also use the Clara bundled with Catch in the main program
	env.Append(CPPPATH=[path.join(os.getcwd(), 'deps/Catch2/include/external')])
	
	# Make sure cppcodec is cloned
	if not path.exists('deps/cppcodec/README.md'):
		subprocess.call(["git", "submodule", "update", "--init", "deps/cppcodec"])

	env.Append(CPPPATH=[path.join(os.getcwd(), 'deps/cppcodec')])

	# On Linux, build TGUI from the subtree if necessary
	if platform == 'posix':
		def check_tgui(conf, second_attempt=False):
			if conf.CheckLib('libtgui', language='C++'):
				return conf
			else:
				if second_attempt:
					print('TGUI is missing, even after trying to build it!')
					Exit(1)
				else:
					subprocess.call(["git", "submodule", "update", "--init", "deps/TGUI"])
					subprocess.call(["cmake", "-D", "TGUI_CXX_STANDARD=14", "-D", "CMAKE_INSTALL_PREFIX=../", "."], cwd="deps/TGUI")
					subprocess.call(["make"], cwd="deps/TGUI")
					subprocess.call(["make", "install"], cwd="deps/TGUI")

					env = conf.Finish()
					env.Append(CPPPATH=[path.join(os.getcwd(), 'deps/include')], LIBPATH=[path.join(os.getcwd(), 'deps/lib'), path.join(os.getcwd(), 'deps/lib64')])
					conf = Configure(env)
					return check_tgui(conf, True)
		conf = check_tgui(conf)


	env = conf.Finish()

env.Append(CPPDEFINES=["TIXML_USE_TICPP"])

project_includes = []
for (root, dirs, files) in os.walk('src'):
	project_includes.append(path.join(os.getcwd(), root))

env.Append(CPPPATH=project_includes)

if platform == "win32":
	# For the *resource.h headers
	env.Append(CPPPATH=["#rsrc/menus"])
	# Icons
	env.Append(CPPPATH=["#rsrc/icons/win"])

if platform == "darwin":
	env.Append(LIBS=Split("""
		objc
		c++
	"""))
	env.Append(FRAMEWORKS=Split("""
		OpenGL
		Cocoa
	"""))
elif platform == "win32":
	env.Append(LIBS=Split("""
		opengl32
	"""))
elif platform == "posix":
	env.Append(LIBS=Split("""
		GL
		X11
		tgui
	"""))

Export("env platform")

# The VariantDir directives near the top mean that the SConscript files are
# copied from src/ and test/ into build/obj/ and build/obj/test respectively.
# Thus, any edits to them should be made to the originals in src/ or test/.
# However, when referencing them we have to reference the copies.

# Gather common sources

tools = SConscript("build/obj/tools/SConscript")
dlog_util = SConscript("build/obj/dialogxml/SConscript")
common_sources = dlog_util + tools
install_dir = "#build/Blades of Exile"
party_classes = Glob("#src/universe/*.cpp")
Export("install_dir party_classes common_sources")

# Programs

SConscript([f"build/obj/{target}/SConscript" for target in targets])

# Data files

data_dir = path.join(install_dir, "data")
Export("data_dir")
SConscript(["rsrc/SConscript", "doc/SConscript"])

# Bundle required frameworks and libraries

if platform == "darwin":
	app_targets = []
	if 'game' in targets:
		app_targets.append("Blades of Exile")
	if 'pcedit' in targets:
		app_targets.append("BoE Character Editor")
	if 'scenedit' in targets:
		app_targets.append("BoE Scenario Editor")
	for targ in app_targets:
		target_dir = path.join(install_dir, targ + '.app', 'Contents/Frameworks')
		binary = path.join(install_dir, targ + '.app', 'Contents/MacOS', targ)
		env.Command(Dir(target_dir), binary, [Delete(target_dir), bundle_libraries_for])
elif platform == "win32":
	bundled_libs += Split("""
		libsndfile-1
		openal32
		sfml-audio-2
		sfml-graphics-2
		sfml-system-2
		sfml-window-2
		zlib1
		freetype
		vorbis
		vorbisfile
		vorbisenc
		ogg
		FLAC
		bz2
		brotlidec
		brotlicommon
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
		if path.exists("deps/VCRedistInstall.exe"):
			env.Install("build/Blades of Exile/", "dep/VCRedistInstall.exe")
		else:
			print("WARNING: Cannot find installer for the MSVC redistributable libraries for your version of Visual Studio.")
			print("Please download it from Microsoft's website and place it at:")
			print("      deps/VCRedistInstall.exe")
			# Create it so its lack doesn't cause makensis to break
			# (Because the installer is an optional component.)
			os.makedirs("build/Blades of Exile", exist_ok=True)
			open("build/Blades of Exile/VCRedistInstall.exe", 'w').close()

if env["package"]:
	if platform == "darwin":
		env.VariantDir("#build/pkg", "pkg/mac")
		SConscript("build/pkg/SConscript")
	elif platform == "win32":
		if subprocess.call(['where', '/Q', 'makensis']) == 0:
			env.VariantDir("#build/pkg", "pkg/win")
			SConscript("build/pkg/SConscript")
		else:
			print('NSIS must be installed to generate an installer for Windows.')
			Exit(1)

# Cleanup

env.Clean('.', 'build')
env.Clean('.', Glob('.sconsign.*'))
if env.GetOption('clean'):
	atexit.register(lambda: print('If the build fails immediately after cleaning, delete .sconsign.dblite manually and try again.'))
