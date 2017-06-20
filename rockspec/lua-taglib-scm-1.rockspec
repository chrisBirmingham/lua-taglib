package = "lua-taglib"
version = "scm-1"
source = {
   url = "*** please add URL for source tarball, zip or repository here ***"
}
description = {
   homepage = "*** please enter a project homepage ***",
   license = "MIT"
}
dependencies = {}
build = {
    type = "builtin",
    modules = {
        ["taglib.tagReader"] = {
            sources = {"src/tagReader.cpp"},
            libraries = {"tag"}
        }
    }
}
