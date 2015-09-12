
import os.path as path
import subprocess

platform = ARGUMENTS.get('OS', Platform())

if str(platform) not in ("darwin", "windows"):
	print "Sorry, your platform is not supported."
	print "Platform is:", platform
	print "Specify OS=<your-platform> if you believe this is incorrect."
	print "(Supported platforms are: darwin, windows)"
	Exit(1)

env = Environment()
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
	env.Command('src/tools/gitrev.hpp', '', """
		echo -e "\n#define GIT_REVISION \"\"\n#define GIT_TAG \"\"\n#define GIT_TAG_REVISION \"\"\n" > #TARGET
	""")

if str(platform) == "darwin":
	env.Append(CXXFLAGS="-std=c++11 -stdlib=libc++")
	env["CC"] = 'clang'
	env["CXX"] = 'clang++'
	env.Append(BUILDERS={
		'Nib': Builder(
			action = "ibtool --compile $TARGET $SOURCE",
			suffix = ".nib",
			src_suffix = ".xib"
		)
	})
	def build_app_package(env, source, build_dir, info):
		source_name = source[0].name
		pkg_path = path.join(build_dir, "%s.app/Contents/" % source_name)
		nib = env.Nib(info['nib'].replace('rsrc', 'build/obj'), info['nib'])
		icons = [path.join("#rsrc/icons/mac/", x + ".icns") for x in Split(info['icons'])]
		env.Install(path.join(pkg_path, "MacOS"), source)
		env.Install(path.join(pkg_path, "Resources"), icons + nib)
		env.InstallAs(path.join(pkg_path, "Info.plist"), info['plist'])
		env.Command(path.join(pkg_path, 'PkgInfo'), '', action="echo 'APPL%s' > $TARGET" % info['creator'])
elif str(platform) == "windows":
	def build_app_package(env, source, build_dir, info):
		env.Install("#build/Blades of Exile/", source)

env.AddMethod(build_app_package, "Package")

env['CONFIGUREDIR'] = '#build/conf'
env['CONFIGURELOG'] = '#build/conf/config.log'
conf = Configure(env)

if not conf.CheckLib('z'):
	print 'zlib must be installed!'
	Exit(1)

def check_lib(lib, disp):
	if not conf.CheckLib(lib, language='C++', autoadd=False):
		print disp, 'must be installed!'
		Exit(1)

def check_header(header, disp):
	if not conf.CheckCXXHeader(header, '<>'):
		print disp, 'must be installed!'
		Exit(1)

check_header('boost/lexical_cast.hpp', 'Boost.LexicalCast')
check_header('boost/optional.hpp', 'Boost.Optional')
check_header('boost/ptr_container/ptr_container.hpp', 'Boost.PointerContainer')
check_header('boost/any.hpp', 'Boost.Any')
check_header('boost/math_fwd.hpp', 'Boost.Math')
check_header('boost/spirit/include/classic.hpp', 'Boost.Spirit.Classic')
check_lib('boost_system', 'Boost.System')
check_lib('boost_filesystem', 'Boost.Filesystem')
check_lib('boost_thread', 'Boost.Thread')
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

# Linked libraries

bundled_libs = Split("""
	boost_system
	boost_filesystem
	boost_thread
	sfml-audio
	sfml-graphics
	sfml-system
	sfml-window
""")

env.Append(LIBS = bundled_libs)

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
		GL
	"""))

Export("env platform")

# Gather common sources

common_classes, party_classes = SConscript("src/classes/SConscript")
tools = SConscript("src/tools/SConscript")
dlog_util = SConscript("src/dialogxml/SConscript")
common_sources = common_classes + dlog_util + tools
install_dir = "#build/Blades of Exile"
Export("install_dir party_classes common_sources")

# Programs

# The VariantDir directives near the top mean that the SConscript files are
# copied from src/ and test/ into the corresponding build/obj/ location.
# Thus, any edits to them should be made there.

SConscript([
	"build/obj/SConscript",
	"build/obj/pcedit/SConscript",
	"build/obj/scenedit/SConscript",
	"build/obj/test/SConscript"
])

# Data files

data_dir = path.join(install_dir, "/data")
Export("data_dir")
SConscript(["rsrc/SConscript", "doc/SConscript"])

