
import os.path as path

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

env.Append(CONFIGUREDIR='#build/conf', CONFIGURELOG='#build/conf/config.log')
# We don't run any configuration tests yet, but we probably will eventually

if str(platform) == "darwin":
	env.Append(CPPFLAGS="-std=c++11 -stdlib=libc++")
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

env.Append(LIBS = bundled_libs + ["z"])

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

