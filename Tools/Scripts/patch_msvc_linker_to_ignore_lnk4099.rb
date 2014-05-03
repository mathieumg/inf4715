# This ruby program will patch the linker executable (link.exe)
# so that linker warning LNK4099 is ignorable.
#
# Reference: http://www.bottledlight.com/docs/lnk4099.html

require "fileutils"

def link_exes()
 res = []
 res << File.join(ENV["VS90COMNTOOLS"], "../../VC/bin/link.exe") if ENV["VS90COMNTOOLS"]
 res << File.join(ENV["VS100COMNTOOLS"], "../../VC/bin/link.exe") if ENV["VS100COMNTOOLS"]
 res << File.join(ENV["XEDK"], "bin/win32/link.exe") if ENV["XEDK"]
 return res
end

def patch_link_exe(exe)
 data = nil
 File.open(exe, "rb") {|f| data = f.read}
 unpatched = [4088, 4099, 4105].pack("III")
 patched = [4088, 65535, 4105].pack("III")

 if data.scan(patched).size > 0
  puts "* Already patched #{exe}"
  return
 end

 num_unpatched = data.scan(unpatched).size
 raise "Multiple patch locations in #{exe}" if num_unpatched > 1
 raise "Patch location not found in #{exe}" if num_unpatched == 0

 offset = data.index(unpatched)
 puts "* Found patch location #{exe}:#{offset}"
 bak = exe + "-" + Time.now.strftime("%y%m%d-%H%M%S") + ".bak"
 puts "  Creating backup #{bak}"
 FileUtils.cp(exe, bak)
 puts "  Patching exe"
 data[offset,unpatched.size] = patched
 File.open(exe, "wb") {|f| f.write(data)}
 return true
end

link_exes.each do |exe|
 patch_link_exe(exe)
end