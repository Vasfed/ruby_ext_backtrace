#!/usr/bin/env ruby
#encoding: utf-8

require 'mkmf'
require 'debugger/ruby_core_source'

hdrs = proc {
  res = have_header("vm_core.h") # and have_header("iseq.h")

  if RUBY_VERSION=='1.9.2'
    # on 1.9.3 it fails to link in runtime, but still checks to be ok
    have_var('ruby_current_thread', 'vm_core.h')
  end
  res
}

dir_config("ruby") # allow user to pass in non-standard core include directory

if !Debugger::RubyCoreSource::create_makefile_with_core(hdrs, "ruby_ext_backtrace")
  exit(1)
end
